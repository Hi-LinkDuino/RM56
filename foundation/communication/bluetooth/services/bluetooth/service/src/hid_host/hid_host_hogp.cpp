/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "hid_host_hogp.h"
#include "hid_host_service.h"

namespace OHOS {
namespace bluetooth {
HidHostHogp::HidHostHogp(const std::string &address) : address_(address)
{
    gattCallback_ = std::make_unique<HogpGattClientCallback>(this);
    state_ = HID_HOST_HOGP_STATE_UNUSED;
    appId_ = RegisterGattClientApplication(RawAddress(address_));
    LOG_DEBUG("[HOGP] %{public}s: appId_ %{public}d ", __func__, appId_);
    dispatcher_ = std::make_unique<Dispatcher>(address_);
}

HidHostHogp::~HidHostHogp()
{
    DeregisterGattClientApplication();
    dispatcher_->Uninitialize();
    dispatcher_ = nullptr;
}

int HidHostHogp::RegisterGattClientApplication(const RawAddress &addr)
{
    IProfileGattClient *gattClientService_ = GetGattClientService();
    if (gattClientService_ != nullptr) {
        return gattClientService_->RegisterSharedApplication(
            *gattCallback_, addr, BTTransport::ADAPTER_BLE);
    }
    LOG_ERROR("[Hogp] %{public}s: gattClientService is null. return ", __func__);
    return RET_BAD_STATUS;
}

int HidHostHogp::DeregisterGattClientApplication()
{
    IProfileGattClient *gattClientService_ = GetGattClientService();
    int ret = RET_BAD_STATUS;
    if (gattClientService_ != nullptr) {
        ret = gattClientService_->DeregisterApplication(appId_);
        if (ret == BT_NO_ERROR) {
            appId_ = -1;
        }
        LOG_DEBUG("[HOGP] %{public}s: DeregisterApplication return %{public}d", __func__, ret);
    } else {
        LOG_ERROR("[Hogp] %{public}s: gattClientService is null.", __func__);
    }

    return ret;
}

int HidHostHogp::Connect()
{
    LOG_DEBUG("[HOGP] %{public}s: appId_ %{public}d", __func__, appId_);
    if (appId_ < 0) {
        LOG_ERROR("[HOGP] %{public}s:appId_ is error appId_ = %{public}d", __func__, appId_);
        return RET_BAD_STATUS;
    }
    IProfileGattClient *gattClientService_ = GetGattClientService();
    if (gattClientService_ == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    if (state_ == HID_HOST_HOGP_STATE_CONNECTED) {
        LOG_DEBUG("[HOGP] %{public}s: HOGP is connected state", __func__);
        HidHostMessage event(HID_HOST_OPEN_CMPL_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }
    if (state_ != HID_HOST_HOGP_STATE_UNUSED) {
        LOG_ERROR("[HOGP] %{public}s:state error state = %{public}d", __func__, state_);
        return RET_BAD_STATUS;
    }

    int ret = gattClientService_->Connect(appId_, false);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[HOGP] %{public}s:gatt connect faild ret = %{public}d", __func__, ret);
        return ret;
    }
    state_ = HID_HOST_HOGP_STATE_CONNECTING;
    return ret;
}

int HidHostHogp::Disconnect()
{
    if (appId_ < 0) {
        LOG_ERROR("[HOGP] %{public}s:appId_ is error appId_ = %{public}d", __func__, appId_);
        return RET_BAD_STATUS;
    }
    if (state_ == HID_HOST_CONN_STATE_UNUSED) {
        LOG_DEBUG("[HOGP] %{public}s: HOGP is disconnected state", __func__);
        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    }
    IProfileGattClient *gattClientService_ = GetGattClientService();
    if (gattClientService_ == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    int ret = gattClientService_->Disconnect(appId_);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[HOGP] %{public}s:gatt disconnect faild ret = %{public}d", __func__, ret);
        return ret;
    }
    state_ = HID_HOST_HOGP_STATE_DISCONNECTING;
    return ret;
}

int HidHostHogp::SendData(const HidHostMessage &msg)
{
    dispatcher_->Initialize();
    dispatcher_->PostTask(std::bind(&HidHostHogp::SendData_, this, msg));
    return BT_NO_ERROR;
}

void HidHostHogp::SendData_(const HidHostMessage &msg)
{
    HogpReport* report = FindReportByTypeAndId(msg.sendData_.param, msg.sendData_.reportId);
    if (report == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:not find report", __func__);
    } else if (report->character == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:report character is null", __func__);
    } else {
        switch (msg.sendData_.type) {
            case HID_HOST_DATA_TYPE_GET_REPORT:
                SendGetReport(report->reportId, *report->character);
                break;
            case HID_HOST_DATA_TYPE_SET_REPORT:
                SendSetReport(*report->character, msg.dataLength_, msg.data_.get());
                break;
            default:
                break;
        }
    }
    SendStopHogpThread();
}

void HidHostHogp::SendStopHogpThread()
{
    HidHostMessage event(HID_HOST_HOGP_STOP_THREAD_EVT);
    event.dev_ = address_;
    HidHostService::GetService()->PostEvent(event);
}

int HidHostHogp::SendGetReport(uint8_t reportId, Characteristic character)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
    gattClientService->ReadCharacteristic(appId_, character);
    if (cvfull_.wait_for(lock,
        std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
        LOG_ERROR("[HOGP] %{public}s:get report timeout", __func__);
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
        return RET_BAD_STATUS;
    }
    if ((characteristicTemp_ != nullptr) &&
        (character.handle_ == characteristicTemp_->handle_)) {
        int ret = ReceiveControlData(*characteristicTemp_, reportId);
        characteristicTemp_ = nullptr;
        return ret;
    }
    LOG_ERROR("[HOGP] %{public}s:handle_ is error", __func__);
    ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
    return RET_BAD_STATUS;
}

int HidHostHogp::SendSetReport(Characteristic character, int length, uint8_t* pkt)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
        return RET_BAD_STATUS;
    }
    if (length <= 1) {
        LOG_ERROR("[HOGP] %{public}s:data length is error", __func__);
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
        return RET_BAD_STATUS;
    }
    std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
    // Delete report id from the data
    character.length_ = static_cast<size_t>(length - 1);
    character.value_ = std::make_unique<uint8_t[]>(character.length_);
    if (memcpy_s(character.value_.get(), length - 1, pkt + 1, length - 1) != EOK) {
        LOG_ERROR("[HOGP] %{public}s:character copy error", __func__);
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
        return RET_BAD_STATUS;
    }
    gattClientService->WriteCharacteristic(appId_, character);
    if (cvfull_.wait_for(lock,
        std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
        LOG_ERROR("[HOGP] %{public}s:set report timeout", __func__);
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
        return RET_BAD_STATUS;
    }
    if ((characteristicTemp_ != nullptr) &&
        (character.handle_ == characteristicTemp_->handle_)) {
        ReceiveHandShake(HID_HOST_SUCCESS);
        characteristicTemp_ = nullptr;
        return BT_NO_ERROR;
    }
    LOG_ERROR("[HOGP] %{public}s:handle_ is error", __func__);
    ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
    return RET_BAD_STATUS;
}

void HidHostHogp::ReceiveHandShake(uint16_t err)
{
    HidHostMessage event(HID_HOST_INT_HANDSK_EVT, err);
    event.dev_ = address_;
    HidHostService::GetService()->PostEvent(event);
}

int HidHostHogp::ReceiveControlData(Characteristic character, uint8_t reportId)
{
    if ((character.value_ != nullptr) && (character.length_ != 0)) {
        HidHostMessage event(HID_HOST_INT_CTRL_DATA);
        event.dev_ = address_;
        // Add report id as the first byte of the report before sending it to uhid
        event.dataLength_ = static_cast<int>(character.length_ + 1);
        event.data_ = std::make_unique<uint8_t[]>(event.dataLength_);
        event.data_[0] = reportId;
        if (memcpy_s(event.data_.get() + 1, character.length_,
            character.value_.get(), character.length_) != EOK) {
            LOG_ERROR("[HOGP]%{public}s():memcpy error", __FUNCTION__);
            return RET_BAD_STATUS;
        }
        HidHostService::GetService()->PostEvent(event);
        return BT_NO_ERROR;
    } else {
        LOG_ERROR("[HOGP]%{public}s():data is null length_=%{public}zu", __FUNCTION__, character.length_);
        ReceiveHandShake(HID_HOST_HANDSHAKE_ERROR);
    }
    return RET_BAD_STATUS;
}

HogpReport* HidHostHogp::FindReportByTypeAndId(uint8_t type, uint8_t reportId)
{
    for (auto iter = reports_.begin(); iter != reports_.end(); ++iter) {
        if ((iter->second != nullptr) && (iter->second->reportId == reportId) &&
            (iter->second->reportType == type)) {
            return iter->second.get();
        }
    }
    return nullptr;
}

PnpInformation& HidHostHogp::GetRemotePnpInfo()
{
    return pnpInf_;
}

HidInformation& HidHostHogp::GetRemoteHidInfo()
{
    return hidInf_;
}

IProfileGattClient *HidHostHogp::GetGattClientService()
{
    return static_cast<IProfileGattClient *>(IProfileManager::GetInstance()
        ->GetProfileService(PROFILE_NAME_GATT_CLIENT));
}

int HidHostHogp::DiscoverStart()
{
    LOG_DEBUG("[HOGP]%{public}s", __FUNCTION__);
    int result;
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    result = gattClientService->DiscoveryServices(appId_);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("[HOGP] %{public}s:DiscoveryServices faild result=%{public}d", __func__, result);
        Disconnect();
    } else {
        state_ = HID_HOST_HOGP_STATE_DISCOVERING;
    }
    return result;
}

void HidHostHogp::OnConnectionStateChangedTask(int newState)
{
    LOG_DEBUG("[HOGP]%{public}s state:%{public}d", __FUNCTION__, newState);
    if (newState == static_cast<int>(BTConnectState::CONNECTED)) {
        DiscoverStart();
    } else if (newState == static_cast<int>(BTConnectState::DISCONNECTED)) {
        state_ = HID_HOST_HOGP_STATE_UNUSED;

        HidHostMessage event(HID_HOST_INT_CLOSE_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

void HidHostHogp::OnServicesDiscoveredTask(int status)
{
    OnServicesDiscoveredTask_(status);
    SendStopHogpThread();
}

void HidHostHogp::OnServicesDiscoveredTask_(int status)
{
    LOG_DEBUG("[HOGP]%{public}s state:%{public}d", __FUNCTION__, status);
    if (state_ == HID_HOST_HOGP_STATE_DISCOVERING) {
        if (status != GattStatus::GATT_SUCCESS) {
            LOG_ERROR("[HOGP] %{public}s:discover failed status=%{public}d", __func__, status);
            Disconnect();
            return;
        }
        IProfileGattClient *gattClientService = GetGattClientService();
        if (gattClientService == nullptr) {
            LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
            return;
        }
        std::vector<Service> services = gattClientService->GetServices(appId_);
        int ret = BT_NO_ERROR;
        for (auto &service : services) {
            if (service.uuid_ == Uuid::ConvertFrom16Bits(UUID_DEVICE_INFORMATION_SERVICE)) {
                ret = GetPnpInformation(service);
            } else if (service.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_SERVCLASS_LE_HID)) {
                ret = GetHidInformation(service);
            } else if (service.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_BATTERY_SERVICE)) {
                LOG_INFO("[HOGP] %{public}s:for pts to get battery level.", __func__);
                GetBatteryInformation(service);
            }
            if (ret != BT_NO_ERROR) {
                Disconnect();
                return;
            }
        }
        SetInputReportNotification();
        state_ = HID_HOST_HOGP_STATE_CONNECTED;
        HidHostMessage event(HID_HOST_OPEN_CMPL_EVT);
        event.dev_ = address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

int HidHostHogp::GetPnpInformation(Service service)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    for (auto &character : service.characteristics_) {
        if (character.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_PNP_ID)) {
            std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
            gattClientService->ReadCharacteristic(appId_, character);
            if (cvfull_.wait_for(lock,
                std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
                LOG_ERROR("[HOGP] %{public}s:GetPnpInformation timeout", __func__);
                return RET_BAD_STATUS;
            }
            if ((characteristicTemp_ != nullptr) &&
                (character.handle_ == characteristicTemp_->handle_)) {
                int ret = SavePnpInformation(*characteristicTemp_);
                characteristicTemp_ = nullptr;
                return ret;
            }
            LOG_ERROR("[HOGP] %{public}s:handle_ is error", __func__);
            return RET_BAD_STATUS;
        }
    }
    return RET_BAD_STATUS;
}

int HidHostHogp::GetHidInformation(Service service)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    for (auto &character : service.characteristics_) {
        int ret = BT_NO_ERROR;
        if (character.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_HID_INFORMATION)) {
            std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
            gattClientService->ReadCharacteristic(appId_, character);
            if (cvfull_.wait_for(lock,
                std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
                LOG_ERROR("[HOGP] %{public}s:GetHidInformation timeout", __func__);
                return RET_BAD_STATUS;
            }
            if ((characteristicTemp_ != nullptr) &&
                (character.handle_ == characteristicTemp_->handle_)) {
                ret = SaveHidInformation(*characteristicTemp_);
                characteristicTemp_ = nullptr;
            } else {
                LOG_ERROR("[HOGP] %{public}s:handle_ is error", __func__);
                return RET_BAD_STATUS;
            }
        } else if (character.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_HID_REPORT_MAP)) {
            ret = GetHidReportMap(character);
        } else if (character.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_HID_REPORT)) {
            GetHidReport(character);
            GetExternalCfgInfo(character);
        }
        if (ret != BT_NO_ERROR) {
            return ret;
        }
    }
    return BT_NO_ERROR;
}

