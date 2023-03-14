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

#ifndef OHOS_WIFI_HAL_STRUCT_H
#define OHOS_WIFI_HAL_STRUCT_H

#include "wifi_hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct WifiChip {
    int chip;
} WifiChip;

typedef struct WifiIface {
    int index;
    int type;
    char name[WIFI_IFACE_NAME_MAXLEN];
    char macAddr[WIFI_MAC_LENGTH + 1];
} WifiIface;

typedef enum ScanStyle {
    SCAN_TYPE_LOW_SPAN = 0,
    SCAN_TYPE_LOW_POWER = 1,
    SCAN_TYPE_HIGH_ACCURACY = 2,
    SCAN_TYPE_INVALID = 0XFF
} ScanStyle;

typedef struct ScanSettings {
    int freqSize;
    int *freqs;
    int hiddenSsidSize;
    char **hiddenSsid;
    ScanStyle scanStyle;
} ScanSettings;

typedef struct ScanInfoElem {
    unsigned int id;
    char *content;
    int size;
} ScanInfoElem;

typedef enum AntEnum {
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
} Ant;

typedef struct ScanInfo {
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_BSSID_LENGTH];
    int freq;
    int channelWidth;
    int centerFrequency0;
    int centerFrequency1;
    ScanInfoElem *infoElems;
    int ieSize;
    int64_t features;
    int siglv;
    char flags[WIFI_SCAN_INFO_CAPABILITY_LENGTH];
    int64_t timestamp;
    Ant ant;
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

typedef struct SetNetworkConfig {
    DeviceConfigType cfgParam;                       /* Setting parameters. */
    char cfgValue[WIFI_NETWORK_CONFIG_VALUE_LENGTH];  /* Parameter value. */
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
    int signal; /* RSSI */
    int txrate;
    int rxrate;
    int noise;
    int frequency;
} WpaSignalInfo;
#ifdef __cplusplus
}
#endif
#endif