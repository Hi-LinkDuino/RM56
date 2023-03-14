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

#include "ble_config.h"

#include "ble_defs.h"
#include "log.h"
#include "xml_parse.h"

namespace OHOS {
namespace bluetooth {
BleConfig &BleConfig::GetInstance()
{
    static BleConfig instance;
    return instance;
}

BleConfig::BleConfig()
{
    config_ = AdapterDeviceConfig::GetInstance();
}

BleConfig::~BleConfig()
{}

bool BleConfig::LoadConfigInfo() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->Load();
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Load device config file failed!");
    }
    return ret;
}

bool BleConfig::Save() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    return config_->Save();
}

std::string BleConfig::GetLocalName() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string name = "";
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_DEVICE_NAME, name);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get local name failed!");
    }
    return name;
}

bool BleConfig::SetLocalName(const std::string &name) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_DEVICE_NAME, name);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set local name failed!");
    }
    return ret;
}

std::string BleConfig::GetLocalAddress() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string addr = BLE_INVALID_MAC_ADDRESS;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_DEVICE_ADDR, addr);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get local address failed!");
    }
    return addr;
}

bool BleConfig::SetLocalAddress(const std::string &addr) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_DEVICE_ADDR, addr);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set local address failed!");
    }
    return ret;
}

int BleConfig::GetIoCapability() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int io = 0;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_IO_CAPABILITY, io);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get local IO capability failed!");
    }
    return io;
}

std::string BleConfig::GetLoaclPasskey() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string passkey = "";
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_LOCAL_PASSKEY, passkey);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get local passkey failed!");
    }
    return passkey;
}

int BleConfig::GetBleRoles() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int roles = (GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_OBSERVER | GAP_LE_ROLE_PREIPHERAL | GAP_LE_ROLE_CENTRAL);
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_ROLES, roles);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get ble roles failed!");
    }
    return roles;
}

bool BleConfig::SetBleRoles(uint8_t roles) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_ROLES, roles);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble roles failed!");
    }
    return ret;
}

bool BleConfig::SetPasskey(const std::string &passkey) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_LOCAL_PASSKEY, passkey);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set local pass key failed!");
    }
    return ret;
}

int BleConfig::GetBleModel1Level() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int level = LE_MODE_1_LEVEL_1;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_MODE_1_LEVEL, level);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get ble model1 level failed!");
    }
    return level;
}

int BleConfig::GetBleModel2Level() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int level = LE_MODE_2_LEVEL_1;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_MODE_2_LEVEL, level);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get ble model2 level failed!");
    }
    return level;
}

bool BleConfig::GetBleSecurity() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool sc = false;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_SECURITY, sc);
    if (!ret) {
        sc = true;
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get ble security failed!");
    }
    return sc;
}

int BleConfig::GetBleScanMode() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int scanMode = BLE_SCAN_MODE_ALL;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_SCAN_MODE, scanMode);
    if (!ret) {
        scanMode = BLE_SCAN_MODE_ALL;
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble non disc mode is valid failed!");
    }
    return scanMode;
}

int BleConfig::GetBleLocalAddrType() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int localAddrType = BLE_ADDR_TYPE_PUBLIC;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_LOCAL_ADDR_TYPE, localAddrType);
    if (!ret) {
        localAddrType = BLE_ADDR_TYPE_PUBLIC;
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble non disc mode is valid failed!");
    }
    return localAddrType;
}

int BleConfig::GetBleAddrType() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int addrType = BLE_ADDR_TYPE_PUBLIC;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_ADDR_TYPE, addrType);
    if (!ret) {
        addrType = BLE_ADDR_TYPE_PUBLIC;
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble non disc mode is valid failed!");
    }
    return addrType;
}

bool BleConfig::SetBleModel1Level(int level) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_MODE_1_LEVEL, level);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble model1 level failed!");
    }
    return ret;
}

bool BleConfig::SetBleModel2Level(int level) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_MODE_2_LEVEL, level);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble model1 level failed!");
    }
    return ret;
}

bool BleConfig::SetBleSecurity(bool security) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_SECURITY, security);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble security failed!");
    }
    return ret;
}

