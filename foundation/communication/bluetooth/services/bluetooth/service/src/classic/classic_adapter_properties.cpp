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

#include "classic_adapter_properties.h"

#include <string>

#include "classic_data_structure.h"
#include "classic_data_type_defs.h"
#include "classic_defs.h"
#include "classic_utils.h"
#include "gap_if.h"
#include "log_util.h"
#include "raw_address.h"
#include "securec.h"

namespace OHOS {
namespace bluetooth {
ClassicAdapterProperties &ClassicAdapterProperties::GetInstance()
{
    static ClassicAdapterProperties instance;
    return instance;
}

ClassicAdapterProperties::ClassicAdapterProperties() : config_(ClassicConfig::GetInstance())
{}

ClassicAdapterProperties::~ClassicAdapterProperties()
{}

bool ClassicAdapterProperties::LoadConfigInfo()
{
    if (!config_.LoadConfigFile()) {
        LOG_ERROR("Load config file failed!");
        return false;
    } else {
        LoadHostInfo();
    }

    return true;
}

void ClassicAdapterProperties::LoadHostInfo()
{
    deviceName_ = config_.GetLocalName();
    if (deviceName_.empty()) {
        deviceName_ = DEFAULT_DEVICE_NAME;
    }

    cod_ = (config_.GetLocalDeviceClass() & CLASS_OF_DEVICE_RANGE);
    if (INVALID_VALUE > cod_) {
        cod_ = DEFAULT_CLASS_OF_DEVICE;
    }

    ioCapability_ = config_.GetIoCapability();
    if ((GAP_IO_DISPLAYONLY > ioCapability_) || (GAP_IO_NOINPUTNOOUTPUT < ioCapability_)) {
        ioCapability_ = GAP_IO_DISPLAYYESNO;
    }

    passkey_ = config_.GetLocalPasskey();
    if (passkey_.empty()) {
        passkey_ = DEFAULT_PASSKEY;
    }

    securityMode_ = config_.GetSecurityMode();
    if ((securityMode_ != SEC_MODE_2) && (securityMode_ != SEC_MODE_4)) {
        securityMode_ = SEC_MODE_2;
    }

    LOG_DEBUG("Get Host info:");
    LOG_DEBUG("Device name is: %{public}s", deviceName_.c_str());
    LOG_DEBUG("Class of device is: %{public}d", cod_);
    LOG_DEBUG("IoCapability is: %{public}d", ioCapability_);
    LOG_DEBUG("Passkey is: %{public}s", passkey_.c_str());
    LOG_DEBUG("securityMode is: %{public}d", securityMode_);
}

std::string ClassicAdapterProperties::GetLocalName() const
{
    return deviceName_;
}

bool ClassicAdapterProperties::SetLocalName(const std::string &name)
{
    std::lock_guard<std::recursive_mutex> lock(propertiesMutex_);

    int length = name.length();
    if (length >= MAX_LOC_BT_NAME_LEN) {
        length = MAX_LOC_BT_NAME_LEN;
        deviceName_ = name.substr(0, length);
    } else {
        deviceName_ = name;
    }

    // Update controller
    bool ret = (GAPIF_SetLocalName(deviceName_.c_str(), length) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "GAPIF_SetLocalName", ret);

    // Set EIR
    ret &= SetEirData();
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetEirData", ret);

    // Update config
    ret &= UpdateConfig(BT_PROPERTY_BDNAME);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "UpdateConfig", ret);

    return ret;
}

std::string ClassicAdapterProperties::GetLocalAddress() const
{
    return macAddr_;
}

int ClassicAdapterProperties::GetLocalDeviceClass() const
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s cod: %{public}d", __func__, cod_);
    return cod_;
}

bool ClassicAdapterProperties::SetLocalDeviceClass(int deviceClass)
{
    bool ret = false;
    cod_ = deviceClass;

    int result = GAPIF_SetClassOfDevice(cod_);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("ClassicAdapterProperties::%{public}s GAPIF_SetClassOfDevice failed!", __func__);
        return ret;
    }

    ret = UpdateConfig(BT_PROPERTY_CLASS_OF_DEVICE);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "UpdateConfig", ret);

    return true;
}

int ClassicAdapterProperties::GetBondableMode() const
{
    return bondableMode_;
}

bool ClassicAdapterProperties::SetBondableMode(int mode)
{
    if (mode < BONDABLE_MODE_OFF || mode > BONDABLE_MODE_ON) {
        LOG_ERROR("ClassicAdapterProperties::%{public}s. Invalid Parameter", __func__);
        return false;
    }

    switch (mode) {
        case BONDABLE_MODE_OFF:
            bondableMode_ = GAP_BONDABLE_MODE_NON;
            break;
        case BONDABLE_MODE_ON:
            bondableMode_ = GAP_BONDABLE_MODE;
            break;
        default:
            bondableMode_ = GAP_BONDABLE_MODE_NON;
            break;
    }
    int ret = GAPIF_SetBondableMode(bondableMode_);
    if (ret != BT_NO_ERROR) {
        return false;
    }

    return true;
}

std::string ClassicAdapterProperties::GetPasskey() const
{
    return passkey_;
}

int ClassicAdapterProperties::GetIoCapability() const
{
    return ioCapability_;
}

bool ClassicAdapterProperties::SetIoCapability(int ioCapability)
{
    ioCapability_ = ioCapability;
    return true;
}

bool ClassicAdapterProperties::ConfigProperties()
{
    /// Read MAC Addr from Controller.
    bool ret = ReadAddrFromController();
    if (!ret) {
        return ret;
    }

    /// Update MAC Addr to config file.
    ret &= UpdateConfig(BT_PROPERTY_BDADDR);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "UpdateConfig", ret);

    ret &= SetLocalName(deviceName_);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetLocalName", ret);

    // Set Class of Device.
    ret &= SetLocalDeviceClass(cod_);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetLocalDeviceClass", ret);

    return ret;
}

bool ClassicAdapterProperties::InitMode()
{
    /// Set BondMode.
    bool ret = true;
    if (bondableMode_ != BONDABLE_MODE_OFF) {
        ret = SetBondableMode(BONDABLE_MODE_OFF);
        ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetBondableMode", ret);
    }

    return ret;
}

bool ClassicAdapterProperties::SetSecurityMode()
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);

    bool ret = (GAPIF_SetSecurityMode((GAP_SecurityMode)securityMode_) == BT_NO_ERROR);
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "GAPIF_SetSecurityMode", ret);

    return ret;
}

int ClassicAdapterProperties::GetDiscoverableTimeout() const
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);

    return discoverableTimeout_;
}

bool ClassicAdapterProperties::SetDiscoverableTimeout(int time)
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s time = %{public}d", __func__, time);

    bool ret = true;
    if (discoverableTimeout_ != time) {
        discoverableTimeout_ = time;
    } else {
        LOG_WARN("ClassicAdapterProperties::SetDiscoverableTimeout same value");
    }

    return ret;
}

bool ClassicAdapterProperties::ReadAddrFromController()
{
    BtAddr btAddr;
    errno_t result = memset_s(&btAddr, sizeof(BtAddr), 0, sizeof(BtAddr));
    if (result != EOK) {
        LOG_ERROR("%{public}s::memset_s failed!", __func__);
        return false;
    }

    bool ret = (GAPIF_GetLocalAddr(&btAddr) == BT_NO_ERROR);
    if (!ret) {
        return ret;
    }
    macAddr_ = RawAddress::ConvertToString(btAddr.addr).GetAddress();
    HILOGI("GAPIF_GetLocalAddr: %{public}s", GetEncryptAddr(macAddr_).c_str());

    return ret;
}

bool ClassicAdapterProperties::UpdateConfig(int type)
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s Type: %{public}d", __func__, type);

    bool ret = false;
    switch (type) {
        case BT_PROPERTY_BDNAME: {
            ret = config_.SetLocalName(deviceName_);
            if (ret == false) {
                LOG_ERROR("UpdateConfig::SetLocalName failed");
            } else {
                SendDeviceNameChanged(deviceName_);
            }
            break;
        }
        case BT_PROPERTY_BDADDR: {
            ret = config_.SetLocalAddress(macAddr_);
            if (ret == false) {
                LOG_ERROR("UpdateConfig::SetLocalAddress failed");
            } else {
                SendDeviceAddrChanged(macAddr_);
            }
            break;
        }
        case BT_PROPERTY_CLASS_OF_DEVICE:
            ret = config_.SetLocalDeviceClass(cod_);
            ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetLocalDeviceClass", ret);
            break;
        case BT_PROPERTY_ADAPTER_DISCOVERABLE_TIMEOUT:
            ret = config_.SetDiscoverableTimeout(discoverableTimeout_);
            ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetDiscoverableTimeout", ret);
            break;
        default:
            break;
    }

    ret = config_.Save();
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "Save", ret);

    return ret;
}

void ClassicAdapterProperties::SendDeviceNameChanged(const std::string &deviceName)
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);

    adapterObservers_.ForEach(
        [deviceName](IAdapterClassicObserver &observer) { observer.OnDeviceNameChanged(deviceName); });
}

void ClassicAdapterProperties::SendDeviceAddrChanged(const std::string &address)
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);

    adapterObservers_.ForEach([address](IAdapterClassicObserver &observer) { observer.OnDeviceAddrChanged(address); });
}

bool ClassicAdapterProperties::SetEirData()
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);

    std::lock_guard<std::recursive_mutex> lock(propertiesMutex_);
    bool ret = true;
    /// New bluetooth object to make up the eir data.
    std::unique_ptr<ClassicBluetoothData> eirData = std::make_unique<ClassicBluetoothData>();

    /// Set data length for eir.
    eirData->SetDataMaxLength(MAX_EXTEND_INQUIRY_RESPONSE_LEN);

    /// Set eir name length and eir type.
    uint8_t nameLen = deviceName_.length() + EXTEND_INQUIRY_RESPONSE_TYPE_SIZE;
    int nameType = BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME;

    uint8_t uuidLen = (UUID16_BYTES_TYPE * uuids_.size()) + EXTEND_INQUIRY_RESPONSE_TYPE_SIZE;
    if (EXTEND_INQUIRY_RESPONSE_TYPE_SIZE < uuidLen) {
        int dataLen = nameLen + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE + uuidLen + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE;
        if (dataLen >= MAX_EXTEND_INQUIRY_RESPONSE_LEN) {
            nameType = BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME;
            nameLen = MAX_EXTEND_INQUIRY_RESPONSE_LEN - (uuidLen + EXTEND_INQUIRY_RESPONSE_LENGTH_SIZE) -
                      EXTEND_INQUIRY_RESPONSE_TYPE_SIZE;
        }
    } else {
        if (nameLen >= MAX_EXTEND_INQUIRY_RESPONSE_LEN) {
            nameType = BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME;
            nameLen = MAX_EXTEND_INQUIRY_RESPONSE_LEN - EXTEND_INQUIRY_RESPONSE_TYPE_SIZE;
        }
    }
    std::string subString = deviceName_.substr(0, (nameLen - 1));
    std::vector<uint8_t> eirName;
    eirName.assign(subString.begin(), subString.end());

    /// Construct the eir data
    ClassicDataStructure nameData(nameLen, nameType, eirName);
    eirData->AddDataStructure(nameData);

    if (!uuids_.empty()) {
        int uuidType = BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS;
        std::vector<uint8_t> value(uuidLen - EXTEND_INQUIRY_RESPONSE_TYPE_SIZE);
        int idx = 0;
        for (auto it = uuids_.begin(); it != uuids_.end(); it++) {
            uint16_t uuid = (*it).ConvertTo16Bits();
            value[idx] = (uint8_t)(uuid & 0x00FF);
            value[idx + 1] = (uint8_t)((uuid & 0xFF00) >> MOVE_ONE_BYTE);
            idx += sizeof(uint16_t);
        }
        ClassicDataStructure uuidData(uuidLen, uuidType, value);
        eirData->AddDataStructure(uuidData);
        value.clear();
    }

    /// Set eir data to GAP
    int result = GAPIF_SetExtendedInquiryResponse(eirData->GetClassicBluetoothData().data());
    if (result != BT_NO_ERROR) {
        LOG_ERROR("ClassicAdapterProperties::GAPIF_SetExtendedInquiryResponse failed, ret = %{public}d", result);
        ret = false;
    }
    return ret;
}

void ClassicAdapterProperties::RegisterClassicAdapterObserver(IAdapterClassicObserver &observer)
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);
    adapterObservers_.Register(observer);
}

void ClassicAdapterProperties::DeregisterClassicAdapterObserver(IAdapterClassicObserver &observer)
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);
    adapterObservers_.Deregister(observer);
}

std::vector<std::string> ClassicAdapterProperties::GetPairedAddrList() const
{
    return config_.GetPairedAddrList();
}

void ClassicAdapterProperties::SaveConfigFile() const
{
    LOG_DEBUG("[ClassicAdapterProperties]::%{public}s", __func__);
    bool ret = config_.Save();
    LOG_DEBUG("ClassicAdapterProperties::%{public}s result = %{public}d", __func__, ret);
}

