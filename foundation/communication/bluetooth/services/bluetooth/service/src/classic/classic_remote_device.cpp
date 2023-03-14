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

#include "classic_remote_device.h"

#include <cstring>
#include "log.h"
#include "log_util.h"

namespace OHOS {
namespace bluetooth {
ClassicRemoteDevice::ClassicRemoteDevice()
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);
    cod_ = INVALID_VALUE;
    rssi_ = INVALID_VALUE;
    connectionHandle_ = INVALID_VALUE;
    pairState_ = PAIR_NONE;
    ioCapability_ = INVALID_VALUE;
    batteryLevel_ = INVALID_VALUE;
}

ClassicRemoteDevice::ClassicRemoteDevice(const std::string &addr) : macAddr_(addr)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);
    cod_ = INVALID_VALUE;
    rssi_ = INVALID_VALUE;
    connectionHandle_ = INVALID_VALUE;
    pairState_ = PAIR_NONE;
    ioCapability_ = INVALID_VALUE;
    batteryLevel_ = INVALID_VALUE;
}

ClassicRemoteDevice::~ClassicRemoteDevice()
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);
}

std::string ClassicRemoteDevice::GetAddress() const
{
    HILOGI("addr = %{public}s", GetEncryptAddr(macAddr_).c_str());

    return macAddr_;
}

void ClassicRemoteDevice::SetAddress(const std::string &addr)
{
    HILOGI("addr = %{public}s", GetEncryptAddr(addr).c_str());

    macAddr_ = addr;
}

std::string ClassicRemoteDevice::GetRemoteName() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, name = %{public}s", __func__, deviceName_.c_str());

    return deviceName_;
}

void ClassicRemoteDevice::SetRemoteName(const std::string &name)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, name = %{public}s", __func__, name.c_str());

    if (name.length() >= MAX_LOC_BT_NAME_LEN) {
        deviceName_ = name.substr(0, MAX_LOC_BT_NAME_LEN);
    } else {
        deviceName_ = name;
    }
}

std::string ClassicRemoteDevice::GetAliasName() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, name = %{public}s", __func__, aliasName_.c_str());

    if (aliasName_ == INVALID_NAME) {
        return deviceName_;
    }
    return aliasName_;
}

bool ClassicRemoteDevice::SetAliasName(const std::string &name)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, name = %{public}s", __func__, name.c_str());

    if (name.empty()) {
        LOG_ERROR("%{public}s failed, because of alias name is NULL", __func__);
        return false;
    }

    if (name.length() >= MAX_LOC_BT_NAME_LEN) {
        aliasName_ = name.substr(0, MAX_LOC_BT_NAME_LEN);
    } else {
        aliasName_ = name;
    }
    return true;
}

int ClassicRemoteDevice::GetDeviceClass() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, cod = %{public}d", __func__, cod_);

    return cod_;
}

void ClassicRemoteDevice::SetDeviceClass(int deviceClass)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, cod = %{public}d", __func__, deviceClass);

    cod_ = deviceClass;
}

bool ClassicRemoteDevice::IsBondedFromLocal() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, bondedFromLocal_ = %{public}d", __func__, bondedFromLocal_);

    return bondedFromLocal_;
}

void ClassicRemoteDevice::SetBondedFromLocal(bool flag)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, bondedFromLocal_ = %{public}d", __func__, flag);

    bondedFromLocal_ = flag;
}

bool ClassicRemoteDevice::IsAclConnected() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, state = %{public}d", __func__, aclConnected_);

    return (aclConnected_ != CONNECTION_STATE_DISCONNECTED);
}

bool ClassicRemoteDevice::IsAclEncrypted() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, state = %{public}d", __func__, aclConnected_);

    return (aclConnected_ >= CONNECTION_STATE_CONNECTED);
}

void ClassicRemoteDevice::SetAclConnectState(int connectState)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s, state = %{public}d", __func__, connectState);

    aclConnected_ = connectState;
}

std::vector<Uuid> ClassicRemoteDevice::GetDeviceUuids() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);

    return uuids_;
}

void ClassicRemoteDevice::SetDeviceUuids(const std::vector<Uuid> &uuids)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);

    uuids_.assign(uuids.begin(), uuids.end());
}

int ClassicRemoteDevice::GetDeviceType() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s type = %{public}d", __func__, deviceType_);

    return deviceType_;
}

void ClassicRemoteDevice::SetDeviceType(int type)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s type = %{public}d", __func__, type);

    deviceType_ = type;
}

void ClassicRemoteDevice::SetRssi(int rssi)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s rssi = %{public}d", __func__, rssi);

    rssi_ = rssi;
}

void ClassicRemoteDevice::SetConnectionHandle(int handle)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s connectionHandle = %{public}d", __func__, handle);

    connectionHandle_ = handle;
}

int ClassicRemoteDevice::GetConnectionHandle() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s connectionHandle = %{public}d", __func__, connectionHandle_);

    return connectionHandle_;
}

bool ClassicRemoteDevice::SetPairedStatus(int status)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s pairedStatus = %{public}d", __func__, status);

    if (PAIR_NONE > status || PAIR_CANCELING < status) {
        LOG_ERROR("[ClassicRemoteDevice]::%{public}s. Invalid Parameter", __func__);
        return false;
    }

    pairState_ = status;
    return true;
}

int ClassicRemoteDevice::GetPairedStatus() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s pairedStatus = %{public}d", __func__, pairState_);

    return pairState_;
}

std::vector<uint8_t> ClassicRemoteDevice::GetLinkKey() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);

    return linkKey_;
}

void ClassicRemoteDevice::SetLinkKey(const std::vector<uint8_t> &linkKey)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);

    linkKey_ = linkKey;
}

void ClassicRemoteDevice::DeleteLinkKey()
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);

    linkKey_.clear();
}

int ClassicRemoteDevice::GetLinkKeyType() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s linkKeyType = %{public}d", __func__, linkKeyType_);

    return linkKeyType_;
}

void ClassicRemoteDevice::SetLinkKeyType(int linkKeyType)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s linkKeyType = %{public}d", __func__, linkKeyType);

    linkKeyType_ = linkKeyType;
}

int ClassicRemoteDevice::GetIoCapability() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s ioCapability = %{public}d", __func__, ioCapability_);

    return ioCapability_;
}

void ClassicRemoteDevice::SetIoCapability(int io)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s ioCapability = %{public}d", __func__, io);

    ioCapability_ = io;
}

uint8_t ClassicRemoteDevice::GetFlags() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s flags = %{public}d", __func__, flags_);

    return flags_;
}

void ClassicRemoteDevice::SetFlags(uint8_t flags)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s flags = %{public}d", __func__, flags);

    flags_ = flags;
}

void ClassicRemoteDevice::SetManufacturerSpecificData(const std::vector<uint8_t> &data)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s", __func__);

    manuSpecData_ = data;
}

bool ClassicRemoteDevice::IsPaired() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s pairState = %{public}d", __func__, pairState_);

    return (pairState_ == PAIR_PAIRED);
}

void ClassicRemoteDevice::SetTxPower(uint8_t power)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s txPower = %u", __func__, power);

    txPower_ = power;
}

void ClassicRemoteDevice::SetURI(const std::string &uri)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s uri = %{public}s", __func__, uri.c_str());

    uri_ = uri;
}

int ClassicRemoteDevice::GetPairConfirmState() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s state = %{public}d", __func__, pairConfirmState_);

    return pairConfirmState_;
}

void ClassicRemoteDevice::SetPairConfirmState(int state)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s state = %{public}d", __func__, state);

    pairConfirmState_ = state;
}

void ClassicRemoteDevice::SetPairConfirmType(int type)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s Type = %{public}d", __func__, type);

    pairConfirmType_ = type;
}

int ClassicRemoteDevice::GetPairConfirmType() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s Type = %{public}d", __func__, pairConfirmType_);

    return pairConfirmType_;
}

void ClassicRemoteDevice::SetNameNeedGet(bool nameUnknowned)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s nameNeedGet_ = %{public}d", __func__, nameUnknowned);
    nameNeedGet_ = nameUnknowned;
}

bool ClassicRemoteDevice::GetNameNeedGet() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s nameNeedGet_ = %{public}d", __func__, nameNeedGet_);

    return nameNeedGet_;
}

int ClassicRemoteDevice::GetBatteryLevel() const
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s batteryLevel = %{public}d", __func__, batteryLevel_);

    return batteryLevel_;
}

void ClassicRemoteDevice::SetBatteryLevel(int batteryLevel)
{
    LOG_DEBUG("[ClassicRemoteDevice]::%{public}s batteryLevel = %{public}d", __func__, batteryLevel);

    batteryLevel_ = batteryLevel;
}

bool ClassicRemoteDevice::CheckCod(uint32_t cod) const
{
    uint32_t tmpCod = cod_;
    return (tmpCod & CLASS_OF_DEVICE_MASK) == cod;
}
}  // namespace bluetooth
}  // namespace OHOS