void HidHostHogp::GetBatteryInformation(Service service)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return;
    }

    for (auto &character : service.characteristics_) {
        if (character.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_BATTERY_LEVEL)) {
            std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
            gattClientService->ReadCharacteristic(appId_, character);
            if (cvfull_.wait_for(lock,
                std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
                LOG_ERROR("[HOGP] %{public}s: GetBatteryInformation timeout", __func__);
                return;
            }
        }
    }
}

int HidHostHogp::GetHidReportMap(Characteristic character)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return RET_BAD_STATUS;
    }
    {
        std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
        gattClientService->ReadCharacteristic(appId_, character);
        if (cvfull_.wait_for(lock,
            std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
            LOG_ERROR("[HOGP] %{public}s:GetHidReportMap timeout", __func__);
            return RET_BAD_STATUS;
        }
    }

    GetExternalRptRefInfo(character);

    if ((characteristicTemp_ != nullptr) &&
        (character.handle_ == characteristicTemp_->handle_)) {
        int ret = SaveReportMap(*characteristicTemp_);
        characteristicTemp_ = nullptr;
        return ret;
    }
    LOG_ERROR("[HOGP] %{public}s:handle_ is error", __func__);
    return RET_BAD_STATUS;
}

void HidHostHogp::GetExternalRptRefInfo(Characteristic character)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return;
    }

    for (auto &descriptor : character.descriptors_) {
        if (descriptor.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_EXT_RPT_REF)) {
            LOG_INFO("[HOGP] %{public}s:for pts to get external report reference.", __func__);
            std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
            gattClientService->ReadDescriptor(appId_, descriptor);
            if (cvfull_.wait_for(lock,
                std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
                LOG_ERROR("[HOGP] %{public}s:GetExternalRptRefInfo timeout", __func__);
                return;
            }
        }
    }
}

void HidHostHogp::GetExternalCfgInfo(Characteristic character)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return;
    }

    for (auto &descriptor : character.descriptors_) {
        if (descriptor.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_EXT_CFG_REF)) {
            LOG_INFO("[HOGP] %{public}s:for pts to get external report reference.", __func__);
            std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
            gattClientService->ReadDescriptor(appId_, descriptor);
            if (cvfull_.wait_for(lock,
                std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
                LOG_ERROR("[HOGP] %{public}s:GetExternalRptRefInfo timeout", __func__);
                return;
            }
        } else {
            LOG_ERROR("[HOGP] %{public}s:NOT FOUND", __func__);
        }
    }
}

void HidHostHogp::GetHidReport(Characteristic character)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return;
    }
    Descriptor confifDefault;
    std::unique_ptr<Descriptor> config = std::make_unique<Descriptor>(confifDefault);
    std::unique_ptr<Descriptor> reference = nullptr;
    for (auto &descriptor : character.descriptors_) {
        if (descriptor.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_REPORT_REFERENCE)) {
            reference = std::make_unique<Descriptor>(descriptor);
        } else if (descriptor.uuid_ == Uuid::ConvertFrom16Bits(HID_HOST_UUID_GATT_CLIENT_CHAR_CONFIG)) {
            config = std::make_unique<Descriptor>(descriptor);
        }
    }

    if ((config == nullptr) || (reference == nullptr)) {
        LOG_ERROR("[HOGP] %{public}s:not find config or reference", __func__);
        return;
    }

    std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
    gattClientService->ReadDescriptor(appId_, *reference);
    if (cvfull_.wait_for(lock,
        std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
        LOG_ERROR("[HOGP] %{public}s:GetHidReport timeout", __func__);
        return;
    }

    if (descriptorTemp_ != nullptr) {
        SaveReport(character, *descriptorTemp_, *config);
        descriptorTemp_ = nullptr;
    } else {
        LOG_ERROR("[HOGP] %{public}s:handle_ is error", __func__);
    }
}

int HidHostHogp::SavePnpInformation(Characteristic character)
{
    if (character.length_ != HID_HOST_PNP_ID_SIZE) {
        LOG_ERROR("[HOGP] %{public}s:character length_ is error length=%{public}zu",
            __func__, character.length_);
        return RET_BAD_STATUS;
    }
    uint8_t *data = character.value_.get();
    int offset = 1;
    pnpInf_.vendorId = static_cast<uint16_t>(data[offset]) +
        static_cast<uint16_t>(static_cast<uint16_t>(data[offset + 1]) << HID_HOST_SHIFT_OPRATURN_8);
    offset += sizeof(uint16_t);
    pnpInf_.productId = static_cast<uint16_t>(data[offset]) +
        static_cast<uint16_t>(static_cast<uint16_t>(data[offset + 1]) << HID_HOST_SHIFT_OPRATURN_8);
    offset += sizeof(uint16_t);
    pnpInf_.version = static_cast<uint16_t>(data[offset]) +
        static_cast<uint16_t>(static_cast<uint16_t>(data[offset + 1]) << HID_HOST_SHIFT_OPRATURN_8);
    LOG_DEBUG(
        "[HOGP]%{public}s():vendorId = 0x%{public}x,productId = 0x%{public}x,version = 0x%{public}x",
        __FUNCTION__, pnpInf_.vendorId, pnpInf_.productId, pnpInf_.version);
    return BT_NO_ERROR;
}

int HidHostHogp::SaveHidInformation(Characteristic character)
{
    if (character.length_ != HID_HOST_HID_INFORMATION_SIZE) {
        LOG_ERROR("[HOGP] %{public}s:character length_ is error length=%{public}zu",
            __func__, character.length_);
        return RET_BAD_STATUS;
    }
    hidInf_.ctryCode = *(character.value_.get() + HID_HOST_CTRY_CODE_OFFSET);
    LOG_DEBUG("[HOGP]%{public}s():ctryCode = 0x%{public}x", __FUNCTION__, hidInf_.ctryCode);
    return BT_NO_ERROR;
}

int HidHostHogp::SaveReportMap(Characteristic character)
{
    if (character.length_ <= 0) {
        LOG_ERROR("[HOGP] %{public}s:character length_ is error length=%{public}zu",
            __func__, character.length_);
        return RET_BAD_STATUS;
    }
    hidInf_.descInfo = std::make_unique<uint8_t[]>(character.length_);
    if (memcpy_s(hidInf_.descInfo.get(), character.length_, character.value_.get(), character.length_) != EOK) {
        hidInf_.descInfo = nullptr;
        hidInf_.descLength = 0;
        LOG_ERROR("[HOGP] %{public}s:info copy is error", __func__);
        return RET_BAD_STATUS;
    }
    hidInf_.descLength = character.length_;
    LOG_DEBUG("[HOGP]%{public}s():descLength = 0x%{public}x", __FUNCTION__, hidInf_.descLength);
    return BT_NO_ERROR;
}

void HidHostHogp::SaveReport(Characteristic character, Descriptor descriptor, Descriptor config)
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return;
    }
    if (descriptor.length_ != HID_HOST_REPORT_REFERENCE_SIZE) {
        LOG_ERROR("[HOGP] %{public}s:descriptor length_ is error length=%{public}zu",
            __func__, character.length_);
        return;
    }

    reports_[character.handle_] = std::make_unique<HogpReport>();
    reports_[character.handle_]->reportId = descriptor.value_[0];
    reports_[character.handle_]->reportType = descriptor.value_[1];
    reports_[character.handle_]->character = std::make_unique<Characteristic>(character);
    reports_[character.handle_]->config = std::make_unique<Descriptor>(config);
    LOG_DEBUG("[HOGP]%{public}s():reportId = 0x%{public}x,reportType = 0x%{public}x", __FUNCTION__,
        reports_[character.handle_]->reportId, reports_[character.handle_]->reportType);
}

void HidHostHogp::SetInputReportNotification()
{
    IProfileGattClient *gattClientService = GetGattClientService();
    if (gattClientService == nullptr) {
        LOG_ERROR("[HOGP] %{public}s:gattClientService is null.", __func__);
        return;
    }

    for (auto iter = reports_.begin(); iter != reports_.end(); ++iter) {
        if ((iter->second == nullptr) || (iter->second->reportType != HID_HOST_INPUT_REPORT)) {
            break;
        }
        if ((iter->second->config == nullptr) || (iter->second->config->handle_ == 0)) {
            LOG_ERROR("[HOGP] %{public}s:config is error ", __func__);
            break;
        }
        std::unique_lock<std::mutex> lock(mutexWaitGattCallback_);
        iter->second->config->length_ = HID_HOST_CLIENT_CHAR_CONFIG_SIZE;
        iter->second->config->value_ = std::make_unique<uint8_t[]>(HID_HOST_CLIENT_CHAR_CONFIG_SIZE);
        iter->second->config->value_[0] = 1;
        iter->second->config->value_[1] = 0;
        gattClientService->WriteDescriptor(appId_, *iter->second->config);
        if (cvfull_.wait_for(lock,
            std::chrono::seconds(HOGP_GATT_THREAD_WAIT_TIMEOUT)) == std::cv_status::timeout) {
            LOG_ERROR("[HOGP] %{public}s:set report notifycation timeout", __func__);
        }
    }
}

