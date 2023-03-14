/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "vendor_interface.h"

#include <thread>

#include <dlfcn.h>

#include <hdf_log.h>
#include <securec.h>

#include "bluetooth_address.h"
#include "bt_hal_constant.h"
#include "h4_protocol.h"
#include "mct_protocol.h"

namespace OHOS {
namespace HDI {
namespace BT {
namespace V1_0 {
constexpr int MAX_BUFFER_SIZE = 1024;
bt_vendor_callbacks_t VendorInterface::vendorCallbacks_ = {
    .size = sizeof(bt_vendor_callbacks_t),
    .init_cb = VendorInterface::OnInitCallback,
    .alloc = VendorInterface::OnMallocCallback,
    .dealloc = VendorInterface::OnFreeCallback,
    .xmit_cb = VendorInterface::OnCmdXmitCallback,
};

VendorInterface::VendorInterface()
{}

VendorInterface::~VendorInterface()
{
    CleanUp();
}

bool VendorInterface::WatchHciChannel(const ReceiveCallback &receiveCallback)
{
    int channel[HCI_MAX_CHANNEL] = {0};
    int channelCount = vendorInterface_->op(bt_opcode_t::BT_OP_HCI_CHANNEL_OPEN, channel);
    if (channelCount < 1 || channelCount > HCI_MAX_CHANNEL) {
        HDF_LOGE("vendorInterface_->op BT_OP_HCI_CHANNEL_OPEN failed ret:%d.", channelCount);
        return false;
    }

    if (channelCount == 1) {
        auto h4 = std::make_shared<HCI::H4Protocol>(channel[0],
            receiveCallback.onAclReceive,
            receiveCallback.onScoReceive,
            std::bind(&VendorInterface::OnEventReceived, this, std::placeholders::_1));
        watcher_.AddFdToWatcher(channel[0], std::bind(&HCI::H4Protocol::ReadData, h4, std::placeholders::_1));
        hci_ = h4;
    } else {
        auto mct = std::make_shared<HCI::MctProtocol>(channel,
            receiveCallback.onAclReceive,
            receiveCallback.onScoReceive,
            std::bind(&VendorInterface::OnEventReceived, this, std::placeholders::_1));
        watcher_.AddFdToWatcher(
            channel[hci_channels_t::HCI_ACL_IN], std::bind(&HCI::MctProtocol::ReadAclData, mct, std::placeholders::_1));
        watcher_.AddFdToWatcher(
            channel[hci_channels_t::HCI_EVT], std::bind(&HCI::MctProtocol::ReadEventData, mct, std::placeholders::_1));
        hci_ = mct;
    }

    return true;
}

bool VendorInterface::Initialize(
    InitializeCompleteCallback initializeCompleteCallback, const ReceiveCallback &receiveCallback)
{
    HDF_LOGI("%{public}s, ", __func__);
    initializeCompleteCallback_ = initializeCompleteCallback;
    eventDataCallback_ = receiveCallback.onEventReceive;

    vendorHandle_ = dlopen(VENDOR_NAME.c_str(), RTLD_NOW);
    if (vendorHandle_ == nullptr) {
        HDF_LOGE("dlopen %s failed.", VENDOR_NAME.c_str());
        return false;
    }

    vendorInterface_ =
        reinterpret_cast<bt_vendor_interface_t *>(dlsym(vendorHandle_, VENDOR_INTERFACE_SYMBOL_NAME.c_str()));
    if (vendorInterface_ == nullptr) {
        HDF_LOGE("dlsym %s failed.", VENDOR_INTERFACE_SYMBOL_NAME.c_str());
        return false;
    }

    auto bluetoothAddress = BluetoothAddress::GetDeviceAddress();
    std::vector<uint8_t> address = { 0, 0, 0, 0, 0, 0 };
    if (bluetoothAddress != nullptr) {
        bluetoothAddress->ReadAddress(address);
    }

    int result = vendorInterface_->init(&vendorCallbacks_, address.data());
    if (result != 0) {
        HDF_LOGE("vendorInterface_->init failed.");
        return false;
    }

    result = vendorInterface_->op(bt_opcode_t::BT_OP_POWER_ON, nullptr);
    if (result != 0) {
        HDF_LOGE("vendorInterface_->op BT_OP_POWER_ON failed.");
        return false;
    }

    if (!WatchHciChannel(receiveCallback)) {
        return false;
    }

    if (!watcher_.Start()) {
        HDF_LOGE("watcher start failed.");
        return false;
    }

    vendorInterface_->op(bt_opcode_t::BT_OP_INIT, nullptr);

    return true;
}

void VendorInterface::CleanUp()
{
    if (vendorInterface_ == nullptr) {
        return;
    }

    watcher_.Stop();

    vendorInterface_->op(bt_opcode_t::BT_OP_LPM_DISABLE, nullptr);
    vendorInterface_->op(bt_opcode_t::BT_OP_HCI_CHANNEL_CLOSE, nullptr);
    vendorInterface_->op(bt_opcode_t::BT_OP_POWER_OFF, nullptr);
    vendorInterface_->close();

    hci_ = nullptr;
    vendorInterface_ = nullptr;
    initializeCompleteCallback_ = nullptr;
    eventDataCallback_ = nullptr;
    dlclose(vendorHandle_);
}

size_t VendorInterface::SendPacket(HCI::HciPacketType type, const std::vector<uint8_t> &packet)
{
    {
        std::lock_guard<std::mutex> lock(wakeupMutex_);
        activity_ = true;
        watcher_.SetTimeout(std::chrono::milliseconds(lpmTimer_), std::bind(&VendorInterface::WatcherTimeout, this));
        if (!wakeupLock_) {
            vendorInterface_->op(bt_opcode_t::BT_OP_WAKEUP_LOCK, nullptr);
            wakeupLock_ = true;
        }
    }

    return hci_->SendPacket(type, packet);
}

void VendorInterface::OnInitCallback(bt_op_result_t result)
{
    HDF_LOGI("%{public}s, ", __func__);
    if (VendorInterface::GetInstance()->initializeCompleteCallback_) {
        VendorInterface::GetInstance()->initializeCompleteCallback_(result == BTC_OP_RESULT_SUCCESS);
        VendorInterface::GetInstance()->initializeCompleteCallback_ = nullptr;
    }

    uint32_t lpmTimer = 0;
    if (VendorInterface::GetInstance()->vendorInterface_->op(bt_opcode_t::BT_OP_GET_LPM_TIMER, &lpmTimer) != 0) {
        HDF_LOGE("Vector interface BT_OP_GET_LPM_TIMER failed");
    }
    VendorInterface::GetInstance()->lpmTimer_ = lpmTimer;

    VendorInterface::GetInstance()->vendorInterface_->op(bt_opcode_t::BT_OP_LPM_ENABLE, nullptr);

    VendorInterface::GetInstance()->watcher_.SetTimeout(std::chrono::milliseconds(lpmTimer),
        std::bind(&VendorInterface::WatcherTimeout, VendorInterface::GetInstance()));
}

void *VendorInterface::OnMallocCallback(int size)
{
    if (size <= 0 || size > MAX_BUFFER_SIZE) {
        HDF_LOGE("%{public}s, size is invalid", __func__);
        return nullptr;
    }
    return malloc(size);
}

void VendorInterface::OnFreeCallback(void *buf)
{
    if (buf != nullptr) {
        free(buf);
    }
}

size_t VendorInterface::OnCmdXmitCallback(uint16_t opcode, void *buf)
{
    HC_BT_HDR *hdr = reinterpret_cast<HC_BT_HDR *>(buf);

    VendorInterface::GetInstance()->vendorSentOpcode_ = opcode;

    return VendorInterface::GetInstance()->SendPacket(
        HCI::HCI_PACKET_TYPE_COMMAND, std::vector<uint8_t>(hdr->data, hdr->data + hdr->len));
}

void VendorInterface::OnEventReceived(const std::vector<uint8_t> &data)
{
    if (data[0] == HCI::HCI_EVENT_CODE_VENDOR_SPECIFIC) {
        size_t buffSize = sizeof(HC_BT_HDR) + data.size();
        HC_BT_HDR *buff = reinterpret_cast<HC_BT_HDR *>(new uint8_t[buffSize]);
        buff->event = data[0];
        buff->len = data.size();
        buff->offset = 0;
        buff->layer_specific = 0;
        (void)memcpy_s(buff->data, buffSize - sizeof(HC_BT_HDR), data.data(), data.size());
        vendorInterface_->op(bt_opcode_t::BT_OP_EVENT_CALLBACK, buff);
        delete[] buff;
    } else if (vendorSentOpcode_ != 0 && data[0] == HCI::HCI_EVENT_CODE_COMMAND_COMPLETE) {
        uint8_t opcodeOffset = hci_->GetPacketHeaderInfo(HCI::HCI_PACKET_TYPE_EVENT).headerSize + 1;
        uint16_t opcode = data[opcodeOffset] + (data[opcodeOffset + 1] << 0x08);
        if (opcode == vendorSentOpcode_) {
            size_t buffSize = sizeof(HC_BT_HDR) + data.size();
            HC_BT_HDR *buff = reinterpret_cast<HC_BT_HDR *>(new uint8_t[buffSize]);
            buff->event = data[0];
            buff->len = data.size();
            buff->offset = 0;
            buff->layer_specific = 0;
            (void)memcpy_s(buff->data, buffSize - sizeof(HC_BT_HDR), data.data(), data.size());
            vendorSentOpcode_ = 0;
            vendorInterface_->op(bt_opcode_t::BT_OP_EVENT_CALLBACK, buff);
            delete[] buff;
        }
    }

    eventDataCallback_(data);
}

void VendorInterface::WatcherTimeout()
{
    std::lock_guard<std::mutex> lock(wakeupMutex_);
    if (!activity_ && wakeupLock_) {
        vendorInterface_->op(bt_opcode_t::BT_OP_WAKEUP_UNLOCK, nullptr);
        wakeupLock_ = false;
    }
    activity_ = false;
}
}  // namespace V1_0
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS