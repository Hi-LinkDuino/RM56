/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "ble_properties.h"

#include "ble_defs.h"

#include "bt_uuid.h"
#include "btstack.h"
#include "gap_if.h"
#include "log.h"
#include "raw_address.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
struct BleProperties::impl {
    std::string deviceName_{BLE_DEFAULT_DEVICE_NAME};
    BaseObserverList<IAdapterBleObserver> *observer_ = nullptr;
    int ioCapability_ = BLE_DEFAULT_IO;
    std::string passkey_{BLE_DEFAULT_LOCAL_PASSKEY};
    int bondableMode_ = BLE_BONDABLE_MODE_NONE;
    std::string macAddr_{BLE_INVALID_MAC_ADDRESS};
};

BleProperties &BleProperties::GetInstance()
{
    static BleProperties instance;
    return instance;
}

BleProperties::BleProperties() : pimpl(std::make_unique<BleProperties::impl>())
{}

BleProperties::~BleProperties()
{}

std::string BleProperties::GetLocalName() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return pimpl->deviceName_;
}

bool BleProperties::SetLocalName(const std::string &name) const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    int length = name.length();
    std::string newName = name;

    if (name.empty()) {
        return false;
    }

    if (length >= DEVICE_NAME_MAX_LEN) {
        length = DEVICE_NAME_MAX_LEN;
        newName = name.substr(0, length);
    }

    pimpl->deviceName_ = BleConfig::GetInstance().GetLocalName();
    if (newName != pimpl->deviceName_) {
        pimpl->deviceName_ = newName;
        int type = BLE_CONFIG_LOCAL_NAME;
        UpdateConfig(type);
    }
    return true;
}

std::string BleProperties::GetLocalAddress() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);
    return pimpl->macAddr_;
}

int BleProperties::GetBondableMode() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return pimpl->bondableMode_;
}

int BleProperties::SetBondableMode(int mode) const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    if (pimpl->bondableMode_ == mode) {
        return BT_NO_ERROR;
    }
    switch (mode) {
        case BLE_BONDABLE_MODE_NONE:
            pimpl->bondableMode_ = GAP_BONDABLE_MODE_NON;
            break;
        case BLE_BONDABLE_MODE_ON:
            pimpl->bondableMode_ = GAP_BONDABLE_MODE;
            break;
        default:
            pimpl->bondableMode_ = GAP_BONDABLE_MODE_NON;
            break;
    }
    return GAPIF_LeSetBondMode(pimpl->bondableMode_);
}

std::string BleProperties::GetPasskey() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return pimpl->passkey_;
}

int BleProperties::GetIoCapability() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return pimpl->ioCapability_;
}

bool BleProperties::SetIoCapability(int ioCapability) const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    pimpl->ioCapability_ = ioCapability;
    return true;
}

bool BleProperties::GetAddrFromController() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    BtAddr btAddr;
    (void)memset_s(&btAddr, sizeof(btAddr), 0x00, sizeof(btAddr));
    int ret = GAPIF_GetLocalAddr(&btAddr);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("BleProperties::GAP_GetLocalAddr Failed");
        return false;
    }
    RawAddress addr = RawAddress::ConvertToString(btAddr.addr);
    pimpl->macAddr_ = addr.GetAddress();
    return UpdateConfig(BLE_CONFIG_LOCAL_ADDRESS);
}

bool BleProperties::UpdateConfig(int type) const
{
    LOG_DEBUG("[BleProperties] %{public}s:Type = %{public}d", __func__, type);

    bool ret = BT_OPERATION_FAILED;
    switch (type) {
        case BLE_CONFIG_LOCAL_NAME:
            ret = BleConfig::GetInstance().SetLocalName(pimpl->deviceName_);
            if (pimpl->observer_ != nullptr) {
                std::string deviceName = pimpl->deviceName_;
                pimpl->observer_->ForEach(
                    [deviceName](IAdapterBleObserver &observer) { observer.OnDeviceNameChanged(deviceName); });
            }
            break;
        case BLE_CONFIG_LOCAL_ADDRESS:
            BleConfig::GetInstance().SetLocalAddress(pimpl->macAddr_);
            ret = BleConfig::GetInstance().SetBleLocalAddrType(BLE_ADDR_TYPE::BLE_ADDR_TYPE_PUBLIC);
            if (pimpl->observer_ != nullptr) {
                std::string macAddr = pimpl->macAddr_;
                pimpl->observer_->ForEach(
                    [macAddr](IAdapterBleObserver &observer) { observer.OnDeviceAddrChanged(macAddr); });
            }
            break;
        default:
            break;
    }

    ret &= BleConfig::GetInstance().Save();
    return ret;
}

bool BleProperties::LoadBleConfigInfo() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    bool ret = BleConfig::GetInstance().LoadConfigInfo();
    if (!ret) {
        LOG_ERROR("[BleProperties] %{public}s:%{public}s", __func__, "Load device config file failed!");
    }
    ReadBleHostInfo();
    return ret;
}

bool BleProperties::ConfigBleProperties() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return SetLocalName(pimpl->deviceName_);
}

int BleProperties::GetBleRoles()
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return BleConfig::GetInstance().GetBleRoles();
}

bool BleProperties::SetBleRoles(uint8_t roles)
{
    LOG_DEBUG("[BleProperties] %{public}s:%u", __func__, roles);
    int ret = GAPIF_LeSetRole(roles);
    if (ret != BT_NO_ERROR) {
        LOG_ERROR("[BleProperties] %{public}s:%{public}s", __func__, "Set ble roles failed!");
    }

    return BleConfig::GetInstance().SetBleRoles(roles);
}

void BleProperties::ReadBleHostInfo() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    pimpl->deviceName_ = BleConfig::GetInstance().GetLocalName();
    pimpl->ioCapability_ = BleConfig::GetInstance().GetIoCapability();
    pimpl->passkey_ = BleConfig::GetInstance().GetLoaclPasskey();
}

int BleProperties::GetAppearance()
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    return BleConfig::GetInstance().GetAppearance();
}

bool BleProperties::SetPasskey(const std::string &passkey)
{
    LOG_DEBUG("[BleProperties] %{public}s:%{public}s", __func__, passkey.c_str());

    return BleConfig::GetInstance().SetPasskey(passkey);
}

bool BleProperties::SetBleModel1Level(int level)
{
    LOG_DEBUG("[BleProperties] %{public}s:%{public}d", __func__, level);

    return BleConfig::GetInstance().SetBleModel1Level(level);
}

bool BleProperties::SetBleModel2Level(int level)
{
    LOG_DEBUG("[BleProperties] %{public}s:%{public}d", __func__, level);

    return BleConfig::GetInstance().SetBleModel2Level(level);
}

bool BleProperties::SetBleSecurity(bool security)
{
    LOG_DEBUG("[BleProperties] %{public}s:%{public}d", __func__, security);

    return BleConfig::GetInstance().SetBleSecurity(security);
}

bool BleProperties::SetBleScanMode(int scanmode)
{
    LOG_DEBUG("[BleProperties] %{public}s:%{public}d", __func__, scanmode);

    return BleConfig::GetInstance().SetBleScanMode(scanmode);
}

bool BleProperties::SaveDefaultValues() const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    bool ret = SetLocalName(BLE_DEFAULT_DEVICE_NAME);
    ret &= SetIoCapability(BLE_DEFAULT_IO);
    ret &= SetPasskey(BLE_DEFAULT_LOCAL_PASSKEY);
    ret &= SetBleRoles(BLE_DEFAULT_ROLES);
    ret &= SetBleModel1Level(BLE_DEFAULT_MODEL1_LEVEL);
    ret &= SetBleModel2Level(BLE_DEFAULT_MODEL2_LEVEL);
    ret &= SetBleSecurity(BLE_DEFAULT_SECURITY);
    ret &= SetBleScanMode(BLE_DEFAULT_SCAN_MODE);
    ret &= BleConfig::GetInstance().Save();
    return ret;
}

void BleProperties::RegisterBleAdapterObserver(BaseObserverList<IAdapterBleObserver> &observer) const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    pimpl->observer_ = &observer;
}

void BleProperties::DeregisterBleAdapterObserver(IAdapterBleObserver &observer) const
{
    LOG_DEBUG("[BleProperties] %{public}s", __func__);

    pimpl->observer_->Deregister(observer);
}
}  // namespace bluetooth
}  // namespace OHOS
