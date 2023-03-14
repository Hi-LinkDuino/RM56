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

#ifndef OHOS_WIFI_HAL_P2P_STRUCT_H
#define OHOS_WIFI_HAL_P2P_STRUCT_H

#include "wifi_hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum P2pSupplicantErrCode {
    P2P_SUP_ERRCODE_SUCCESS = 0,
    P2P_SUP_ERRCODE_FAILED = 1,
    P2P_SUP_ERRCODE_TIMEOUT = 2,
    P2P_SUP_ERRCODE_PBC_OVERLAP = 3,
    P2P_SUP_ERRCODE_UNKNOWN = 4,
    P2P_SUP_ERRCODE_INPUT_ERROR = 5,
    P2P_SUP_ERRCODE_INVALID = 0XFF,
} P2pSupplicantErrCode;

#define WIFI_P2P_DEVICE_TYPE_LENGTH 64
#define WIFI_P2P_DEVICE_NAME_LENGTH 128
#define WIFI_P2P_WFD_DEVICE_INFO_LENGTH 32
#define WIFI_P2P_PASSWORD_SIZE 128

typedef struct P2pDeviceInfo {
    short configMethods;
    int deviceCapabilities;
    int groupCapabilities;
    unsigned int wfdLength;
    char srcAddress[WIFI_BSSID_LENGTH];
    char p2pDeviceAddress[WIFI_BSSID_LENGTH];
    char primaryDeviceType[WIFI_P2P_DEVICE_TYPE_LENGTH];
    char deviceName[WIFI_P2P_DEVICE_NAME_LENGTH];
    char wfdDeviceInfo[WIFI_P2P_WFD_DEVICE_INFO_LENGTH];
} P2pDeviceInfo;

typedef struct P2pGroupInfo {
    int isGo;
    int isPersistent;
    int frequency;
    char groupIfName[WIFI_P2P_GROUP_IFNAME_LENGTH];
    char ssid[WIFI_SSID_LENGTH];
    char psk[WIFI_P2P_PASSWORD_SIZE];
    char passphrase[WIFI_P2P_PASSWORD_SIZE];
    char goDeviceAddress[WIFI_BSSID_LENGTH];
} P2pGroupInfo;

typedef struct P2pInvitationInfo {
    int type;
    int persistentNetworkId;
    int operatingFrequency;
    char srcAddress[WIFI_BSSID_LENGTH];
    char goDeviceAddress[WIFI_BSSID_LENGTH];
    char bssid[WIFI_BSSID_LENGTH];
} P2pInvitationInfo;

typedef struct P2pServDiscRespInfo {
    short updateIndicator;
    char srcAddress[WIFI_BSSID_LENGTH];
    char *tlvs;
} P2pServDiscRespInfo;

typedef struct P2pConnectInfo {
    int persistent;  // persistent=<network id>]
    int mode;        // [join|auth]
    int goIntent;    // [go_intent=<0..15>]
    int provdisc;    // [provdisc]
    char peerDevAddr[WIFI_BSSID_LENGTH];
    char pin[WIFI_PIN_CODE_LENGTH + 1];  // <pbc|pin|PIN#|p2ps>
} P2pConnectInfo;

typedef struct P2pNetworkInfo {
    int id;
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_BSSID_LENGTH];
    char flags[WIFI_NETWORK_FLAGS_LENGTH];
} P2pNetworkInfo;

typedef struct P2pNetworkList {
    int infoNum;
    P2pNetworkInfo *infos;
} P2pNetworkList;

typedef struct P2pServDiscReqInfo {
    int freq;
    int dialogToken;
    int updateIndic;
    char mac[WIFI_BSSID_LENGTH];
    char *tlvs;
} P2pServDiscReqInfo;

typedef struct P2pServiceInfo {
    int mode; /* 0/1, upnp/bonjour  */
    int version;
    char name[WIFI_P2P_SERVER_NAME_LENGTH];
    char query[WIFI_P2P_SERVE_INFO_LENGTH];
    char resp[WIFI_P2P_SERVE_INFO_LENGTH];
} P2pServiceInfo;

typedef struct P2pGroupConfig {
    P2pGroupConfigType cfgParam;                       /* param */
    char cfgValue[WIFI_P2P_GROUP_CONFIG_VALUE_LENGTH]; /* param value */
} P2pGroupConfig;

typedef struct P2pWpsPinDisplayArgv {
    int mode; /* 0/1 : PIN_KEYPAD/PIN_DISPLAY */
    char pinCode[WIFI_PIN_CODE_LENGTH + 1];
    char bssid[WIFI_BSSID_LENGTH];
} P2pWpsPinDisplayArgv;

typedef struct P2pHalInviteArgv {
    int persistent;
    char ifname[WIFI_P2P_GROUP_IFNAME_LENGTH];
    char peerbssid[WIFI_BSSID_LENGTH];
    char gobssid[WIFI_BSSID_LENGTH];
} P2pHalInviteArgv;

typedef struct P2pHalReInviteArgv {
    int networkId;
    char peerbssid[WIFI_BSSID_LENGTH];
} P2pHalReInviteArgv;

typedef struct P2pProvisionDiscoveryArgv {
    char peerbssid[WIFI_BSSID_LENGTH];
    int mode; /* 0/1/2 : Pbc/display/keypad */
} P2pProvisionDiscoveryArgv;

typedef struct P2pWpaGroupConfigArgv {
    int id; /* network id */
    P2pGroupConfigType param; /* set network param */
    char value[WIFI_P2P_GROUP_CONFIG_VALUE_LENGTH];  /* set network value */
} P2pWpaGroupConfigArgv;

typedef struct Hid2dConnectInfo {
    char ssid[WIFI_SSID_LENGTH];
    char bssid[WIFI_BSSID_LENGTH];
    char passphrase[WIFI_P2P_PASSWORD_SIZE];
    int frequency;
} Hid2dConnectInfo;

#ifdef __cplusplus
}
#endif
#endif