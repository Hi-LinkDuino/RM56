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

#include "wifi_config_file_spec.h"

namespace OHOS {
namespace Wifi {
static void ClearWifiDeviceConfig(WifiDeviceConfig &item)
{
    item.networkId = 0;
    item.status = 0;
    item.bssid.clear();
    item.ssid.clear();
    item.band = 0;
    item.channel = 0;
    item.frequency = 0;
    item.level = 0;
    item.isPasspoint = false;
    item.isEphemeral = false;
    item.preSharedKey.clear();
    item.keyMgmt.clear();
    for (int i = 0; i < WEPKEYS_SIZE; ++i) {
        item.wepKeys[i].clear();
    }
    item.wepTxKeyIndex = 0;
    item.priority = 0;
    item.hiddenSSID = false;
    return;
}

static void ClearWifiDeviceConfigIp(WifiDeviceConfig &item)
{
    item.wifiIpConfig.assignMethod = AssignIpMethod::DHCP;
    item.wifiIpConfig.staticIpAddress.ipAddress.address.family = 0;
    item.wifiIpConfig.staticIpAddress.ipAddress.address.addressIpv4 = 0;
    item.wifiIpConfig.staticIpAddress.ipAddress.address.addressIpv6.clear();
    item.wifiIpConfig.staticIpAddress.ipAddress.prefixLength = 0;
    item.wifiIpConfig.staticIpAddress.ipAddress.flags = 0;
    item.wifiIpConfig.staticIpAddress.ipAddress.scope = 0;
    item.wifiIpConfig.staticIpAddress.gateway.family = 0;
    item.wifiIpConfig.staticIpAddress.gateway.addressIpv4 = 0;
    item.wifiIpConfig.staticIpAddress.gateway.addressIpv6.clear();
    item.wifiIpConfig.staticIpAddress.dnsServer1.family = 0;
    item.wifiIpConfig.staticIpAddress.dnsServer1.addressIpv4 = 0;
    item.wifiIpConfig.staticIpAddress.dnsServer1.addressIpv6.clear();
    item.wifiIpConfig.staticIpAddress.dnsServer2.family = 0;
    item.wifiIpConfig.staticIpAddress.dnsServer2.addressIpv4 = 0;
    item.wifiIpConfig.staticIpAddress.dnsServer2.addressIpv6.clear();
    item.wifiIpConfig.staticIpAddress.domains.clear();
    return;
}

static void ClearWifiDeviceConfigEap(WifiDeviceConfig &item)
{
    item.wifiEapConfig.eap.clear();
    item.wifiEapConfig.identity.clear();
    item.wifiEapConfig.password.clear();
    return;
}

static void ClearWifiDeviceConfigProxy(WifiDeviceConfig &item)
{
    item.wifiProxyconfig.configureMethod = ConfigureProxyMethod::CLOSED;
    item.wifiProxyconfig.autoProxyConfig.pacWebAddress.clear();
    item.wifiProxyconfig.manualProxyConfig.serverHostName.clear();
    item.wifiProxyconfig.manualProxyConfig.serverPort = 0;
    item.wifiProxyconfig.manualProxyConfig.exclusionObjectList.clear();
    return;
}

static void ClearWifiDeviceConfigPrivacy(WifiDeviceConfig &item)
{
    item.wifiPrivacySetting = WifiPrivacyConfig::RANDOMMAC;
    return;
}

template<>
void ClearTClass<WifiDeviceConfig>(WifiDeviceConfig &item)
{
    ClearWifiDeviceConfig(item);
    ClearWifiDeviceConfigIp(item);
    ClearWifiDeviceConfigEap(item);
    ClearWifiDeviceConfigProxy(item);
    ClearWifiDeviceConfigPrivacy(item);
    return;
}

static int SetWifiDeviceConfigFirst(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (key == "networkId") {
        item.networkId = std::stoi(value);
    } else if (key == "status") {
        item.status = std::stoi(value);
    } else if (key == "bssid") {
        item.bssid = value;
    } else if (key == "ssid") {
        item.ssid = value;
    } else if (key == "band") {
        item.band = std::stoi(value);
    } else if (key == "channel") {
        item.channel = std::stoi(value);
    } else if (key == "frequency") {
        item.frequency = std::stoi(value);
    } else if (key == "level") {
        item.level = std::stoi(value);
    } else if (key == "isPasspoint") {
        item.isPasspoint = std::stoi(value);
    } else if (key == "isEphemeral") {
        item.isEphemeral = std::stoi(value);
    } else if (key == "preSharedKey") {
        item.preSharedKey = value;
    } else if (key == "keyMgmt") {
        item.keyMgmt = value;
    } else if (key == "wepTxKeyIndex") {
        item.wepTxKeyIndex = std::stoi(value);
    } else if (key == "priority") {
        item.priority = std::stoi(value);
    } else {
        return -1;
    }
    return 0;
}

static void SetWifiDeviceConfig(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (SetWifiDeviceConfigFirst(item, key, value) == 0) {
        return;
    }
    if (key == "hiddenSSID") {
        item.hiddenSSID = std::stoi(value);
    } else if (key.compare(0, strlen("wepKeys"), "wepKeys") == 0) {
        int pos = std::stoi(key.substr(strlen("wepKeys") + 1));
        if (pos >= 0 && pos < WEPKEYS_SIZE) {
            item.wepKeys[pos] = value;
        }
    }
    return;
}

static void SetWifiDeviceConfigIp(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (key == "wifiIpConfig.assignMethod") {
        item.wifiIpConfig.assignMethod = AssignIpMethod(std::stoi(value));
    } else if (key == "wifiIpConfig.staticIpAddress.ipAddress.address.family") {
        item.wifiIpConfig.staticIpAddress.ipAddress.address.family = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.ipAddress.address.addressIpv4") {
        item.wifiIpConfig.staticIpAddress.ipAddress.address.SetIpv4Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.ipAddress.address.addressIpv6") {
        item.wifiIpConfig.staticIpAddress.ipAddress.address.SetIpv6Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.ipAddress.prefixLength") {
        item.wifiIpConfig.staticIpAddress.ipAddress.prefixLength = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.ipAddress.flags") {
        item.wifiIpConfig.staticIpAddress.ipAddress.flags = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.ipAddress.scope") {
        item.wifiIpConfig.staticIpAddress.ipAddress.scope = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.gateway.family") {
        item.wifiIpConfig.staticIpAddress.gateway.family = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.gateway.addressIpv4") {
        item.wifiIpConfig.staticIpAddress.gateway.SetIpv4Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.gateway.addressIpv6") {
        item.wifiIpConfig.staticIpAddress.gateway.SetIpv6Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.dnsServer1.family") {
        item.wifiIpConfig.staticIpAddress.dnsServer1.family = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.dnsServer1.addressIpv4") {
        item.wifiIpConfig.staticIpAddress.dnsServer1.SetIpv4Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.dnsServer1.addressIpv6") {
        item.wifiIpConfig.staticIpAddress.dnsServer1.SetIpv6Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.dnsServer2.family") {
        item.wifiIpConfig.staticIpAddress.dnsServer2.family = std::stoi(value);
    } else if (key == "wifiIpConfig.staticIpAddress.dnsServer2.addressIpv4") {
        item.wifiIpConfig.staticIpAddress.dnsServer2.SetIpv4Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.dnsServer2.addressIpv6") {
        item.wifiIpConfig.staticIpAddress.dnsServer2.SetIpv6Address(value);
    } else if (key == "wifiIpConfig.staticIpAddress.domains") {
        item.wifiIpConfig.staticIpAddress.domains = value;
    }
    return;
}

static void SetWifiDeviceConfigEap(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (key == "wifiEapConfig.eap") {
        item.wifiEapConfig.eap = value;
    } else if (key == "wifiEapConfig.identity") {
        item.wifiEapConfig.identity = value;
    } else if (key == "wifiEapConfig.password") {
        item.wifiEapConfig.password = value;
    }
    return;
}

static void SetWifiDeviceConfigProxy(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (key == "wifiProxyconfig.configureMethod") {
        item.wifiProxyconfig.configureMethod = ConfigureProxyMethod(std::stoi(value));
    } else if (key == "wifiProxyconfig.autoProxyConfig.pacWebAddress") {
        item.wifiProxyconfig.autoProxyConfig.pacWebAddress = value;
    } else if (key == "wifiProxyconfig.ManualProxyConfig.serverHostName") {
        item.wifiProxyconfig.manualProxyConfig.serverHostName = value;
    } else if (key == "wifiProxyconfig.ManualProxyConfig.serverPort") {
        item.wifiProxyconfig.manualProxyConfig.serverPort = std::stoi(value);
    } else if (key == "wifiProxyconfig.ManualProxyConfig.exclusionObjectList") {
        item.wifiProxyconfig.manualProxyConfig.exclusionObjectList = value;
    }
    return;
}

static void SetWifiDeviceconfigPrivacy(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (key == "wifiPrivacySetting") {
        item.wifiPrivacySetting = WifiPrivacyConfig(std::stoi(value));
    }
}

template<>
void SetTClassKeyValue<WifiDeviceConfig>(WifiDeviceConfig &item, const std::string &key, const std::string &value)
{
    if (key.compare(0, strlen("wifiIpConfig"), "wifiIpConfig") == 0) {
        SetWifiDeviceConfigIp(item, key, value);
    } else if (key.compare(0, strlen("wifiEapConfig"), "wifiEapConfig") == 0) {
        SetWifiDeviceConfigEap(item, key, value);
    } else if (key.compare(0, strlen("wifiProxyconfig"), "wifiProxyconfig") == 0) {
        SetWifiDeviceConfigProxy(item, key, value);
    } else if (key.compare(0, strlen("wifiPrivacySetting"), "wifiPrivacySetting") == 0) {
        SetWifiDeviceconfigPrivacy(item, key, value);
    } else {
        SetWifiDeviceConfig(item, key, value);
    }
    return;
}

template<>
std::string GetTClassName<WifiDeviceConfig>()
{
    return "WifiDeviceConfig";
}

static std::string OutPutWifiDeviceConfig(WifiDeviceConfig &item)
{
    std::ostringstream ss;
    ss << "status=" << item.status << std::endl;
    ss << "bssid=" << item.bssid << std::endl;
    ss << "ssid=" << item.ssid << std::endl;
    ss << "band=" << item.band << std::endl;
    ss << "channel=" << item.channel << std::endl;
    ss << "frequency=" << item.frequency << std::endl;
    ss << "level=" << item.level << std::endl;
    ss << "isPasspoint=" << item.isPasspoint << std::endl;
    ss << "isEphemeral=" << item.isEphemeral << std::endl;
    ss << "preSharedKey=" << item.preSharedKey << std::endl;
    ss << "keyMgmt=" << item.keyMgmt << std::endl;
    ss << "wepTxKeyIndex=" << item.wepTxKeyIndex << std::endl;
    ss << "priority=" << item.priority << std::endl;
    ss << "hiddenSSID=" << (int)item.hiddenSSID << std::endl;
    for (int i = 0; i < WEPKEYS_SIZE; ++i) {
        ss << "wepKeys_" << i << "=" << item.wepKeys[i] << std::endl;
    }
    return ss.str();
}

static std::string OutPutWifiDeviceConfigIp(WifiDeviceConfig &item)
{
    std::ostringstream ss;
    ss << "wifiIpConfig.assignMethod=" << (int)item.wifiIpConfig.assignMethod << std::endl;
    ss << "wifiIpConfig.staticIpAddress.ipAddress.address.family="
       << item.wifiIpConfig.staticIpAddress.ipAddress.address.family << std::endl;
    ss << "wifiIpConfig.staticIpAddress.ipAddress.address.addressIpv4="
       << item.wifiIpConfig.staticIpAddress.ipAddress.address.GetIpv4Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.ipAddress.address.addressIpv6="
       << item.wifiIpConfig.staticIpAddress.ipAddress.address.GetIpv6Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.ipAddress.prefixLength="
       << item.wifiIpConfig.staticIpAddress.ipAddress.prefixLength << std::endl;
    ss << "wifiIpConfig.staticIpAddress.ipAddress.flags=" << item.wifiIpConfig.staticIpAddress.ipAddress.flags
       << std::endl;
    ss << "wifiIpConfig.staticIpAddress.ipAddress.scope=" << item.wifiIpConfig.staticIpAddress.ipAddress.scope
       << std::endl;
    ss << "wifiIpConfig.staticIpAddress.gateway.family=" << item.wifiIpConfig.staticIpAddress.gateway.family
       << std::endl;
    ss << "wifiIpConfig.staticIpAddress.gateway.addressIpv4="
       << item.wifiIpConfig.staticIpAddress.gateway.GetIpv4Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.gateway.addressIpv6="
       << item.wifiIpConfig.staticIpAddress.gateway.GetIpv6Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.dnsServer1.family=" << item.wifiIpConfig.staticIpAddress.dnsServer1.family
       << std::endl;
    ss << "wifiIpConfig.staticIpAddress.dnsServer1.addressIpv4="
       << item.wifiIpConfig.staticIpAddress.dnsServer1.GetIpv4Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.dnsServer1.addressIpv6="
       << item.wifiIpConfig.staticIpAddress.dnsServer1.GetIpv6Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.dnsServer2.family=" << item.wifiIpConfig.staticIpAddress.dnsServer2.family
       << std::endl;
    ss << "wifiIpConfig.staticIpAddress.dnsServer2.addressIpv4="
       << item.wifiIpConfig.staticIpAddress.dnsServer2.GetIpv4Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.dnsServer2.addressIpv6="
       << item.wifiIpConfig.staticIpAddress.dnsServer2.GetIpv6Address() << std::endl;
    ss << "wifiIpConfig.staticIpAddress.domains=" << item.wifiIpConfig.staticIpAddress.domains << std::endl;
    return ss.str();
}

static std::string OutPutWifiDeviceConfigEap(WifiDeviceConfig &item)
{
    std::ostringstream ss;
    ss << "wifiEapConfig.eap=" << item.wifiEapConfig.eap << std::endl;
    ss << "wifiEapConfig.identity=" << item.wifiEapConfig.identity << std::endl;
    ss << "wifiEapConfig.password=" << item.wifiEapConfig.password << std::endl;
    return ss.str();
}

static std::string OutPutWifiDeviceConfigProxy(WifiDeviceConfig &item)
{
    std::ostringstream ss;
    ss << "wifiProxyconfig.configureMethod=" << (int)item.wifiProxyconfig.configureMethod << std::endl;
    ss << "wifiProxyconfig.autoProxyConfig.pacWebAddress=" << item.wifiProxyconfig.autoProxyConfig.pacWebAddress
       << std::endl;
    ss << "wifiProxyconfig.ManualProxyConfig.serverHostName=" << item.wifiProxyconfig.manualProxyConfig.serverHostName
       << std::endl;
    ss << "wifiProxyconfig.ManualProxyConfig.serverPort=" << item.wifiProxyconfig.manualProxyConfig.serverPort
       << std::endl;
    ss << "wifiProxyconfig.ManualProxyConfig.exclusionObjectList="
       << item.wifiProxyconfig.manualProxyConfig.exclusionObjectList << std::endl;
    return ss.str();
}

static std::string OutPutWifiDeviceConfigPrivacy(WifiDeviceConfig &item)
{
    std::ostringstream ss;
    ss << "wifiPrivacySetting=" << (int)item.wifiPrivacySetting << std::endl;
    return ss.str();
}

template<>
std::string OutTClassString<WifiDeviceConfig>(WifiDeviceConfig &item)
{
    std::ostringstream ss;
    ss << OutPutWifiDeviceConfig(item) << OutPutWifiDeviceConfigIp(item) << OutPutWifiDeviceConfigEap(item)
       << OutPutWifiDeviceConfigProxy(item) << OutPutWifiDeviceConfigPrivacy(item);
    return ss.str();
}

template<>
void ClearTClass<HotspotConfig>(HotspotConfig &item)
{
    item.SetSsid("");
    item.SetPreSharedKey("");
    item.SetSecurityType(KeyMgmt::NONE);
    item.SetBand(BandType::BAND_NONE);
    item.SetChannel(0);
    item.SetMaxConn(0);
    return;
}

template<>
void SetTClassKeyValue<HotspotConfig>(HotspotConfig &item, const std::string &key, const std::string &value)
{
    if (key == "ssid") {
        item.SetSsid(value);
    } else if (key == "preSharedKey") {
        item.SetPreSharedKey(value);
    } else if (key == "securityType") {
        item.SetSecurityType(static_cast<KeyMgmt>(std::stoi(value)));
    } else if (key == "band") {
        item.SetBand(static_cast<BandType>(std::stoi(value)));
    } else if (key == "channel") {
        item.SetChannel(std::stoi(value));
    } else if (key == "maxConn") {
        item.SetMaxConn(std::stoi(value));
    }
    return;
}

template<>
std::string GetTClassName<HotspotConfig>()
{
    return "HotspotConfig";
}

template<>
std::string OutTClassString<HotspotConfig>(HotspotConfig &item)
{
    std::ostringstream ss;
    ss << "ssid=" << item.GetSsid() << std::endl;
    ss << "preSharedKey=" << item.GetPreSharedKey() << std::endl;
    ss << "securityType=" << static_cast<int>(item.GetSecurityType()) << std::endl;
    ss << "band=" << static_cast<int>(item.GetBand()) << std::endl;
    ss << "channel=" << item.GetChannel() << std::endl;
    ss << "maxConn=" << item.GetMaxConn() << std::endl;
    return ss.str();
}

template<>
void ClearTClass<P2pVendorConfig>(P2pVendorConfig &item)
{
    item.SetRandomMacSupport(false);
    item.SetIsAutoListen(true);
    item.SetDeviceName("");
    item.SetPrimaryDeviceType("");
    item.SetSecondaryDeviceType("");
    return;
}

template<>
void SetTClassKeyValue<P2pVendorConfig>(P2pVendorConfig &item, const std::string &key, const std::string &value)
{
    if (key == "randomMacSupport") {
        item.SetRandomMacSupport(std::stoi(value) != 0);
    } else if (key == "autoListen") {
        item.SetIsAutoListen(std::stoi(value) != 0);
    } else if (key == "deviceName") {
        item.SetDeviceName(value);
    } else if (key == "primaryDeviceType") {
        item.SetPrimaryDeviceType(value);
    } else if (key == "secondaryDeviceType") {
        item.SetSecondaryDeviceType(value);
    }
    return;
}

template<>
std::string GetTClassName<P2pVendorConfig>()
{
    return "P2pVendorConfig";
}

template<>
std::string OutTClassString<P2pVendorConfig>(P2pVendorConfig &item)
{
    std::ostringstream ss;
    ss << "randomMacSupport=" << item.GetRandomMacSupport() << std::endl;
    ss << "autoListen=" << item.GetIsAutoListen() << std::endl;
    ss << "deviceName=" << item.GetDeviceName() << std::endl;
    ss << "primaryDeviceType=" << item.GetPrimaryDeviceType() << std::endl;
    ss << "secondaryDeviceType=" << item.GetSecondaryDeviceType() << std::endl;
    return ss.str();
}

template<>
void ClearTClass<StationInfo>(StationInfo &item)
{
    item.deviceName.clear();
    item.bssid.clear();
    item.ipAddr.clear();
    return;
}

template<>
void SetTClassKeyValue<StationInfo>(StationInfo &item, const std::string &key, const std::string &value)
{
    if (key == "deviceName") {
        item.deviceName = value;
    } else if (key == "bssid") {
        item.bssid = value;
    } else if (key == "ipAddr") {
        item.ipAddr = value;
    }
    return;
}

template<>
std::string GetTClassName<StationInfo>()
{
    return "StationInfo";
}

template<>
std::string OutTClassString<StationInfo>(StationInfo &item)
{
    std::ostringstream ss;
    ss << "deviceName=" << item.deviceName << std::endl;
    ss << "bssid=" << item.bssid << std::endl;
    ss << "ipAddr=" << item.ipAddr << std::endl;
    return ss.str();
}

template<>
void ClearTClass<WifiConfig>(WifiConfig &item)
{
    item.scanAlwaysSwitch = false;
    item.staAirplaneMode = false;
    item.canOpenStaWhenAirplane = false;
    item.staLastState = false;
    item.savedDeviceAppraisalPriority = PRIORITY_1;
    item.scoretacticsScoreSlope = SCORE_SLOPE;
    item.scoretacticsInitScore = INIT_SCORE;
    item.scoretacticsSameBssidScore = SAME_BSSID_SCORE;
    item.scoretacticsSameNetworkScore = SAME_NETWORK_SCORE;
    item.scoretacticsFrequency5GHzScore = FREQUENCY_5_GHZ_SCORE;
    item.scoretacticsLastSelectionScore = LAST_SELECTION_SCORE;
    item.scoretacticsSecurityScore = SECURITY_SCORE;
    item.whetherToAllowNetworkSwitchover = true;
    item.dhcpIpType = static_cast<int>(DhcpIpType::DHCP_IPTYPE_MIX);
    item.defaultWifiInterface = "wlan0";
    item.preLoadSta = false;
    item.preLoadScan = false;
    item.preLoadAp = false;
    item.preLoadP2p = false;
    item.preLoadAware = false;
    item.supportHwPnoFlag = true;
    item.minRssi2Dot4Ghz = MIN_RSSI_24GHZ;
    item.minRssi5Ghz = MIN_RSSI_5GHZ;
    item.firstRssiLevel2G = RSSI_LEVEL_1_2G;
    item.secondRssiLevel2G = RSSI_LEVEL_2_2G;
    item.thirdRssiLevel2G = RSSI_LEVEL_3_2G;
    item.fourthRssiLevel2G = RSSI_LEVEL_4_2G;
    item.firstRssiLevel5G = RSSI_LEVEL_1_5G;
    item.secondRssiLevel5G = RSSI_LEVEL_2_5G;
    item.thirdRssiLevel5G = RSSI_LEVEL_3_5G;
    item.fourthRssiLevel5G = RSSI_LEVEL_4_5G;
    item.strDnsBak = "8.8.8.8";
    item.isLoadStabak = true;
    return;
}

static int SetWifiConfigValueFirst(WifiConfig &item, const std::string &key, const std::string &value)
{
    if (key == "scanAlwaysSwitch") {
        item.scanAlwaysSwitch = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "staAirplaneMode") {
        item.staAirplaneMode = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "canOpenStaWhenAirplane") {
        item.canOpenStaWhenAirplane = (std::stoi(value) != 0);
    } else if (key == "staLastState") {
        item.staLastState = (std::stoi(value) != 0);
    } else if (key == "savedDeviceAppraisalPriority") {
        item.savedDeviceAppraisalPriority = std::stoi(value);
    } else if (key == "scoretacticsScoreSlope") {
        item.scoretacticsScoreSlope = std::stoi(value);
    } else if (key == "scoretacticsInitScore") {
        item.scoretacticsInitScore = std::stoi(value);
    } else if (key == "scoretacticsSameBssidScore") {
        item.scoretacticsSameBssidScore = std::stoi(value);
    } else if (key == "scoretacticsSameNetworkScore") {
        item.scoretacticsSameNetworkScore = std::stoi(value);
    } else if (key == "scoretacticsFrequency5GHzScore") {
        item.scoretacticsFrequency5GHzScore = std::stoi(value);
    } else if (key == "scoretacticsLastSelectionScore") {
        item.scoretacticsLastSelectionScore = std::stoi(value);
    } else if (key == "scoretacticsSecurityScore") {
        item.scoretacticsSecurityScore = std::stoi(value);
    } else if (key == "whetherToAllowNetworkSwitchover") {
        item.whetherToAllowNetworkSwitchover = (std::stoi(value) != 0);
    } else if (key == "dhcpIpType") {
        item.dhcpIpType = std::stoi(value);
    } else if (key == "defaultWifiInterface") {
        item.defaultWifiInterface = value;
    } else {
        return -1;
    }
    return 0;
}

template<>
void SetTClassKeyValue<WifiConfig>(WifiConfig &item, const std::string &key, const std::string &value)
{
    if (SetWifiConfigValueFirst(item, key, value) == 0) {
        return;
    }
    if (key == "preLoadSta") {
        item.preLoadSta = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "preLoadScan") {
        item.preLoadScan = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "preLoadAp") {
        item.preLoadAp = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "preLoadP2p") {
        item.preLoadP2p = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "preLoadAware") {
        item.preLoadAware = (std::stoi(value) != 0); /* 0 -> false 1 -> true */
    } else if (key == "supportHwPnoFlag") {
        item.supportHwPnoFlag = std::stoi(value);
    } else if (key == "minRssi2Dot4Ghz") {
        item.minRssi2Dot4Ghz = std::stoi(value);
    } else if (key == "minRssi5Ghz") {
        item.minRssi5Ghz = std::stoi(value);
    } else if (key == "firstRssiLevel2G") {
        item.firstRssiLevel2G = std::stoi(value);
    } else if (key == "secondRssiLevel2G") {
        item.secondRssiLevel2G = std::stoi(value);
    } else if (key == "thirdRssiLevel2G") {
        item.thirdRssiLevel2G = std::stoi(value);
    } else if (key == "fourthRssiLevel2G") {
        item.fourthRssiLevel2G = std::stoi(value);
    } else if (key == "firstRssiLevel5G") {
        item.firstRssiLevel5G = std::stoi(value);
    } else if (key == "secondRssiLevel5G") {
        item.secondRssiLevel5G = std::stoi(value);
    } else if (key == "thirdRssiLevel5G") {
        item.thirdRssiLevel5G = std::stoi(value);
    } else if (key == "fourthRssiLevel5G") {
        item.fourthRssiLevel5G = std::stoi(value);
    } else if (key == "strDnsBak") {
        item.strDnsBak = value;
    } else if (key == "isLoadStabak") {
        item.isLoadStabak = (std::stoi(value) != 0);
    }
    return;
}

template<>
std::string GetTClassName<WifiConfig>()
{
    return "WifiConfig";
}

template<>
std::string OutTClassString<WifiConfig>(WifiConfig &item)
{
    std::ostringstream ss;
    ss << "scanAlwaysSwitch=" << item.scanAlwaysSwitch << std::endl; /* bool false->0 true->1 */
    ss << "staAirplaneMode=" << item.staAirplaneMode << std::endl;
    ss << "canOpenStaWhenAirplane=" << item.canOpenStaWhenAirplane << std::endl;
    ss << "staLastState=" << item.staLastState << std::endl;
    ss << "savedDeviceAppraisalPriority=" << item.savedDeviceAppraisalPriority << std::endl;
    ss << "scoretacticsScoreSlope=" << item.scoretacticsScoreSlope << std::endl;
    ss << "scoretacticsInitScore=" << item.scoretacticsInitScore << std::endl;
    ss << "scoretacticsSameBssidScore=" << item.scoretacticsSameBssidScore << std::endl;
    ss << "scoretacticsSameNetworkScore=" << item.scoretacticsSameNetworkScore << std::endl;
    ss << "scoretacticsFrequency5GHzScore=" << item.scoretacticsFrequency5GHzScore << std::endl;
    ss << "scoretacticsLastSelectionScore=" << item.scoretacticsLastSelectionScore << std::endl;
    ss << "scoretacticsSecurityScore=" << item.scoretacticsSecurityScore << std::endl;
    ss << "whetherToAllowNetworkSwitchover=" << item.whetherToAllowNetworkSwitchover << std::endl;
    ss << "dhcpIpType=" << item.dhcpIpType << std::endl;
    ss << "defaultWifiInterface=" << item.defaultWifiInterface << std::endl;
    ss << "preLoadSta=" << item.preLoadSta << std::endl;
    ss << "preLoadScan=" << item.preLoadScan << std::endl;
    ss << "preLoadAp=" << item.preLoadAp << std::endl;
    ss << "preLoadP2p=" << item.preLoadP2p << std::endl;
    ss << "preLoadAware=" << item.preLoadAware << std::endl;
    ss << "supportHwPnoFlag=" << item.supportHwPnoFlag << std::endl;
    ss << "minRssi2Dot4Ghz=" << item.minRssi2Dot4Ghz << std::endl;
    ss << "minRssi5Ghz=" << item.minRssi5Ghz << std::endl;
    ss << "firstRssiLevel2G=" << item.firstRssiLevel2G << std::endl;
    ss << "secondRssiLevel2G=" << item.secondRssiLevel2G << std::endl;
    ss << "thirdRssiLevel2G=" << item.thirdRssiLevel2G << std::endl;
    ss << "fourthRssiLevel2G=" << item.fourthRssiLevel2G << std::endl;
    ss << "firstRssiLevel5G=" << item.firstRssiLevel5G << std::endl;
    ss << "secondRssiLevel5G=" << item.secondRssiLevel5G << std::endl;
    ss << "thirdRssiLevel5G=" << item.thirdRssiLevel5G << std::endl;
    ss << "fourthRssiLevel5G=" << item.fourthRssiLevel5G << std::endl;
    ss << "strDnsBak=" << item.strDnsBak << std::endl;
    ss << "isLoadStabak=" << item.isLoadStabak << std::endl;
    return ss.str();
}

template<>
void ClearTClass<WifiP2pGroupInfo>(WifiP2pGroupInfo &item)
{
    item.SetIsGroupOwner(false);
    WifiP2pDevice device;
    item.SetOwner(device);
    item.SetPassphrase("");
    item.SetInterface("");
    item.SetGroupName("");
    item.SetFrequency(0);
    item.SetIsPersistent(false);
    item.SetP2pGroupStatus(static_cast<P2pGroupStatus>(0));
    item.SetNetworkId(0);
    item.SetGoIpAddress("");
    item.ClearClientDevices();
}

static void SetWifiP2pDevicClassKeyValue(WifiP2pDevice &item, const std::string &key, const std::string &value)
{
    if (key == "deviceName") {
        item.SetDeviceName(value);
    } else if (key == "deviceAddress") {
        item.SetDeviceAddress(value);
    } else if (key == "primaryDeviceType") {
        item.SetPrimaryDeviceType(value);
    } else if (key == "status") {
        item.SetP2pDeviceStatus(static_cast<P2pDeviceStatus>(std::stoi(value)));
    } else if (key == "supportWpsConfigMethods") {
        item.SetWpsConfigMethod(std::stoi(value));
    } else if (key == "deviceCapabilitys") {
        item.SetDeviceCapabilitys(std::stoi(value));
    } else if (key == "groupCapabilitys") {
        item.SetGroupCapabilitys(std::stoi(value));
    }
}

template<>
void SetTClassKeyValue<WifiP2pGroupInfo>(WifiP2pGroupInfo &item, const std::string &key, const std::string &value)
{
    if (key == "isGroupOwner") {
        item.SetIsGroupOwner(std::stoi(value) != 0);
    } else if (key == "passphrase") {
        item.SetPassphrase(value);
    } else if (key == "interface") {
        item.SetInterface(value);
    } else if (key == "groupName") {
        item.SetGroupName(value);
    } else if (key == "networkId") {
        item.SetNetworkId(std::stoi(value));
    } else if (key == "frequency") {
        item.SetFrequency(std::stoi(value));
    } else if (key == "isPersistent") {
        item.SetIsPersistent(std::stoi(value) != 0);
    } else if (key == "groupStatus") {
        item.SetP2pGroupStatus(static_cast<P2pGroupStatus>(std::stoi(value)));
    } else if (key == "goIpAddress") {
        item.SetGoIpAddress(value);
    } else if (key.compare(0, strlen("ownerDev."), "ownerDev.") == 0) {
        WifiP2pDevice owner = item.GetOwner();
        SetWifiP2pDevicClassKeyValue(owner, key.substr(strlen("ownerDev.")), value);
        item.SetOwner(owner);
    } else if (key.compare(0, strlen("vecDev_"), "vecDev_") == 0) {
        std::string::size_type pos = key.find(".");
        if (pos == std::string::npos) {
            WifiP2pDevice device;
            item.AddClientDevice(device);
        } else {
            unsigned long index = std::stoi(key.substr(strlen("vecDev_"), pos));
            if (index < item.GetClientDevices().size()) {
                std::vector<WifiP2pDevice> clients = item.GetClientDevices();
                SetWifiP2pDevicClassKeyValue(clients[index], key.substr(pos + 1), value);
                item.SetClientDevices(clients);
            }
        }
    }
}

template<>
std::string GetTClassName<WifiP2pGroupInfo>()
{
    return "WifiP2pGroupInfo";
}

static std::string OutWifiP2pDevicClassString(const WifiP2pDevice &item, std::string prefix = "")
{
    std::ostringstream ss;

    ss << prefix << "deviceName=" << item.GetDeviceName() << std::endl;
    ss << prefix << "deviceAddress=" << item.GetDeviceAddress() << std::endl;
    ss << prefix << "primaryDeviceType=" << item.GetPrimaryDeviceType() << std::endl;
    ss << prefix << "status=" << static_cast<int>(item.GetP2pDeviceStatus()) << std::endl;
    ss << prefix << "supportWpsConfigMethods=" << item.GetWpsConfigMethod() << std::endl;
    ss << prefix << "deviceCapabilitys=" << item.GetDeviceCapabilitys() << std::endl;
    ss << prefix << "groupCapabilitys=" << item.GetGroupCapabilitys() << std::endl;

    return ss.str();
}

template<>
std::string OutTClassString<WifiP2pGroupInfo>(WifiP2pGroupInfo &item)
{
    std::ostringstream ss;
    ss << "isGroupOwner=" << item.IsGroupOwner() << std::endl;
    ss << "passphrase=" << item.GetPassphrase() << std::endl;
    ss << "interface=" << item.GetInterface() << std::endl;
    ss << "groupName=" << item.GetGroupName() << std::endl;
    ss << "networkId=" << item.GetNetworkId() << std::endl;
    ss << "frequency=" << item.GetFrequency() << std::endl;
    ss << "isPersistent=" << item.IsPersistent() << std::endl;
    ss << "groupStatus=" << static_cast<int>(item.GetP2pGroupStatus()) << std::endl;
    ss << "goIpAddress=" << item.GetGoIpAddress() << std::endl;
    ss << OutWifiP2pDevicClassString(item.GetOwner(), "ownerDev.");
    int size = item.GetClientDevices().size();
    for (int i = 0; i < size; i++) {
        std::string prefix = "vecDev_" + std::to_string(i) + ".";
        ss << "vecDev_=" << i << std::endl;
        const WifiP2pDevice &tmp = item.GetClientDevices().at(i);
        ss << OutWifiP2pDevicClassString(tmp, prefix);
    }
    return ss.str();
}

template <>
void ClearTClass<TrustListPolicy>(TrustListPolicy &item)
{
    item.sceneId = 0;
    item.sceneName.clear();
    item.trustList.clear();
    return;
}

template <>
void SetTClassKeyValue<TrustListPolicy>(TrustListPolicy &item, const std::string &key, const std::string &value)
{
    if (key == "sceneId") {
        item.sceneId = std::stoi(value);
    } else if (key == "sceneName") {
        item.sceneName = value;
    } else if (key == "trustList") {
        item.trustList = value;
    }
    return;
}

template <>
std::string GetTClassName<TrustListPolicy>()
{
    return "TrustListPolicy";
}

template <> std::string OutTClassString<TrustListPolicy>(TrustListPolicy &item)
{
    std::ostringstream ss;
    ss << "sceneId=" << item.sceneId << std::endl;
    ss << "sceneName=" << item.sceneName << std::endl;
    ss << "trustList=" << item.trustList << std::endl;
    return ss.str();
}

template <> void ClearTClass<MovingFreezePolicy>(MovingFreezePolicy &item)
{
    item.trustList.clear();
    return;
}

template <>
void SetTClassKeyValue<MovingFreezePolicy>(MovingFreezePolicy &item, const std::string &key, const std::string &value)
{
    if (key == "trustList") {
        item.trustList = value;
    }

    return;
}

template <> std::string GetTClassName<MovingFreezePolicy>()
{
    return "MovingFreezePolicy";
}

template <> std::string OutTClassString<MovingFreezePolicy>(MovingFreezePolicy &item)
{
    std::ostringstream ss;
    ss << "trustList=" << item.trustList << std::endl;
    return ss.str();
}

template <> void ClearTClass<WifiStoreRandomMac>(WifiStoreRandomMac &item)
{
    item.ssid.clear();
    item.keyMgmt.clear();
    item.peerBssid.clear();
    item.randomMac.clear();
    return;
}

template <>
void SetTClassKeyValue<WifiStoreRandomMac>(WifiStoreRandomMac &item, const std::string &key, const std::string &value)
{
    if (key == "ssid") {
        item.ssid = value;
    } else if (key == "keyMgmt") {
        item.keyMgmt = value;
    } else if (key == "peerBssid") {
        item.peerBssid = value;
    } else if (key == "randomMac") {
        item.randomMac = value;
    }
    return;
}

template <> std::string GetTClassName<WifiStoreRandomMac>()
{
    return "WifiStoreRandomMac";
}

template <> std::string OutTClassString<WifiStoreRandomMac>(WifiStoreRandomMac &item)
{
    std::ostringstream ss;
    ss << "ssid=" << item.ssid << std::endl;
    ss << "keyMgmt=" << item.keyMgmt << std::endl;
    ss << "peerBssid=" << item.peerBssid << std::endl;
    ss << "randomMac=" << item.randomMac << std::endl;
    return ss.str();
}
} // namespace Wifi
} // namespace OHOS

