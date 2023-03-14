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

#ifndef OHOS_C_P2P_CONFIG_H
#define OHOS_C_P2P_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COMMON_MAC_LEN
#define COMMON_MAC_LEN 6
#endif

#define PASSPHRASE_LENGTH 64
#define P2P_NAME_LENGTH 33
#define INTERFACE_LENGTH 16
#define DEVICE_TYPE_LENGTH 128
#define MAX_DEVICES_NUM 256
#define IP_ADDR_STR_LEN 16

typedef enum GroupOwnerBand {
    GO_BAND_AUTO,
    GO_BAND_2GHZ,
    GO_BAND_5GHZ
} GroupOwnerBand;

typedef struct WifiP2pConfig {
    unsigned char devAddr[COMMON_MAC_LEN]; /* the device MAC address */
    GroupOwnerBand goBand;
    int netId; /* network id, when -2 means persistent and -1 means temporary, else need >= 0 */
    char passphrase[PASSPHRASE_LENGTH]; /* the value ranges from 8 to 63. */
    int groupOwnerIntent; /* the value is -1.(A value of -1 indicates the system can choose an appropriate value.) */
    char groupName[P2P_NAME_LENGTH]; /* the value ranges from 1 to 32. */
} WifiP2pConfig;

typedef enum P2pGroupStatus {
    GS_CREATING,
    GS_CREATED,
    GS_STARTED,
    GS_REMOVING,
    GS_INVALID
} P2pGroupStatus;

typedef enum P2pDeviceStatus {
    PDS_CONNECTED,
    PDS_INVITED,
    PDS_FAILED,
    PDS_AVAILABLE,
    PDS_UNAVAILABLE
} P2pDeviceStatus;

typedef enum P2pState {
    P2P_STATE_NONE = 0,
    P2P_STATE_IDLE,
    P2P_STATE_STARTING,
    P2P_STATE_STARTED,
    P2P_STATE_CLOSING,
    P2P_STATE_CLOSED,
} P2pState;

typedef enum P2pConnectionState {
    P2P_DISCONNECTED = 0,
    P2P_CONNECTED,
} P2pConnectionState;

typedef struct WifiP2pWfdInfo {
    int wfdEnabled; /* 0: false, 1: true */
    int deviceInfo;
    int ctrlPort;
    int maxThroughput;
} WifiP2pWfdInfo;

typedef struct WifiP2pDevice {
    char deviceName[P2P_NAME_LENGTH]; /* the value range is 0 to 32 characters. */
    unsigned char devAddr[COMMON_MAC_LEN]; /* the device MAC address */
    char primaryDeviceType[DEVICE_TYPE_LENGTH];
    char secondaryDeviceType[DEVICE_TYPE_LENGTH];
    P2pDeviceStatus status;
    WifiP2pWfdInfo wfdInfo;
    unsigned int supportWpsConfigMethods;
    int deviceCapabilitys;
    int groupCapabilitys;
} WifiP2pDevice;

typedef struct WifiP2pGroupInfo {
    WifiP2pDevice owner;
    int isP2pGroupOwner; /* 0: false, 1: true */
    char passphrase[PASSPHRASE_LENGTH]; /* the value ranges from 8 to 63. */
    char interface[INTERFACE_LENGTH];
    char groupName[P2P_NAME_LENGTH];
    int networkId;
    int frequency; /* for example : freq=2412 to select 2.4 GHz channel 1.(Based on 2.4 GHz or 5 GHz) */
    int isP2pPersistent; /* 0: false, 1: true */
    P2pGroupStatus groupStatus;
    WifiP2pDevice clientDevices[MAX_DEVICES_NUM];
    int clientDevicesSize; /* the true size of clientDevices array */
    char goIpAddress[IP_ADDR_STR_LEN];
} WifiP2pGroupInfo;

typedef struct WifiP2pLinkedInfo {
    P2pConnectionState connectState;
    int isP2pGroupOwner;
    unsigned char groupOwnerAddress[COMMON_MAC_LEN];
} WifiP2pLinkedInfo;

#ifdef __cplusplus
}
#endif

#endif