std::shared_ptr<ClassicRemoteDevice> ClassicAdapterProperties::GetPairedDevice(std::string addr)
{
    HILOGI("addr: %{public}s", GetEncryptAddr(addr).c_str());

    std::shared_ptr<ClassicRemoteDevice> remote = std::make_shared<ClassicRemoteDevice>(addr);

    std::string name = config_.GetRemoteName(addr);
    remote->SetRemoteName(name);

    std::string alias = config_.GetRemoteAlias(addr);
    remote->SetAliasName(alias);

    int linkKeyType = config_.GetRemoteLinkkeyType(addr);
    remote->SetLinkKeyType(linkKeyType);

    if (linkKeyType != PAIR_INVALID_LINK_KEY_TYPE) {
        std::string key = config_.GetRemoteLinkkey(addr);
        LOG_DEBUG("Get linkKey value is %{public}s", key.c_str());
        std::vector<uint8_t> linkKey;
        ClassicUtils::ConvertHexStringToInt(key, linkKey);
        remote->SetLinkKey(linkKey);
    }

    int io = config_.GetRemoteDeviceIoCapability(addr);
    remote->SetIoCapability(io);

    int cod = config_.GetRemoteDeviceClass(addr);
    remote->SetDeviceClass(cod);

    int deviceType = config_.GetRemoteDeviceType(addr);
    remote->SetDeviceType(deviceType);

    bool pairFlag = config_.GetRemoteDevicePairFlag(addr);
    if (pairFlag == true) {
        remote->SetPairedStatus(PAIR_PAIRED);
    }

    bool bondFromLocal = config_.GetRemoteDeviceBondFromLocal(addr);
    remote->SetBondedFromLocal(bondFromLocal);

    std::string uuidVal = config_.GetRemoteUuids(addr);
    std::vector<Uuid> uuids = ClassicUtils::ConvertStringToUuid(uuidVal);
    if (!uuids.empty()) {
        remote->SetDeviceUuids(uuids);
    }

    return remote;
}

void ClassicAdapterProperties::SavePairedDeviceInfo(std::shared_ptr<ClassicRemoteDevice> remote)
{
    HILOGI("addr: %{public}s", GetEncryptAddr(remote->GetAddress()).c_str());
    std::string addr = remote->GetAddress();

    std::string name = remote->GetRemoteName();
    if (!name.empty()) {
        config_.SetRemoteName(addr, name);
    }

    std::string alias = remote->GetAliasName();
    if (!alias.empty()) {
        config_.SetRemoteAlias(addr, alias);
    }

    int linkKeyType = remote->GetLinkKeyType();
    config_.SetRemoteLinkkeyType(addr, linkKeyType);

    if (linkKeyType != PAIR_INVALID_LINK_KEY_TYPE) {
        std::vector<uint8_t> linkKey = remote->GetLinkKey();
        std::string key = ClassicUtils::ConvertIntToHexString(linkKey);
        LOG_DEBUG("Save LinkKey value is %{public}s", key.c_str());
        config_.SetRemoteLinkkey(addr, key);
    }

    int io = remote->GetIoCapability();
    config_.SetRemoteDeviceIoCapability(addr, io);

    int cod = remote->GetDeviceClass();
    config_.SetRemoteDeviceClass(addr, cod);

    int deviceType = remote->GetDeviceType();
    config_.SetRemoteDeviceType(addr, deviceType);

    bool pairFlag = remote->IsPaired();
    config_.SetRemoteDevicePairFlag(addr, pairFlag);

    bool bondFromLocal = remote->IsBondedFromLocal();
    config_.SetRemoteDeviceBondFromLocal(addr, bondFromLocal);

    std::lock_guard<std::recursive_mutex> lock(propertiesMutex_);
    std::vector<Uuid> uuids = remote->GetDeviceUuids();
    if (uuids.empty()) {
        return;
    }
    std::string uuidVal = ClassicUtils::ConvertUuidToString(uuids);
    if (!uuidVal.empty()) {
        config_.SetRemoteUuids(addr, uuidVal);
    }
}

void ClassicAdapterProperties::RemovePairedDeviceInfo(std::string addr) const
{
    HILOGI("addr: %{public}s", GetEncryptAddr(addr).c_str());

    bool ret = config_.RemovePairedDevice(addr);
    if (ret == false) {
        HILOGI("failed, addr is %{public}s", GetEncryptAddr(addr).c_str());
    }
}

bool ClassicAdapterProperties::SaveSupportUuids(const std::vector<Uuid> &uuids)
{
    std::lock_guard<std::recursive_mutex> lock(propertiesMutex_);

    if (uuids.empty()) {
        LOG_DEBUG("ClassicAdapterProperties::%{public}s input parameter(uuids) is null.", __func__);
        return false;
    }

    if (!uuids_.empty()) {
        uuids_.clear();
    }
    uuids_ = uuids;

    bool ret = SetEirData();
    ClassicUtils::CheckReturnValue("ClassicAdapaterProperties", "SetEirData", ret);

    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS