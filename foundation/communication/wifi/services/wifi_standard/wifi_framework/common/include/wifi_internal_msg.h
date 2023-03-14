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

#ifndef OHOS_WIFI_INTERNAL_MSG_H
#define OHOS_WIFI_INTERNAL_MSG_H

#include "wifi_msg.h"
#include "wifi_scan_msg.h"
#include "wifi_p2p_msg.h"
#include "wifi_ap_msg.h"

namespace OHOS {
namespace Wifi {
constexpr int REOPEN_STA_INTERVAL = 500; /* when reopen sta, need over last close sta time then interval */
constexpr int DEVICE_CONFIG_INDEX_SSID = 0;
constexpr int DEVICE_CONFIG_INDEX_BSSID = 1;
constexpr int PRIORITY_1 = 1;
constexpr int PRIORITY_2 = 2;
constexpr int PRIORITY_3 = 3;
constexpr int SCORE_SLOPE = 5;
constexpr int INIT_SCORE = 10;
constexpr int SAME_BSSID_SCORE = 8;
constexpr int SAME_NETWORK_SCORE = 5;
constexpr int FREQUENCY_5_GHZ_SCORE = 10;
constexpr int LAST_SELECTION_SCORE = 120;
constexpr int SECURITY_SCORE = 20;
constexpr int MIN_RSSI_24GHZ = -80;
constexpr int MIN_RSSI_5GHZ = -77;
constexpr int RSSI_LEVEL_1_2G = -88;
constexpr int RSSI_LEVEL_2_2G = -82;
constexpr int RSSI_LEVEL_3_2G = -75;
constexpr int RSSI_LEVEL_4_2G = -65;
constexpr int RSSI_LEVEL_1_5G = -85;
constexpr int RSSI_LEVEL_2_5G = -79;
constexpr int RSSI_LEVEL_3_5G = -72;
constexpr int RSSI_LEVEL_4_5G = -65;

enum class WifiOprMidState { CLOSED = 0, OPENING = 1, RUNNING = 2, CLOSING = 3, UNKNOWN };

enum class WifiFeatures {
    WIFI_FEATURE_INFRA = 0x0001,             // The feature id indicates support basic infrastructure mode
    WIFI_FEATURE_INFRA_5G = 0x0002,          // The feature id indicates support 5 GHz Band
    WIFI_FEATURE_PASSPOINT = 0x0004,         // The feature id indicates support GAS/ANQP
    WIFI_FEATURE_P2P = 0x0008,               // The feature id indicates support Wifi-Direct
    WIFI_FEATURE_MOBILE_HOTSPOT = 0x0010,    // The feature id indicates support Soft AP
    WIFI_FEATURE_AWARE = 0x0040,             // The feature id indicates support Wi-Fi AWare networking
    WIFI_FEATURE_AP_STA = 0x8000,            // The feature id indicates support AP STA Concurrency
    WIFI_FEATURE_WPA3_SAE = 0x8000000,       // The feature id indicates support WPA3-Personal SAE
    WIFI_FEATURE_WPA3_SUITE_B = 0x10000000,  // The feature id indicates support WPA3-Enterprise Suite-B
    WIFI_FEATURE_OWE = 0x20000000,           // The feature id indicates support Enhanced Open
};

enum class OperateResState {
    OPEN_WIFI_OPENING = 0,             /* open wifi opening */
    OPEN_WIFI_SUCCEED,                 /* open wifi succeed */
    OPEN_WIFI_FAILED,                  /* open wifi failed */
    OPEN_WIFI_OVERRIDE_OPEN_FAILED,    /* enable wifi repeatedly */
    OPEN_WIFI_DISABLED,                /* open wifi failed, set wifi disabled */
    OPEN_WIFI_SUPPLICANT_INIT_FAILED,  /* wpa_supplicant not inited or init failed */
    OPEN_WIFI_OPEN_SUPPLICANT_FAILED,  /* wpa_supplicant start failed */
    OPEN_WIFI_CONN_SUPPLICANT_FAILED,  /* connect wpa_supplicant failed */
    CLOSE_WIFI_CLOSING,                /* close wifi closing */
    CLOSE_WIFI_SUCCEED,                /* close wifi succeed */
    CLOSE_WIFI_FAILED,                 /* close wifi failed */
    CONNECT_CONNECTING,                /* connecting */
    CONNECT_CONNECTING_TIMEOUT,        /* connecting time out */
    CONNECT_TO_OWN_AP_FAILED,          /* connect own ap failed */
    CONNECT_ENABLE_NETWORK_FAILED,     /* wpa_supplicant enable network failed */
    CONNECT_SELECT_NETWORK_FAILED,     /* wpa_supplicant select network failed */
    CONNECT_SAVE_DEVICE_CONFIG_FAILED, /* wpa_supplicant save network config failed */
    CONNECT_AP_CONNECTED,              /* connect succeed */
    CONNECT_CHECK_PORTAL,              /* check connect to a portal hotspot */
    CONNECT_NETWORK_ENABLED,           /* can visit internet */
    CONNECT_NETWORK_DISABLED,          /* cannot visit internet */
    DISCONNECT_DISCONNECTING,          /* disconnecting */
    DISCONNECT_DISCONNECT_FAILED,      /* disconnect failed */
    DISCONNECT_DISCONNECTED,           /* disconnect succeed */
    CONNECT_PASSWORD_WRONG,            /* wrong password */
    CONNECT_CONNECTION_FULL,           /* connection full */
    CONNECT_CONNECTION_REJECT,         /* connection reject */
    CONNECT_OBTAINING_IP,              /* obtain ip */
    CONNECT_OBTAINING_IP_FAILED,       /* obtain ip FAILED */
    CONNECT_ASSOCIATING,
    CONNECT_ASSOCIATED,
};

/* is wps connected to a network */
enum class IsWpsConnected {
    WPS_CONNECTED = 0,
    WPS_INVALID = -1,
};

enum class Ant {
    NETWORK_PRIVATE = 0,
    NETWORK_PRIVATEWITHGUEST = 1,
    NETWORK_CHARGEABLEPUBLIC = 2,
    NETWORK_FREEPUBLIC = 3,
    NETWORK_PERSONAL = 4,
    NETWORK_EMERGENCYONLY = 5,
    NETWORK_RESVD6 = 6,
    NETWORK_RESVD7 = 7,
    NETWORK_RESVD8 = 8,
    NETWORK_RESVD9 = 9,
    NETWORK_RESVD10 = 10,
    NETWORK_RESVD11 = 11,
    NETWORK_RESVD12 = 12,
    NETWORK_RESVD13 = 13,
    NETWORK_TESTOREXPERIMENTAL = 14,
    NETWORK_WILDCARD = 15,
    NETWORK_ANT_INVALID = 16
};

struct InterScanInfo {
    std::string bssid;
    std::string ssid;
    /**
     * Network performance, including authentication,
     * key management, and encryption mechanisms
     * supported by the access point
     */
    std::string capabilities;
    int frequency;
    int band;  /* ap band, 1: 2.4G, 2: 5G */
    WifiChannelWidth channelWidth;
    int centerFrequency0;
    int centerFrequency1;
    int rssi; /* signal level */
    WifiSecurity securityType;
    std::vector<WifiInfoElem> infoElems;
    int64_t features;
    int64_t timestamp;
    Ant ant;

