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
#ifndef OHOS_IDL_IWIFI_STRUCT_H
#define OHOS_IDL_IWIFI_STRUCT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#define WIFI_SSID_LENGTH 132
#define WIFI_BSSID_LENGTH 18
#define WIFI_NETWORK_FLAGS_LENGTH 64
#define WIFI_SCAN_INFO_CAPABILITIES_LENGTH 256
#define WIFI_NETWORK_CONFIG_NAME_LENGTH 64
#define WIFI_NETWORK_CONFIG_VALUE_LENGTH 256
#define WIFI_MAC_ADDR_LENGTH 17
#define WIFI_AP_PASSWORD_LENGTH 64
#define WIFI_INTERFACE_NAME_SIZE 32
#define WIFI_PIN_CODE_LENGTH 8

/* IWifiIface */
/*
 * This is a temporary definition. The empty structure compilation alarm needs
 * to be deleted, affecting other alarms.
 */
typedef struct TagIWifiIface {
    int index;
    int type;
    char name[WIFI_INTERFACE_NAME_SIZE];
    char macAddr[WIFI_MAC_ADDR_LENGTH + 1];
} IWifiIface;

/* IWifiClientIface */
/*
 * This is a temporary definition. The empty structure compilation alarm needs
 * to be deleted, affecting other alarms.
 */
typedef struct TagIWifiClientIface {
    char name[WIFI_INTERFACE_NAME_SIZE];
    int type;
} IWifiClientIface;

/* IWifiApIface */
/*
 * This is a temporary definition. The empty structure compilation alarm needs
 * to be deleted, affecting other alarms.
 */
typedef struct TagIWifiApIface {
    char name[WIFI_INTERFACE_NAME_SIZE];
    int type;
} IWifiApIface;

/* IWifiChip */
/*
 * This is a temporary definition. The empty structure compilation alarm needs
 * to be deleted, affecting other alarms.
 */
typedef struct TagIWifiChip {
    int i;
} IWifiChip;

typedef struct StSupplicantEventCallback {
    void (*onScanNotify)(int32_t result);
} ISupplicantEventCallback;

typedef struct ScanSettings {
    int freqSize;
    int *freqs;
    int hiddenSsidSize;
    char **hiddenSsid;
    int scanStyle;
} ScanSettings;

typedef struct ScanInfoElem {
    unsigned int id;
    char* content;
    int size;
} ScanInfoElem;

typedef struct ScanInfo {
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_BSSID_LENGTH];
    int frequency;
    int channelWidth;
    int centerFrequency0;
    int centerFrequency1;
    ScanInfoElem* infoElems;
    int ieSize;
    int64_t features;
    int signalLevel;
    char capability[WIFI_SCAN_INFO_CAPABILITIES_LENGTH];
    int64_t timestamp;
    int associated;
    int antValue;
} ScanInfo;

typedef struct WifiNetworkInfo {
    int id;
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_BSSID_LENGTH];
    char flags[WIFI_NETWORK_FLAGS_LENGTH];
} WifiNetworkInfo;

typedef struct PnoScanSettings {
    int scanInterval;
    int minRssi2Dot4Ghz;
    int minRssi5Ghz;
    int freqSize;
    int *freqs;
    int hiddenSsidSize;
    char **hiddenSsid;
    int savedSsidSize;
    char **savedSsid;
} PnoScanSettings;

