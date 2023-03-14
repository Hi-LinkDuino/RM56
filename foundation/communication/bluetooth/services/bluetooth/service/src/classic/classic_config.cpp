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

#include "classic_config.h"

#include <vector>

#include "bt_def.h"
#include "classic_defs.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
ClassicConfig &ClassicConfig::GetInstance()
{
    static ClassicConfig instance;
    return instance;
}

ClassicConfig::ClassicConfig() : config_(AdapterDeviceConfig::GetInstance())
{}

ClassicConfig::~ClassicConfig()
{}

bool ClassicConfig::LoadConfigFile() const
{
    /// Load Device Config File.
    bool ret = config_->Load();
    if (!ret) {
        LOG_ERROR("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return ret;
}

bool ClassicConfig::Save() const
{
    bool ret = config_->Save();
    if (!ret) {
        LOG_ERROR("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return ret;
}

std::string ClassicConfig::GetLocalName() const
{
    std::string name = "";
    if (!config_->GetValue(SECTION_HOST, PROPERTY_DEVICE_NAME, name)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return name;
}

bool ClassicConfig::SetLocalName(const std::string &name) const
{
    if (!config_->SetValue(SECTION_HOST, PROPERTY_DEVICE_NAME, name)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}
bool ClassicConfig::SetLocalAddress(const std::string &addr) const
{
    if (!config_->SetValue(SECTION_HOST, PROPERTY_DEVICE_ADDR, addr)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

int ClassicConfig::GetLocalDeviceClass() const
{
    int cod = 0;
    if (!config_->GetValue(SECTION_HOST, PROPERTY_CLASS_OF_DEVICE, cod)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return cod;
}

bool ClassicConfig::SetLocalDeviceClass(int cod) const
{
    if (!config_->SetValue(SECTION_HOST, PROPERTY_CLASS_OF_DEVICE, cod)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

int ClassicConfig::GetIoCapability() const
{
    int io = 0;
    if (!config_->GetValue(SECTION_HOST, PROPERTY_IO_CAPABILITY, io)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return io;
}

int ClassicConfig::GetDiscoverableTimeout() const
{
    int time = 0;
    if (!config_->GetValue(SECTION_HOST, PROPERTY_DISCOVERABLE_TIMEOUT, time)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return time;
}

bool ClassicConfig::SetDiscoverableTimeout(int time) const
{
    if (!config_->SetValue(SECTION_HOST, PROPERTY_DISCOVERABLE_TIMEOUT, time)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

std::string ClassicConfig::GetLocalPasskey() const
{
    std::string passkey = "";
    if (!config_->GetValue(SECTION_HOST, PROPERTY_LOCAL_PASSKEY, passkey)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return passkey;
}

int ClassicConfig::GetSecurityMode() const
{
    int securityMode = 0;
    if (!config_->GetValue(SECTION_HOST, PROPERTY_SECURITY_MODE, securityMode)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return securityMode;
}

std::vector<std::string> ClassicConfig::GetPairedAddrList() const
{
    std::vector<std::string> pairedList;
    if (!config_->GetSubSections(SECTION_BREDR_PAIRED_LIST, pairedList)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return pairedList;
}

std::string ClassicConfig::GetRemoteName(const std::string &subSection) const
{
    std::string name = "";
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_DEVICE_NAME, name)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return name;
}

std::string ClassicConfig::GetRemoteAlias(const std::string &subSection) const
{
    std::string name = "";
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_ALIAS_NAME, name)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return name;
}

std::string ClassicConfig::GetRemoteLinkkey(const std::string &subSection) const
{
    std::string key = "";
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_LINK_KEY, key)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return key;
}

int ClassicConfig::GetRemoteDeviceType(const std::string &subSection) const
{
    int type = 0;
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_DEVICE_TYPE, type)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return type;
}

int ClassicConfig::GetRemoteLinkkeyType(const std::string &subSection) const
{
    int type = 0;
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_LINK_KEY_TYPE, type)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return type;
}

int ClassicConfig::GetRemoteDeviceClass(const std::string &subSection) const
{
    int cod = 0;
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_CLASS_OF_DEVICE, cod)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return cod;
}

int ClassicConfig::GetRemoteDeviceIoCapability(const std::string &subSection) const
{
    int io = 0;
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_IO_CAPABILITY, io)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return io;
}

bool ClassicConfig::SetRemoteName(const std::string &subSection, const std::string &name) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_DEVICE_NAME, name)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteAlias(const std::string &subSection, const std::string &name) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_ALIAS_NAME, name)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteDeviceType(const std::string &subSection, int type) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_DEVICE_TYPE, type)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteLinkkey(const std::string &subSection, const std::string &linkKey) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_LINK_KEY, linkKey)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteLinkkeyType(const std::string &subSection, int type) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_LINK_KEY_TYPE, type)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteDeviceClass(const std::string &subSection, int cod) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_CLASS_OF_DEVICE, cod)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteDeviceIoCapability(const std::string &subSection, int io) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_IO_CAPABILITY, io)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::GetRemoteDevicePairFlag(const std::string &subSection) const
{
    bool flag = false;
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_PAIR_FLAG, flag)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return flag;
}

bool ClassicConfig::GetRemoteDeviceBondFromLocal(const std::string &subSection) const
{
    bool flag = false;
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_BOND_FROM_LOCAL, flag)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return flag;
}

bool ClassicConfig::SetRemoteDevicePairFlag(const std::string &subSection, const bool flag) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_PAIR_FLAG, flag)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteDeviceBondFromLocal(const std::string &subSection, const bool flag) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_BOND_FROM_LOCAL, flag)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::RemovePairedDevice(const std::string &subSection) const
{
    if (!config_->RemoveSection(SECTION_BREDR_PAIRED_LIST, subSection)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

bool ClassicConfig::SetRemoteUuids(const std::string &subSection, const std::string &uuids) const
{
    if (!config_->SetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_REMOTE_UUIDS, uuids)) {
        LOG_WARN("[ClassicConfig]::%{public}s failed!", __func__);
        return false;
    }

    return true;
}

std::string ClassicConfig::GetRemoteUuids(const std::string &subSection) const
{
    std::string uuids = "";
    if (!config_->GetValue(SECTION_BREDR_PAIRED_LIST, subSection, PROPERTY_REMOTE_UUIDS, uuids)) {
        LOG_INFO("[ClassicConfig]::%{public}s failed!", __func__);
    }

    return uuids;
}
}  // namespace bluetooth
}  // namespace OHOS