    InterScanInfo()
        : frequency(0),
          band(0),
          channelWidth(WifiChannelWidth::WIDTH_INVALID),
          centerFrequency0(0),
          centerFrequency1(0),
          rssi(0),
          securityType(WifiSecurity::INVALID),
          features(0),
          timestamp(0),
          ant(Ant::NETWORK_ANT_INVALID)
    {}

    ~InterScanInfo()
    {}

    void GetDeviceMgmt(std::string &mgmt)
    {
        switch (securityType) {
            case WifiSecurity::PSK:
                mgmt = "WPA-PSK";
                break;
            case WifiSecurity::EAP:
                mgmt = "WPA-EAP";
                break;
            case WifiSecurity::SAE:
                mgmt = "SAE";
                break;
            case WifiSecurity::OWE:
                mgmt = "OWE";
                break;
            default:
                mgmt = "NONE";
                break;
        }
    }
};


struct SingleAppForbid {
    int appID;
    ScanIntervalMode scanIntervalMode;
    int lessThanIntervalCount;
    time_t continueScanTime;
    time_t blockListScanTime;
    int expScanCount;
    int fixedScanCount;
    time_t fixedCurrentTime;
    SingleAppForbid()
    {
        appID = 0;
        lessThanIntervalCount = 0;
        continueScanTime = 0;
        blockListScanTime = 0;
        expScanCount = 0;
        fixedScanCount = 0;
        fixedCurrentTime = 0;
    }
};

struct WifiEventCallbackMsg {
    int msgCode;
    int msgData;
    std::string pinCode; /* wps pin mode code */
    WifiLinkedInfo linkInfo;
    StationInfo staInfo;
    std::vector<WifiP2pDevice> device;
    std::vector<WifiP2pServiceInfo> serviceInfo;
    WifiP2pLinkedInfo p2pInfo;
    WifiP2pDevice p2pDevice;
    P2pActionCallback p2pAction;
    WifiEventCallbackMsg()
    {
        msgCode = 0;
        msgData = 0;
        p2pAction = P2pActionCallback::UNKNOWN;
    }
};

enum class DhcpIpType { /* dhcp IP type: ipv4 ipv6 mix */
    DHCP_IPTYPE_IPV4,
    DHCP_IPTYPE_IPV6,
    DHCP_IPTYPE_MIX,
};

/* wifi config store */
struct WifiConfig {
    bool scanAlwaysSwitch; /* scan always switch */
    bool staAirplaneMode; /* when open airplane mode, whether close sta */
    bool canOpenStaWhenAirplane; /* if airplane is opened, whether can open sta */
    /**
     * last sta service state, when service started, power
     * saving off, airplane mode off we use this saved state to
     * discuss whether need restore sta service. when open sta
     * service, set true; when user call DisableWifi succeed,
     * set false;
     */
    bool staLastState;
    int savedDeviceAppraisalPriority;
    int scoretacticsScoreSlope;
    int scoretacticsInitScore;
    int scoretacticsSameBssidScore;
    int scoretacticsSameNetworkScore;
    int scoretacticsFrequency5GHzScore;
    int scoretacticsLastSelectionScore;
    int scoretacticsSecurityScore;
    bool whetherToAllowNetworkSwitchover;
    int dhcpIpType;
    std::string defaultWifiInterface;
    bool preLoadSta;
    bool preLoadScan;
    bool preLoadAp;
    bool preLoadP2p;
    bool preLoadAware;
    bool supportHwPnoFlag;
    int minRssi2Dot4Ghz;
    int minRssi5Ghz;
    int firstRssiLevel2G;
    int secondRssiLevel2G;
    int thirdRssiLevel2G;
    int fourthRssiLevel2G;
    int firstRssiLevel5G;
    int secondRssiLevel5G;
    int thirdRssiLevel5G;
    int fourthRssiLevel5G;
    std::string strDnsBak;
    bool isLoadStabak;