bool BleConfig::SetBleScanMode(int scanmode) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_SCAN_MODE, scanmode);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble scan mode failed!");
    }
    return ret;
}

bool BleConfig::SetBleLocalAddrType(int localAddrType) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_LOCAL_ADDR_TYPE, localAddrType);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble scan mode failed!");
    }
    return ret;
}

int BleConfig::GetAppearance() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int appearance = 0;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_APPEARANCE, appearance);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get ble appearance failed!");
    }
    return appearance;
}

bool BleConfig::SetLocalIdentityAddr(const std::string &addr) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_LOCAL_IDENTITY_ADDR, addr);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local identity addr failed!");
    }
    return ret;
}

bool BleConfig::SetLocalIrk(const std::string &irk) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_HOST, PROPERTY_BLE_LOCAL_IRK, irk);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local csrk failed!");
    }
    return ret;
}

bool BleConfig::SetLocalLtk(const std::string &section, const std::string &ltk) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_LTK, ltk);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local ltk failed!");
    }
    return ret;
}

bool BleConfig::SetLocalKeySize(const std::string &section, const std::string &keysize) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_KEY_SIZE, keysize);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local keysize failed!");
    }
    return ret;
}

bool BleConfig::SetLocalEdivRand(const std::string &section, const std::string &ediv, const std::string &rand) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_EDIV, ediv);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local ediv failed!");
    }

    ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_RAND, rand);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local rand failed!");
    }
    return ret;
}

bool BleConfig::SetLocalCsrk(const std::string &section, const std::string &csrk) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_CSRK, csrk);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local csrk failed!");
    }
    return ret;
}

bool BleConfig::SetLocalSignCounter(const std::string &section, uint32_t signCounter) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(
        SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_SIGN_COUNTER, std::to_string(signCounter));
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble local signCounter failed!");
    }
    return ret;
}

bool BleConfig::SetPeerKeyType(const std::string &section, const std::string &keytype) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_KEY_TYPE, keytype);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer keytype failed!");
    }
    return ret;
}

bool BleConfig::SetPeerLtk(const std::string &section, const std::string &ltk) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_LTK, ltk);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer ltk failed!");
    }
    return ret;
}

bool BleConfig::SetPeerKeySize(const std::string &section, const std::string &keysize) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_KEY_SIZE, keysize);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer keysize failed!");
    }
    return ret;
}

bool BleConfig::SetPeerEdivRand(const std::string &section, const std::string &ediv, const std::string &rand) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_EDIV, ediv);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer ediv failed!");
    }

    ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_RAND, rand);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer rand failed!");
    }
    return ret;
}

bool BleConfig::SetPeerIdentityAddr(const std::string &section, uint8_t type, const std::string &peerAddress) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_IDENTITY_ADDR_TYPE, type);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer identity addr type failed!");
    }

    ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_IDENTITY_ADDR, peerAddress);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer identity addr failed!");
    }
    return ret;
}

bool BleConfig::SetPeerIrk(const std::string &section, const std::string &irk) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_IRK, irk);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer irk failed!");
    }
    return ret;
}

bool BleConfig::SetPeerCsrk(const std::string &section, const std::string &csrk) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_CSRK, csrk);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer csrk failed!");
    }
    return ret;
}

bool BleConfig::SetPeerSignCounter(const std::string &section, uint32_t signCounter) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(
        SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_SIGN_COUNTER, std::to_string(signCounter));
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set ble peer signCounter failed!");
    }
    return ret;
}

std::string BleConfig::GetLocalIrk() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string irk;
    bool ret = config_->GetValue(SECTION_HOST, PROPERTY_BLE_LOCAL_IRK, irk);
    if (ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble local irk failed!");
    }
    return irk;
}

std::string BleConfig::GetLocalLtk(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string ltk;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_LTK, ltk);
    if (ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble local ltk failed!");
    }
    return ltk;
}

std::string BleConfig::GetLocalEdiv(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string ediv;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_EDIV, ediv);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble local ediv failed!");
    }
    return ediv;
}

std::string BleConfig::GetLocalRand(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string rand;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_RAND, rand);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble local rand failed!");
    }
    return rand;
}

