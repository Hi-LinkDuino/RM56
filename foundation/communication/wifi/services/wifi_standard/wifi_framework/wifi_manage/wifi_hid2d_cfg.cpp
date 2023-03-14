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

#include "wifi_hid2d_cfg.h"
#include <random>
#include <sstream>
#include "wifi_logger.h"

DEFINE_WIFILOG_P2P_LABEL("WifiHid2dCfg");
namespace OHOS {
namespace Wifi {
static constexpr int INVALID_VALUE_OR_TYPE = -1;
static constexpr int INDEX_START = 0;
static constexpr int INDEX_LENGTH = 1;
static constexpr int INDEX_VALUE = 2;
static constexpr int TYPE_VERSION = 1;
static constexpr int TYPE_P2P_CHANNEL_OPT = 2;
static constexpr int TYPE_DBDC = 3;
static constexpr int TYPE_CSA = 4;
static constexpr int TYPE_RADAR_DETECT = 5;
static constexpr int TYPE_CREATE_DFS_CHANNEL = 6;
static constexpr int TYPE_CREATE_INDOOR_CHANNEL = 7;
static constexpr int TYPE_CURRENT_STA_CHANNEL = 8;
static constexpr int TYPE_CURRENT_AP_IS_PORTAL = 9;
static constexpr int TYPE_CURRENT_AP_SSID = 10;
static constexpr int TYPE_CURRENT_AP_BSSID = 11;
static constexpr int TYPE_CURRENT_STA_HAS_INTERNET = 12;
static constexpr int TYPE_P2P_SUPPORT_CHANNEL = 13;
static constexpr int TYPE_CURRENT_AP_PASSWORD = 14;
static constexpr int TYPE_CURRENT_AP_SECURITY_TYPE = 15;
static constexpr int TYPE_CURRENT_AP_ROUTE_TYPE = 16;
static constexpr int TYPE_DEVICE_TYPE = 17;
static constexpr int TYPE_CURRENT_SESSION_ID_TYPE = 18;
static constexpr int TYPE_STA_STATE_FOR_CALLBACK = 1;
static constexpr int TYPE_STA_CHANNEL_FOR_CALLBACK = 2;
static constexpr int TYPE_AP_SSID_FOR_CALLBACK = 3;
static constexpr int TYPE_AP_BSSID_FOR_CALLBACK = 4;
static constexpr int LENGTH_OFFSET = 1;
static constexpr int VALUE_OFFSET = 2;
static constexpr int NORMAL_TYPE_LENGTH = 1; // Most scenes of length(length) in Tlv is 1
static constexpr int TYPE_AND_LENGTH = 2;    // Most scenes of length(type + length) is 2
static constexpr int TYPICAL_TLV_LENGTH = 3; // Most scenes of length(tpye + length +value) is 3
static constexpr int MAX_SSID_LENGTH = 32;
static constexpr int MAX_PASSWORD_LENGTH = 64;
static constexpr int BSSID_LENGTH = 6;
static constexpr int BYTE_MASK = 0xFF;
static constexpr int MAX_BYTES = 255;
static constexpr int THE_NUM_FROM_CH1_TO_CH11 = 11;
static constexpr int DBAC_VERSION = 2;
static constexpr int SESSION_ID_LENGTH = 20;
static constexpr int CHANNEL_12 = 12;

DeviceWifiInfo::DeviceWifiInfo()
{
    m_version = DBAC_VERSION;
    m_isChooseP2pChannelOpt = false;
    m_isSupportDbdc = false;
    m_isSupportCsa = false;
    m_isP2pSupportRadarDetect = false;
    m_isP2pSupportDfsChannel = false;
    m_isP2pSupportIndoorChannel = false;
    m_isPortalAp = false;
    m_staChannel = 0;
    m_currentApSsid = "";
    m_currentApBssid = "";
    m_isCurrentApHasInternet = false;
    m_pwd = "";
    m_sessionId = "";
    m_securityType = INVALID_VALUE_OR_TYPE;
    m_routerType = INVALID_VALUE_OR_TYPE;
    m_deviceType = DeviceClass::Default;
}

DeviceWifiInfo::~DeviceWifiInfo()
{
}

DeviceClass DeviceWifiInfo::GetDeviceClass()
{
    /* Read from configuration items */
    return DeviceClass::Default;
}

void DeviceWifiInfo::UpdateDeviceCapability()
{
    m_isChooseP2pChannelOpt = false;
    m_isSupportDbdc = false;
    m_isSupportCsa = false;
    m_isP2pSupportRadarDetect = false;
    m_isP2pSupportDfsChannel = false;
    m_isP2pSupportIndoorChannel = false;
    m_deviceType = GetDeviceClass();
}

void DeviceWifiInfo::ResetWifiDeviceCfg()
{
    m_staChannel = 0;
    m_currentApSsid = "";
    m_currentApBssid = "";
    m_isCurrentApHasInternet = false;
    m_pwd = "";
    m_isPortalAp = false;
    m_securityType = INVALID_VALUE_OR_TYPE;
    m_routerType = INVALID_VALUE_OR_TYPE;
}

int DeviceWifiInfo::GetVersion()
{
    WIFI_LOGI("GetVersion :  %{public}d ", m_version);
    return m_version;
}

void DeviceWifiInfo::SetVerison(int version)
{
    if (version <= 0) {
        return;
    }
    m_version = version;
}

bool DeviceWifiInfo::GetCapsOfChooseP2pChannelOpt()
{
    return m_isChooseP2pChannelOpt;
}

void DeviceWifiInfo::SetCapsOfChooseP2pChannelOpt(bool isEnabled)
{
    m_isChooseP2pChannelOpt = isEnabled;
}

bool DeviceWifiInfo::GetCapsOfDbdc()
{
    return m_isSupportDbdc;
}

void DeviceWifiInfo::SetCapsOfDbdc(bool isSupportDbdc)
{
    m_isSupportDbdc = isSupportDbdc;
}

bool DeviceWifiInfo::GetCapsOfCsa()
{
    return m_isSupportCsa;
}

void DeviceWifiInfo::SetCapsOfCsa(bool isSupportCsa)
{
    m_isSupportCsa = isSupportCsa;
}

bool DeviceWifiInfo::GetCapsOfP2pRadarDetect()
{
    return m_isP2pSupportRadarDetect;
}

void DeviceWifiInfo::SetCapsOfP2pRadarDetect(bool isP2pSupportRadarDetect)
{
    m_isP2pSupportRadarDetect = isP2pSupportRadarDetect;
}

bool DeviceWifiInfo::GetCapsOfP2pDfsChannel() {
    return m_isP2pSupportDfsChannel;
}

void DeviceWifiInfo::SetCapsOfP2pDfsChannel(bool isP2pSupportDfsChannel)
{
    m_isP2pSupportDfsChannel = isP2pSupportDfsChannel;
}

bool DeviceWifiInfo::GetCapsOfP2pIndoorChannel()
{
    return m_isP2pSupportIndoorChannel;
}

void DeviceWifiInfo::SetCapsOfP2pIndoorChannel(bool isP2pSupportIndoorChannel)
{
    m_isP2pSupportIndoorChannel = isP2pSupportIndoorChannel;
}

void DeviceWifiInfo::SetStaChannel(int channel)
{
    WIFI_LOGI("Set sta channel: %{public}d ", channel);
    if (channel < 0) {
        return;
    }
    m_staChannel = channel;
}

int DeviceWifiInfo::GetStaChannel()
{
    return m_staChannel;
}

bool DeviceWifiInfo::GetApType()
{
    return m_isPortalAp;
}

void DeviceWifiInfo::SetApType(bool isPortalAp)
{
    m_isPortalAp = isPortalAp;
}

std::string DeviceWifiInfo::GetCurrentApSsid()
{
    return m_currentApSsid;
}

void DeviceWifiInfo::SetCurrentApSsid(const std::string& ssid)
{
    m_currentApSsid = ssid;
}

std::string DeviceWifiInfo::GetCurrentApBssid() {
    return m_currentApBssid;
}

void DeviceWifiInfo::SetCurrentApBssid(const std::string& bssid)
{
    m_currentApBssid = bssid;
}

void DeviceWifiInfo::SetCurrentApHasInternet(bool hasInternet)
{
    m_isCurrentApHasInternet = hasInternet;
}

bool DeviceWifiInfo::GetCurrentApHasInternet()
{
    return m_isCurrentApHasInternet;
}

std::vector<int> DeviceWifiInfo::GetP2pSupportChannel()
{
    return m_P2pSupportChannelList;
}

void DeviceWifiInfo::SetP2pSupportChannel(std::vector<int>& channels)
{
    if (channels.empty()) {
        return;
    }
    m_P2pSupportChannelList = channels;
}

std::string DeviceWifiInfo::GetApPwd()
{
    return m_pwd;
}

void DeviceWifiInfo::SetApPwd(const std::string& pwd)
{
    m_pwd = pwd;
}

int DeviceWifiInfo::GetApSecurityType()
{
    return m_securityType;
}

void DeviceWifiInfo::SetApSecurityType(int securityType)
{
    m_securityType = securityType;
}

int DeviceWifiInfo::GetRouterType()
{
    return m_routerType;
}

std::string DeviceWifiInfo::GenerateSessionId()
{
    constexpr int sessionIdSize = 20;
    constexpr int hexMaxNum = 15;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, hexMaxNum);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i != sessionIdSize; ++i) {
        ss << dis(gen);
    }
    m_sessionId = ss.str();
    return m_sessionId;
}

std::string DeviceWifiInfo::GetSessionId()
{
    return m_sessionId;
}

void DeviceWifiInfo::SetSessionId(std::string sessionId)
{
    m_sessionId = sessionId;
}

void DeviceWifiInfo::ClearSessionId()
{
    m_sessionId = "";
}

void DeviceWifiInfo::SetRouteType(int routerType)
{
    m_routerType = routerType;
}

DeviceClass DeviceWifiInfo::GetDeviceType()
{
    return m_deviceType;
}

void DeviceWifiInfo::SetDeviceType(DeviceClass deviceType)
{
    m_deviceType = deviceType;
}

int DeviceWifiInfo::GetValueFromType(int type)
{
    int value = INVALID_VALUE_OR_TYPE;
    switch (type) {
        case TYPE_VERSION:
            value = GetVersion();
            break;
        case TYPE_P2P_CHANNEL_OPT:
            value = GetCapsOfChooseP2pChannelOpt() ? 1 : 0;
            break;
        case TYPE_DBDC:
            value = GetCapsOfDbdc() ? 1 : 0;
            break;
        case TYPE_CSA:
            value = GetCapsOfCsa() ? 1 : 0;
            break;
        case TYPE_RADAR_DETECT:
            value = GetCapsOfP2pRadarDetect() ? 1 : 0;
            break;
        case TYPE_CREATE_DFS_CHANNEL:
            value = GetCapsOfP2pDfsChannel() ? 1 : 0;
            break;
        case TYPE_CREATE_INDOOR_CHANNEL:
            value = GetCapsOfP2pIndoorChannel() ? 1 : 0;
            break;
        case TYPE_CURRENT_STA_CHANNEL:
            value = GetStaChannel();
            break;
        case TYPE_CURRENT_AP_IS_PORTAL:
            value = GetApType() ? 1 : 0;
            break;
        default:
            WIFI_LOGI("GetValueFromType invalid type = %{public}d", type);
            break;
    }
    return value;
}

void DeviceWifiInfo::SetDeviceCfg(int type, int inputValue)
{
    switch (type) {
        case TYPE_VERSION:
            SetVerison(inputValue);
            break;
        case TYPE_P2P_CHANNEL_OPT:
            SetCapsOfChooseP2pChannelOpt(inputValue == 1);
            break;
        case TYPE_DBDC:
            SetCapsOfDbdc(inputValue == 1);
            break;
        case TYPE_CSA:
            SetCapsOfCsa(inputValue == 1);
            break;
        case TYPE_RADAR_DETECT:
            SetCapsOfP2pRadarDetect(inputValue == 1);
            break;
        case TYPE_CREATE_DFS_CHANNEL:
            SetCapsOfP2pDfsChannel(inputValue == 1);
            break;
        case TYPE_CREATE_INDOOR_CHANNEL:
            SetCapsOfP2pIndoorChannel(inputValue == 1);
            break;
        case TYPE_CURRENT_STA_CHANNEL:
            SetStaChannel(inputValue);
            break;
        case TYPE_CURRENT_AP_IS_PORTAL:
            SetApType(inputValue == 1);
            break;
        default:
            WIFI_LOGI("SetDeviceCfg invalid type = %{public}d", type);
            break;
    }
}

WifiHid2dCfg::WifiHid2dCfg()
{
}

WifiHid2dCfg::~WifiHid2dCfg()
{
}

WifiHid2dCfg& WifiHid2dCfg::GetInstance()
{
    static WifiHid2dCfg inst;
    return inst;
}

DeviceWifiInfo WifiHid2dCfg::m_selfDeviceInfo;
DeviceWifiInfo WifiHid2dCfg::m_peerDeviceInfo;

DeviceWifiInfo& WifiHid2dCfg::GetSelfDeviceCfgInfo()
{
    return m_selfDeviceInfo;
}

DeviceWifiInfo& WifiHid2dCfg::GetPeerDeviceCfgInfo()
{
    return m_peerDeviceInfo;
}

std::string WifiHid2dCfg::ParseStringFromByteArray(char* data, int dataLen, int index, int maxLength)
{
    if (data == nullptr || dataLen < index + INDEX_VALUE) {
        return "";
    }
    int length = data[index + INDEX_LENGTH] & BYTE_MASK;
    if (length > maxLength + 1) { /* +1 for the end character '\0' of the C-style string */
        WIFI_LOGE("invalid length = %{public}d", length);
        return "";
    }

    char* ssidArray = new char[maxLength + 1]; /* +1 for the end character '\0' of the C-style string */
    if (ssidArray == nullptr) {
        return "";
    }
    if (memcpy_s(ssidArray, maxLength + 1, data + index + INDEX_VALUE, length) != EOK) {
        WIFI_LOGE("parse string from byte array memcpy_s failed!");
        delete[] ssidArray;
        return "";
    }

    int needCopylen = FindIndexFromByteArray(ssidArray, maxLength + 1, 0);
    std::string s(needCopylen + 1, '\0');
    for (int i = 0; i <= needCopylen && i < maxLength + 1; ++i) {
        s[i] = ssidArray[i];
    }
    delete[] ssidArray;
    return s;
}

/** 2.4g channel count from channel 1 to channel 11 */
void WifiHid2dCfg::ParseP2pSupportChannelFromByteArray(char* data, int dataLen,
    int index, std::vector<int>& p2pSupportChannels)
{
    if (data == nullptr || dataLen < (index + INDEX_VALUE)) {
        return;
    }

    int length = data[index + LENGTH_OFFSET] & BYTE_MASK;
    if (dataLen < (index + length)) {
        return;
    }
    // To save space, channels 1 to 11 are not carried when the peer information is obtained.
    // channels 1 to 11 are supported by all countries, p2pSupportChannels should include it
    int i = 0;
    for (i = 0; i < THE_NUM_FROM_CH1_TO_CH11; i++) {
        p2pSupportChannels.emplace_back(i + 1);
    }
    for (int j = 0; j < data[index + LENGTH_OFFSET]; j++) {
        p2pSupportChannels.emplace_back(data[index + VALUE_OFFSET + j] & BYTE_MASK);
    }
    std::sort(p2pSupportChannels.begin(), p2pSupportChannels.end());
}

void WifiHid2dCfg::HandlePeerApPassword()
{
}

void WifiHid2dCfg::HandleTlvData(int type, char* cfgData, int cfgDataLen, int index)
{
    std::vector<int> p2pSupportChannels;
    switch (type) {
        case TYPE_CURRENT_AP_SSID:
            m_peerDeviceInfo.SetCurrentApSsid(ParseStringFromByteArray(cfgData, cfgDataLen, index, MAX_SSID_LENGTH));
            break;
        case TYPE_CURRENT_AP_BSSID:
            unsigned char macAddr[BSSID_LENGTH];
            if (memcpy_s(macAddr, BSSID_LENGTH, cfgData + index + INDEX_VALUE, BSSID_LENGTH) != EOK) {
                return;
            }
            m_peerDeviceInfo.SetCurrentApBssid(MacArrayToStr(macAddr));
            break;
        case TYPE_CURRENT_STA_HAS_INTERNET:
            m_peerDeviceInfo.SetCurrentApHasInternet((cfgData[index + VALUE_OFFSET] & BYTE_MASK) == 0x01);
            break;
        case TYPE_P2P_SUPPORT_CHANNEL:
            ParseP2pSupportChannelFromByteArray(cfgData, cfgDataLen, index, p2pSupportChannels);
            m_peerDeviceInfo.SetP2pSupportChannel(p2pSupportChannels);
            break;
        case TYPE_CURRENT_AP_PASSWORD:
            m_peerDeviceInfo.SetApPwd(ParseStringFromByteArray(cfgData, cfgDataLen, index, MAX_PASSWORD_LENGTH));
            break;
        case TYPE_CURRENT_AP_SECURITY_TYPE:
            m_peerDeviceInfo.SetApSecurityType(cfgData[index + VALUE_OFFSET] & BYTE_MASK);
            break;
        case TYPE_CURRENT_AP_ROUTE_TYPE:
            m_peerDeviceInfo.SetRouteType(cfgData[index + VALUE_OFFSET] & BYTE_MASK);
            break;
        case TYPE_DEVICE_TYPE:
            m_peerDeviceInfo.SetDeviceType(DeviceClass(cfgData[index + VALUE_OFFSET] & BYTE_MASK));
            break;
        case TYPE_CURRENT_SESSION_ID_TYPE:
            m_peerDeviceInfo.SetSessionId(ParseStringFromByteArray(cfgData, cfgDataLen, index, SESSION_ID_LENGTH));
            break;
        default:
            WIFI_LOGE("handle tlv data invalid type = %{public}d", type);
            return;
    }
}

int WifiHid2dCfg::ParsePeerDeviceCfgInfo(PeerCfgType cfgType, char* cfgData, int cfgDataLen)
{
    if (cfgData == nullptr || cfgDataLen == 0) {
        return INVALID_VALUE_OR_TYPE;
    }
    if (cfgType != PeerCfgType::TYPE_OF_SET_PEER_CONFIG) {
        return INVALID_VALUE_OR_TYPE;
    }
    m_peerDeviceInfo.ResetWifiDeviceCfg();
    int length = cfgData[INDEX_START] & BYTE_MASK;
    if (cfgDataLen != length) {
        return INVALID_VALUE_OR_TYPE;
    }

    WIFI_LOGI("parse peer device cfg length = %{public}d", length);
    for (int i = 1; i < length;) {
        if ((i + LENGTH_OFFSET) >= length) {
            break;
        }
        int valueLength = cfgData[i + LENGTH_OFFSET] & BYTE_MASK;
        if ((i + valueLength + TYPE_AND_LENGTH) > length) {
            break;
        }
        int type = cfgData[i];
        int valueIndex = i + VALUE_OFFSET;
        if (type > 0 && type <= TYPE_CURRENT_AP_IS_PORTAL && valueIndex < length) {
            m_peerDeviceInfo.SetDeviceCfg(type, cfgData[valueIndex] & BYTE_MASK);
            i += valueLength + TYPE_AND_LENGTH;
            continue;
        }
        HandleTlvData(type, cfgData, cfgDataLen, i);
        i += valueLength + TYPE_AND_LENGTH;
    }

    if (m_peerDeviceInfo.GetApSecurityType() != INVALID_VALUE_OR_TYPE) {
        HandlePeerApPassword();
    }
    return 0;
}

int WifiHid2dCfg::ParsePeerDeviceStaChanngeInfo(PeerCfgType cfgType, char* cfgData, int cfgDataLen)
{
    if (cfgData == nullptr || cfgDataLen == 0) {
        return INVALID_VALUE_OR_TYPE;
    }
    if (cfgType != PeerCfgType::TYPE_OF_SET_PEER_STATE_CHANGE) {
        return INVALID_VALUE_OR_TYPE;
    }
    if (cfgDataLen != cfgData[0]) {
        return INVALID_VALUE_OR_TYPE;
    }
    for (int i = 1; i < cfgDataLen;) {
        int type = cfgData[i];
        switch (type) {
            case TYPE_STA_STATE_FOR_CALLBACK:
            case TYPE_STA_CHANNEL_FOR_CALLBACK:
            case TYPE_AP_SSID_FOR_CALLBACK:
            case TYPE_AP_BSSID_FOR_CALLBACK:
            default:
                break;
        }
        i += (cfgData[i + LENGTH_OFFSET] & BYTE_MASK) + TYPE_AND_LENGTH;
    }
    return 0;
}

int WifiHid2dCfg::FindIndexFromByteArray(char* data, int dataLen, int value)
{
    if (data == nullptr) {
        return 0;
    }
    int i = 0;
    for (i = 0; i < dataLen; i++) {
        if (data[i] == value) {
            return i;
        }
    }
    return i;
}