    WifiConfig()
    {
        scanAlwaysSwitch = false;
        staAirplaneMode = false;
        canOpenStaWhenAirplane = false;
        staLastState = false;
        savedDeviceAppraisalPriority = PRIORITY_1;
        scoretacticsScoreSlope = SCORE_SLOPE;
        scoretacticsInitScore = INIT_SCORE;
        scoretacticsSameBssidScore = SAME_BSSID_SCORE;
        scoretacticsSameNetworkScore = SAME_NETWORK_SCORE;
        scoretacticsFrequency5GHzScore = FREQUENCY_5_GHZ_SCORE;
        scoretacticsLastSelectionScore = LAST_SELECTION_SCORE;
        scoretacticsSecurityScore = SECURITY_SCORE;
        whetherToAllowNetworkSwitchover = true;
        dhcpIpType = static_cast<int>(DhcpIpType::DHCP_IPTYPE_MIX);
        defaultWifiInterface = "wlan0";
        preLoadSta = false;
        preLoadScan = false;
        preLoadAp = false;
        preLoadP2p = false;
        preLoadAware = false;
        supportHwPnoFlag = true;
        minRssi2Dot4Ghz = MIN_RSSI_24GHZ;
        minRssi5Ghz = MIN_RSSI_5GHZ;
        firstRssiLevel2G = RSSI_LEVEL_1_2G;
        secondRssiLevel2G = RSSI_LEVEL_2_2G;
        thirdRssiLevel2G = RSSI_LEVEL_3_2G;
        fourthRssiLevel2G = RSSI_LEVEL_4_2G;
        firstRssiLevel5G = RSSI_LEVEL_1_5G;
        secondRssiLevel5G = RSSI_LEVEL_2_5G;
        thirdRssiLevel5G = RSSI_LEVEL_3_5G;
        fourthRssiLevel5G = RSSI_LEVEL_4_5G;
        strDnsBak = "8.8.8.8";
        isLoadStabak = true;
    }
};

struct TrustListPolicy {
    int sceneId = 0;        /* scene id */
    std::string sceneName;  /* scene name, just to read. */
    std::string trustList;  /* trust list, eg: for A,B,and C,the format is A|B|C */
};

struct MovingFreezePolicy {
    std::string trustList;  /* trust list */
};

/* wifi RandomMac store */
struct WifiStoreRandomMac {
    std::string ssid;
    std::string keyMgmt;
    std::string peerBssid;
    std::string randomMac;
};
}  // namespace Wifi
}  // namespace OHOS
#endif