std::string BleConfig::GetLocalCsrk(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string csrk;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_CSRK, csrk);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble local csrk failed!");
    }
    return csrk;
}

uint32_t BleConfig::GetLocalSignCounter(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string signCounter = "0";
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_LOCAL_SIGN_COUNTER, signCounter);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble local signCounter failed!");
    }
    return std::stoul(signCounter);
}

std::string BleConfig::GetPeerLtk(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string ltk;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_LTK, ltk);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear ltk failed!");
    }
    return ltk;
}

std::string BleConfig::GetPeerEdiv(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string ediv;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_EDIV, ediv);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear ediv failed!");
    }
    return ediv;
}

std::string BleConfig::GetPeerRand(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string rand;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_RAND, rand);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear rand failed!");
    }
    return rand;
}

std::string BleConfig::GetPeerIdentityAddr(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string peerAddress;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_IDENTITY_ADDR, peerAddress);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear identity addr failed!");
    }
    return peerAddress;
}

std::string BleConfig::GetPeerIrk(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string irk;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_IRK, irk);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear irk failed!");
    }
    return irk;
}

std::string BleConfig::GetPeerCsrk(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string csrk;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_CSRK, csrk);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear csrk failed!");
    }
    return csrk;
}

uint32_t BleConfig::GetPeerSignCounter(const std::string &section) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string signCounter = "0";
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, section, PROPERTY_BLE_PEER_SIGN_COUNTER, signCounter);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get ble pear signCounter failed!");
    }
    return std::stoul(signCounter);
}

std::string BleConfig::GetPeerName(const std::string &subSection) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string name = "";
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_DEVICE_NAME, name);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get peer name failed!");
    }
    return name;
}

std::string BleConfig::GetPeerAlias(const std::string &subSection) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::string name = "";
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_ALIAS_NAME, name);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get peer alias name failed!");
    }
    return name;
}

int BleConfig::GetPeerDeviceType(const std::string &subSection) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int type = 0;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_DEVICE_TYPE, type);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get peer device type failed!");
    }
    return type;
}

int BleConfig::GetPeerDeviceIoCapability(const std::string &subSection) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int io = 0;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_IO_CAPABILITY, io);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get peer io capability failed!");
    }
    return io;
}

bool BleConfig::SetPeerName(const std::string &subSection, const std::string &name) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_DEVICE_NAME, name);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set peer device name failed!");
    }
    return ret;
}

bool BleConfig::SetPeerDeviceType(const std::string &subSection, const int type) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_DEVICE_TYPE, type);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set peer device type failed!");
    }
    return ret;
}

bool BleConfig::SetPeerAddressType(const std::string &subSection, const int type) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_BLE_PEER_ADDR_TYPE, type);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set peer address type failed!");
    }
    return ret;
}

int BleConfig::GetPeerAddressType(const std::string &subSection) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    int type = 0;
    bool ret = config_->GetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_BLE_PEER_ADDR_TYPE, type);
    if (!ret) {
        LOG_DEBUG("[BleConfig] %{public}s:%{public}s", __func__, "Get peer address type failed!");
    }
    return type;
}

bool BleConfig::RemovePairedDevice(const std::string &subSection) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->RemoveSection(SECTION_BLE_PAIRED_LIST, subSection);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Remove paired device info failed!");
    }
    return ret;
}

std::vector<std::string> BleConfig::GetPairedAddrList() const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    std::vector<std::string> pairedList;
    bool ret = config_->GetSubSections(SECTION_BLE_PAIRED_LIST, pairedList);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Get Paired Device List failed!");
    }
    return pairedList;
}

bool BleConfig::SetPeerDeviceIoCapability(const std::string &subSection, int io) const
{
    LOG_DEBUG("[BleConfig] %{public}s", __func__);

    bool ret = config_->SetValue(SECTION_BLE_PAIRED_LIST, subSection, PROPERTY_IO_CAPABILITY, io);
    if (!ret) {
        LOG_ERROR("[BleConfig] %{public}s:%{public}s", __func__, "Set peer device name failed!");
    }
    return ret;
}
}  // namespace bluetooth
}  // namespace OHOS