int WifiHid2dCfg::SetBssidByte(int typeBssidValue, char* dataArray, int dataArrayLen, int offset)
{
    if (dataArray == nullptr) {
        return 0;
    }
    int length = INDEX_VALUE + BSSID_LENGTH;
    if (dataArrayLen < (offset + length)) {
        return 0;
    }
    dataArray[offset] = (char)typeBssidValue;
    std::string bssid = m_selfDeviceInfo.GetCurrentApBssid();
    dataArray[offset + INDEX_LENGTH] = BSSID_LENGTH;

    unsigned char macByteAddr[BSSID_LENGTH] = { 0 };
    if (!bssid.empty()) {
        if (MacStrToArray(bssid, macByteAddr) != EOK) {
            return 0;
        }
    }

    if (memcpy_s(dataArray + offset + INDEX_VALUE, dataArrayLen - (offset + INDEX_VALUE),
        macByteAddr, BSSID_LENGTH) != EOK) {
        return 0;
    }
    return length;
}

int WifiHid2dCfg::SetP2pSupportChannelByte(char* dataArray, int dataArrayLen, int offset)
{
    if (dataArray == nullptr || dataArrayLen < (offset + INDEX_LENGTH)) {
        return 0;
    }
    std::vector<int> p2pSupportChannelList = m_selfDeviceInfo.GetP2pSupportChannel();
    if (p2pSupportChannelList.empty() || (p2pSupportChannelList.size() - THE_NUM_FROM_CH1_TO_CH11 <= 0)) {
        WIFI_LOGI("Channel list is invalid.");
        return 0;
    }
    dataArray[offset] = TYPE_P2P_SUPPORT_CHANNEL;
    int length = p2pSupportChannelList.size() - THE_NUM_FROM_CH1_TO_CH11;
    dataArray[offset + INDEX_LENGTH] = (char)length;
    if (dataArrayLen < (offset + length + INDEX_VALUE)) {
        return 0;
    }
    int index = 0;
    for (int channel : p2pSupportChannelList) {
        // To save space, channels 1 to 11 are not carried when exchanging information with the peer device for
        // Channels 1 to 11 are supported by all countries and dataArray should not contains channel 1 to 11
        if (channel >= CHANNEL_12) {
            dataArray[offset + INDEX_VALUE + index] = (char)channel;
            index++;
        }
    }
    return length + VALUE_OFFSET;
}

int WifiHid2dCfg::BuildTlvForIntVal(int type, int Value, char* tlvData, int tlvDataLen, int offset)
{
    if (tlvData == nullptr) {
        return 0;
    }
    if (tlvDataLen <= (offset + INDEX_VALUE)) {
        return 0;
    }
    tlvData[offset] = (char)type;
    tlvData[offset + INDEX_LENGTH] = NORMAL_TYPE_LENGTH;
    tlvData[offset + INDEX_VALUE] = (char)Value;
    return TYPICAL_TLV_LENGTH;
}

int WifiHid2dCfg::BuildTlvForStrVal(int type, std::string value, char* tlvData, int tlvDataLen, int offset)
{
    if (tlvData == nullptr) {
        return 0;
    }
    if (value.empty() || (type == TYPE_CURRENT_AP_PASSWORD && value.length() > MAX_PASSWORD_LENGTH) ||
        (type == TYPE_CURRENT_AP_SSID && value.length() > MAX_SSID_LENGTH)) {
        tlvData[offset] = (char)type;
        tlvData[offset + LENGTH_OFFSET] = (char)NORMAL_TYPE_LENGTH;
        tlvData[offset + VALUE_OFFSET] = 0;
        return TYPICAL_TLV_LENGTH;
    }

    /* +1 for the end character '\0' of the C-style string */
    if (tlvDataLen < offset + value.length() + 1) {
        return 0;
    }
    tlvData[offset] = (char)type;
    /* +1 for the end character '\0' of the C-style string */
    tlvData[offset + LENGTH_OFFSET] = (char)value.length() + 1;
    if (memcpy_s(tlvData + offset + VALUE_OFFSET, tlvDataLen - (offset + VALUE_OFFSET),
        value.c_str(), value.length() + 1) != EOK) {
        return 0;
    }
    return tlvData[offset + LENGTH_OFFSET] + TYPE_AND_LENGTH;
}

void WifiHid2dCfg::HandlePeerStaStateChange(char* data, int dataLen, int index)
{
    if (data == nullptr || dataLen <= (index + INDEX_VALUE)) {
        return;
    }
    if (data[index + INDEX_VALUE] == 0) { // 0 : disconnect
        m_peerDeviceInfo.ResetWifiDeviceCfg();
    }
}