void HidHostHogp::ProcessEvent(const HidHostMessage &event)
{
    LOG_DEBUG("[HOGP]%{public}s(): event %{public}s[%{public}d]",
        __FUNCTION__, HidHostHogp::GetEventName(event.what_).c_str(), event.what_);

    switch (event.what_) {
        case HID_HOST_HOGP_CONNECTION_STATE_CHANGED_EVT:
            OnConnectionStateChangedTask(event.arg1_);
            break;
        case HID_HOST_HOGP_SERVICES_DISCOVERED_EVT:
            dispatcher_->Initialize();
            dispatcher_->PostTask(std::bind(&HidHostHogp::OnServicesDiscoveredTask, this, event.arg1_));
            break;
        case HID_HOST_HOGP_STOP_THREAD_EVT:
            dispatcher_->Uninitialize();
            break;
        default:
            break;
    }
}

std::string HidHostHogp::GetEventName(int what)
{
    switch (what) {
        case HID_HOST_HOGP_CONNECTION_STATE_CHANGED_EVT:
            return "HID_HOST_HOGP_CONNECTION_STATE_CHANGED_EVT";
        case HID_HOST_HOGP_SERVICES_DISCOVERED_EVT:
            return "HID_HOST_HOGP_SERVICES_DISCOVERED_EVT";
        case HID_HOST_HOGP_STOP_THREAD_EVT:
            return "HID_HOST_HOGP_STOP_THREAD_EVT";
        default:
            return "Unknown";
    }
}

HidHostHogp::HogpGattClientCallback::HogpGattClientCallback(HidHostHogp *hogp) : hogp_(hogp)
{}

void HidHostHogp::HogpGattClientCallback::OnConnectionStateChanged(int state, int newState)
{
    LOG_DEBUG("[HOGP]%{public}s state:%{public}d newState:%{public}d", __FUNCTION__, state, newState);
    if (state != newState) {
        HidHostMessage event(HID_HOST_HOGP_CONNECTION_STATE_CHANGED_EVT, newState);
        event.dev_ = hogp_->address_;
        HidHostService::GetService()->PostEvent(event);
    }
}

void HidHostHogp::HogpGattClientCallback::OnServicesChanged(const std::vector<Service> &services)
{
    LOG_DEBUG("[HOGP]%{public}s", __FUNCTION__);
}

void HidHostHogp::HogpGattClientCallback::OnCharacteristicRead(int ret, const Characteristic &characteristic)
{
    LOG_DEBUG("[HOGP]%{public}s ret:%{public}d", __FUNCTION__, ret);
    std::lock_guard<std::mutex> lock(hogp_->mutexWaitGattCallback_);
    if (ret != GattStatus::GATT_SUCCESS) {
        LOG_ERROR("[HOGP] %{public}s:ret=%{public}d", __func__, ret);
        hogp_->characteristicTemp_ = nullptr;
    } else {
        hogp_->characteristicTemp_ = std::make_unique<Characteristic>(characteristic);
    }
    hogp_->cvfull_.notify_all();
}

void HidHostHogp::HogpGattClientCallback::OnCharacteristicWrite(int ret, const Characteristic &characteristic)
{
    LOG_DEBUG("[HOGP]%{public}s ret:%{public}d", __FUNCTION__, ret);
    std::lock_guard<std::mutex> lock(hogp_->mutexWaitGattCallback_);
    if (ret != GattStatus::GATT_SUCCESS) {
        LOG_ERROR("[HOGP] %{public}s:ret=%{public}d", __func__, ret);
        hogp_->characteristicTemp_ = nullptr;
    } else {
        hogp_->characteristicTemp_ = std::make_unique<Characteristic>(characteristic);
    }
    hogp_->cvfull_.notify_all();
}

void HidHostHogp::HogpGattClientCallback::OnCharacteristicChanged(const Characteristic &characteristic)
{
    LOG_DEBUG("[HOGP]%{public}s", __FUNCTION__);
    uint8_t reportId = 0;
    auto it = hogp_->reports_.find(characteristic.handle_);
    if ((it != hogp_->reports_.end()) && (it->second != nullptr)) {
        reportId = it->second->reportId;
    } else {
        LOG_ERROR("[HOGP]%{public}s():not find Characteristic[%{public}d]", __FUNCTION__, characteristic.handle_);
        return;
    }

    if ((characteristic.value_ != nullptr) && (characteristic.length_ != 0)) {
        HidHostMessage event(HID_HOST_INT_DATA_EVT);
        event.dev_ = hogp_->address_;
        int offset = 0;
        if (reportId != 0) {
            offset++;
            event.dataLength_ = static_cast<int>(characteristic.length_ + offset);
            event.data_ = std::make_unique<uint8_t[]>(event.dataLength_);
            event.data_[0] = reportId;
        } else {
            event.dataLength_ = static_cast<int>(characteristic.length_);
            event.data_ = std::make_unique<uint8_t[]>(event.dataLength_);
        }
        if (memcpy_s(event.data_.get() + offset, characteristic.length_,
            characteristic.value_.get(), characteristic.length_) != EOK) {
            LOG_ERROR("[HOGP]%{public}s():memcpy error", __FUNCTION__);
            return;
        }
        HidHostService::GetService()->PostEvent(event);
    } else {
        LOG_ERROR("[HOGP]%{public}s():data is null length_=%{public}zu", __FUNCTION__, characteristic.length_);
    }
}

void HidHostHogp::HogpGattClientCallback::OnDescriptorRead(int ret, const Descriptor &descriptor)
{
    LOG_DEBUG("[HOGP]%{public}s ret:%{public}d, descriptor=%{public}d", __FUNCTION__, ret, descriptor.handle_);
    std::lock_guard<std::mutex> lock(hogp_->mutexWaitGattCallback_);
    if (ret != GattStatus::GATT_SUCCESS) {
        LOG_ERROR("[HOGP] %{public}s:ret=%{public}d", __func__, ret);
        hogp_->descriptorTemp_ = nullptr;
    } else {
        hogp_->descriptorTemp_ = std::make_unique<Descriptor>(descriptor);
    }
    hogp_->cvfull_.notify_all();
}

void HidHostHogp::HogpGattClientCallback::OnDescriptorWrite(int ret, const Descriptor &descriptor)
{
    LOG_DEBUG("[HOGP]%{public}s ret:%{public}d", __FUNCTION__, ret);
    std::lock_guard<std::mutex> lock(hogp_->mutexWaitGattCallback_);
    if (ret != GattStatus::GATT_SUCCESS) {
        LOG_ERROR("[HOGP] %{public}s:ret=%{public}d", __func__, ret);
    }
    hogp_->cvfull_.notify_all();
}

void HidHostHogp::HogpGattClientCallback::OnMtuChanged(int state, int mtu)
{
    LOG_DEBUG("[HOGP]%{public}s state:%{public}d, mtu:%{public}d", __FUNCTION__, state, mtu);
}

void HidHostHogp::HogpGattClientCallback::OnConnectionParameterChanged(
    int interval, int latency, int timeout, int status)
{
    LOG_DEBUG("[HOGP]%{public}s state:%{public}d, timeout:%{public}d", __FUNCTION__, status, timeout);
}

void HidHostHogp::HogpGattClientCallback::OnServicesDiscovered(int status)
{
    LOG_DEBUG("[HOGP]%{public}s state:%{public}d", __FUNCTION__, status);
    HidHostMessage event(HID_HOST_HOGP_SERVICES_DISCOVERED_EVT, status);
    event.dev_ = hogp_->address_;
    HidHostService::GetService()->PostEvent(event);
}
}  // namespace bluetooth
}  // namespace OHOS