/* Wifi Network configuration parameter flag */
typedef enum DeviceConfigType {
    DEVICE_CONFIG_SSID = 0,            /* SSID */
    DEVICE_CONFIG_PSK = 1,             /* psk */
    DEVICE_CONFIG_KEYMGMT = 2,         /* key_mgmt，WPA-PSK，WPA-NONE，WPA-EAP */
    DEVICE_CONFIG_PRIORITY = 3,        /* wpaNetwork Priority */
    DEVICE_CONFIG_SCAN_SSID = 4,       /* Set this bit to 1 and deliver it when the hidden network is connected. */
    DEVICE_CONFIG_EAP = 5,             /* EPA mode:/EAP/PEAP */
    DEVICE_CONFIG_IDENTITY = 6,        /* Account name */
    DEVICE_CONFIG_PASSWORD = 7,        /* Account password */
    DEVICE_CONFIG_BSSID = 8,           /* bssid */
    DEVICE_CONFIG_AUTH_ALGORITHMS = 9, /* auth algorithms */
    DEVICE_CONFIG_WEP_KEY_IDX = 10,    /* wep key idx */
    DEVICE_CONFIG_WEP_KEY_0 = 11,
    DEVICE_CONFIG_WEP_KEY_1 = 12,
    DEVICE_CONFIG_WEP_KEY_2 = 13,
    DEVICE_CONFIG_WEP_KEY_3 = 14,
    DEVICE_CONFIG_END_POS, /* Number of network configuration parameters, which is used as the last parameter. */
} DeviceConfigType;

typedef struct SetNetworkConfig {
    DeviceConfigType cfgParam;                       /* param */
    char cfgValue[WIFI_NETWORK_CONFIG_VALUE_LENGTH]; /* param value */
} SetNetworkConfig;

typedef struct GetNetworkConfig {
    int networkId;
    char param[WIFI_NETWORK_CONFIG_NAME_LENGTH];
    char value[WIFI_NETWORK_CONFIG_VALUE_LENGTH];
} GetNetworkConfig;

typedef struct WifiWpsParam {
    int anyFlag;
    int multiAp;
    char bssid[WIFI_BSSID_LENGTH];
    char pinCode[WIFI_PIN_CODE_LENGTH + 1];
} WifiWpsParam;

typedef struct WifiRoamCapability {
    int maxBlocklistSize;
    int maxTrustlistSize;
} WifiRoamCapability;

typedef struct WpaSignalInfo {
    int signal;
    int txrate;
    int rxrate;
    int noise;
    int frequency;
} WpaSignalInfo;

typedef struct HostapdConfig {
    char ssid[WIFI_SSID_LENGTH];
    int32_t ssidLen;
    char preSharedKey[WIFI_AP_PASSWORD_LENGTH];
    int32_t preSharedKeyLen;
    int32_t securityType;
    int32_t band;
    int32_t channel;
    int32_t maxConn;
} HostapdConfig;

typedef struct CStationInfo {
    int type;
    char mac[WIFI_MAC_ADDR_LENGTH + 1];
} CStationInfo;

typedef struct IWifiApEventCallback {
    void (*onStaJoinOrLeave)(const CStationInfo *info);
    void (*onApEnableOrDisable)(int event);
} IWifiApEventCallback;

typedef enum IfaceType { TYPE_STA, TYPE_AP, TYPE_P2P, TYPE_NAN } IfaceType;

/*----------------p2p struct defines begin-------------------------*/
#define WIFI_P2P_TMP_MSG_LENGTH_128 128
#define WIFI_P2P_TMP_MSG_LENGTH_64 64
#define WIFI_P2P_MAX_GROUP_IFACE_NAME_LENGTH 64
#define WIFI_P2P_DEVICE_TYPE_LENGTH 64
#define WIFI_P2P_DEVICE_NAME_LENGTH 128
#define WIFI_P2P_WFD_DEVICE_INFO_LENGTH 32
#define WIFI_P2P_IDL_SERVER_NAME_LENGTH 256
#define WIFI_P2P_IDL_SERVER_INFO_LENGTH 256
#define WIFI_P2P_GROUP_CONFIG_VALUE_LENGTH 256

typedef struct P2pDeviceInfo {
    char srcAddress[WIFI_MAC_ADDR_LENGTH + 1];
    char p2pDeviceAddress[WIFI_MAC_ADDR_LENGTH + 1];
    char primaryDeviceType[WIFI_P2P_DEVICE_TYPE_LENGTH];
    char deviceName[WIFI_P2P_DEVICE_NAME_LENGTH];
    int configMethods;
    int deviceCapabilities;
    int groupCapabilities;
    char wfdDeviceInfo[WIFI_P2P_WFD_DEVICE_INFO_LENGTH];
    unsigned int wfdLength;
} P2pDeviceInfo;

typedef struct P2pGroupInfo {
    int isGo;
    int isPersistent;
    int frequency;
    char groupName[WIFI_P2P_MAX_GROUP_IFACE_NAME_LENGTH];
    char ssid[WIFI_SSID_LENGTH];
    char psk[WIFI_P2P_TMP_MSG_LENGTH_128];
    char passphrase[WIFI_P2P_TMP_MSG_LENGTH_128];
    char goDeviceAddress[WIFI_MAC_ADDR_LENGTH + 1];
} P2pGroupInfo;

typedef struct P2pInvitationInfo {
    int type; /* 0:Recived, 1:Accepted */
    int persistentNetworkId;
    int operatingFrequency;
    char srcAddress[WIFI_MAC_ADDR_LENGTH + 1];
    char goDeviceAddress[WIFI_MAC_ADDR_LENGTH + 1];
    char bssid[WIFI_MAC_ADDR_LENGTH + 1];
} P2pInvitationInfo;

typedef struct P2pServDiscReqInfo {
    int freq;
    int dialogToken;
    int updateIndic;
    int tlvsLength;
    char mac[WIFI_MAC_ADDR_LENGTH + 1];
    unsigned char *tlvs;
} P2pServDiscReqInfo;

typedef struct P2pServiceInfo {
    int mode; /* 0/1, upnp/bonjour  */
    int version;
    char name[WIFI_P2P_IDL_SERVER_NAME_LENGTH];
    char query[WIFI_P2P_IDL_SERVER_INFO_LENGTH];
    char resp[WIFI_P2P_IDL_SERVER_INFO_LENGTH];
} P2pServiceInfo;

typedef struct P2pNetworkInfo {
    int id;
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_MAC_ADDR_LENGTH + 1];
    char flags[WIFI_P2P_TMP_MSG_LENGTH_64];
} P2pNetworkInfo;

typedef struct P2pNetworkList {
    int infoNum;
    P2pNetworkInfo *infos;
} P2pNetworkList;

typedef struct P2pConnectInfo {
    int persistent; /* |persistent=<network id>] */
    int mode; /* [join|auth] */
    int goIntent; /* [go_intent=<0..15>] */
    int provdisc; /* [provdisc] */
    char peerDevAddr[WIFI_MAC_ADDR_LENGTH + 1];
    char pin[WIFI_PIN_CODE_LENGTH + 1]; /* <pbc|pin|PIN#|p2ps> */
} P2pConnectInfo;

/* Wifi P2P Group Network configuration parameter flag */
typedef enum P2pGroupConfigType {
    GROUP_CONFIG_SSID = 0,
    GROUP_CONFIG_BSSID,
    GROUP_CONFIG_PSK,
    GROUP_CONFIG_PROTO,
    GROUP_CONFIG_KEY_MGMT,
    GROUP_CONFIG_PAIRWISE,
    GROUP_CONFIG_AUTH_ALG,
    GROUP_CONFIG_MODE,
    GROUP_CONFIG_DISABLED,
    GROUP_CONFIG_END_POS,
} P2pGroupConfigType;

typedef struct P2pGroupConfig {
    P2pGroupConfigType cfgParam;                       /* param */
    char cfgValue[WIFI_P2P_GROUP_CONFIG_VALUE_LENGTH]; /* param value */
} P2pGroupConfig;

typedef struct Hid2dConnectInfo {
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_MAC_ADDR_LENGTH + 1];
    char passphrase[WIFI_P2P_TMP_MSG_LENGTH_128];
    int frequency;
} Hid2dConnectInfo;
/* ----------------p2p struct defines end--------------------------- */

#ifdef __cplusplus
}
#endif
#endif