void WifiHid2dCfg::GetSelfDeviceCfg(SelfCfgType cfgType, char cfgInfo[CFG_DATA_MAX_BYTES], int& getDatValidLen)
{
    getDatValidLen = 0;
    if ((cfgType != SelfCfgType::TYPE_OF_GET_SELF_CONFIG &&
        cfgType != SelfCfgType::TYPE_OF_GET_SELF_CONFIG_WITH_PASSWORD)) {
        return;
    }

    char* totalCfgInfo = new char[MAX_BYTES];
    if (totalCfgInfo == nullptr) {
        return;
    }
    if (memset_s(totalCfgInfo, MAX_BYTES, 0, MAX_BYTES) != EOK) {
        delete[] totalCfgInfo;
        return;
    }

    int i = 1;
    int position = 1;
    for (i = 1; i <= TYPE_CURRENT_AP_IS_PORTAL; i++) {
        position += BuildTlvForIntVal(i, m_selfDeviceInfo.GetValueFromType(i), totalCfgInfo, MAX_BYTES, position);
    }

    std::string ssid = m_selfDeviceInfo.GetCurrentApSsid();
    position += BuildTlvForStrVal(TYPE_CURRENT_AP_SSID, ssid, totalCfgInfo, MAX_BYTES, position);

    position += SetBssidByte(TYPE_CURRENT_AP_BSSID, totalCfgInfo, MAX_BYTES, position);

    position += BuildTlvForIntVal(TYPE_CURRENT_STA_HAS_INTERNET, m_selfDeviceInfo.GetCurrentApHasInternet() ? 1 : 0,
        totalCfgInfo, MAX_BYTES, position);

    position += SetP2pSupportChannelByte(totalCfgInfo, MAX_BYTES, position);
    if (cfgType == SelfCfgType::TYPE_OF_GET_SELF_CONFIG_WITH_PASSWORD) {
        std::string pwd = m_selfDeviceInfo.GetApPwd();
        position += BuildTlvForStrVal(TYPE_CURRENT_AP_PASSWORD, pwd, totalCfgInfo, MAX_BYTES, position);
        position += BuildTlvForIntVal(TYPE_CURRENT_AP_SECURITY_TYPE, m_selfDeviceInfo.GetApSecurityType(),
            totalCfgInfo, MAX_BYTES, position);
    }

    position += BuildTlvForIntVal(TYPE_CURRENT_AP_ROUTE_TYPE, m_selfDeviceInfo.GetRouterType(),
        totalCfgInfo, MAX_BYTES, position);

    position += BuildTlvForIntVal(TYPE_DEVICE_TYPE, static_cast<int>(m_selfDeviceInfo.GetDeviceType()),
        totalCfgInfo, MAX_BYTES, position);

    std::string sessionID = m_selfDeviceInfo.GenerateSessionId();
    position += BuildTlvForStrVal(TYPE_CURRENT_SESSION_ID_TYPE, sessionID, totalCfgInfo, MAX_BYTES, position);

    totalCfgInfo[INDEX_START] = (char)position;
    getDatValidLen = totalCfgInfo[INDEX_START] & BYTE_MASK;
    WIFI_LOGI("self device cfg length = %{public}d", getDatValidLen);
    if (getDatValidLen <= 0) {
        getDatValidLen = 0;
        delete[] totalCfgInfo;
        return;
    }
    if (memcpy_s(cfgInfo, CFG_DATA_MAX_BYTES, totalCfgInfo, getDatValidLen) != EOK) {
        WIFI_LOGI("GetSelfDeviceCfg  memcpy_s failed!");
        getDatValidLen = 0;
    }
    delete[] totalCfgInfo;
}

int WifiHid2dCfg::Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType, char cfgInfo[CFG_DATA_MAX_BYTES], int setDataValidLen)
{
    WIFI_LOGI("Hid2dSetPeerWifiCfgInfo  cfgType = %{public}d", cfgType);
    if (cfgType == PeerCfgType::TYPE_OF_SET_PEER_CONFIG) {
        return ParsePeerDeviceCfgInfo(cfgType, cfgInfo, setDataValidLen);
    }
    else if (cfgType == PeerCfgType::TYPE_OF_SET_PEER_STATE_CHANGE) {
        return ParsePeerDeviceStaChanngeInfo(cfgType, cfgInfo, setDataValidLen);
    } else {
        return INVALID_VALUE_OR_TYPE;
    }
}
}  // namespace Wifi
}  // namespace OHOS