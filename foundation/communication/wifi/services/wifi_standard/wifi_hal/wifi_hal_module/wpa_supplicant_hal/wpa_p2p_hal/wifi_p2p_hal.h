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

#ifndef WIFI_P2P_HAL_H
#define WIFI_P2P_HAL_H

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include "wifi_hal_p2p_struct.h"
#include "wifi_hal_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define P2P_PIN_KEYPAD 1
#define P2P_PIN_DISPLAY 2
#define P2P_GROUP_IFACE_MAX_LENGTH 128

typedef struct stWifiWpaP2pInterface WifiWpaP2pInterface;
struct stWifiWpaP2pInterface {
    char ifName[WIFI_IFACE_NAME_MAXLEN];

    P2pSupplicantErrCode (*wpaP2pCliCmdSetWpsName)(WifiWpaP2pInterface *p, const char *name);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetWpsDeviceType)(WifiWpaP2pInterface *p, const char *type);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetWpsSecDeviceType)(WifiWpaP2pInterface *p, const char *type);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetWpsConfigMethods)(WifiWpaP2pInterface *p, const char *methods);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetSsidPostfixName)(WifiWpaP2pInterface *p, const char *name);
    P2pSupplicantErrCode (*wpaP2pCliCmdGetDeviceAddress)(WifiWpaP2pInterface *p, char *devAddress, int size);
    P2pSupplicantErrCode (*wpaP2pCliCmdFlush)(WifiWpaP2pInterface *p);
    P2pSupplicantErrCode (*wpaP2pCliCmdFlushService)(WifiWpaP2pInterface *p);
    P2pSupplicantErrCode (*wpaP2pCliCmdP2pStopFind)(WifiWpaP2pInterface *p);
    P2pSupplicantErrCode (*wpaP2pCliCmdP2pRemoveGroup)(WifiWpaP2pInterface *p, const char *groupname);
    P2pSupplicantErrCode (*wpaP2pCliCmdP2pFound)(WifiWpaP2pInterface *p, int timeout);
    P2pSupplicantErrCode (*wpaP2pCliCmdRemoveNetwork)(WifiWpaP2pInterface *p, int networkId);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetWfdEnable)(WifiWpaP2pInterface *p, int enable);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetWfdDeviceInfo)(WifiWpaP2pInterface *p, const char *conf);
    P2pSupplicantErrCode (*wpaP2pCliCmdExtListen)(WifiWpaP2pInterface *p, int enable, int period, int interval);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetListenChannel)(WifiWpaP2pInterface *p, int channel, int regClass);
    P2pSupplicantErrCode (*wpaP2pCliCmdCancelConnect)(WifiWpaP2pInterface *p);
    P2pSupplicantErrCode (*wpaP2pCliCmdInvite)(WifiWpaP2pInterface *p, const P2pHalInviteArgv *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdReInvite)(WifiWpaP2pInterface *p, const P2pHalReInviteArgv *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdServiceAdd)(WifiWpaP2pInterface *p, const P2pServiceInfo *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdServiceDel)(WifiWpaP2pInterface *p, const P2pServiceInfo *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdServDiscReq)(
        WifiWpaP2pInterface *p, const char *peerBssid, const char *tlvs, char *retSeq, unsigned size);
    P2pSupplicantErrCode (*wpaP2pCliCmdServDiscCancelReq)(WifiWpaP2pInterface *p, const char *val);
    P2pSupplicantErrCode (*wpaP2pCliCmdProvisionDiscovery)(
        WifiWpaP2pInterface *p, const P2pProvisionDiscoveryArgv *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdGroupAdd)(WifiWpaP2pInterface *p, int isPersistent, int networkId, int freq);
    P2pSupplicantErrCode (*wpaP2pCliCmdStoreConfig)(WifiWpaP2pInterface *p);
    P2pSupplicantErrCode (*wpaP2pCliCmdNetworkList)(WifiWpaP2pInterface *p, P2pNetworkList *infoList);
    P2pSupplicantErrCode (*wpaP2pCliCmdConnect)(WifiWpaP2pInterface *p, P2pConnectInfo *info);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetPersistentReconnect)(WifiWpaP2pInterface *p, int status);
    P2pSupplicantErrCode (*wpaP2pCliCmdRespServerDiscovery)(WifiWpaP2pInterface *p, P2pServDiscReqInfo *info);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetServDiscExternal)(WifiWpaP2pInterface *p, int mode);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetRandomMac)(WifiWpaP2pInterface *p, int mode);
    P2pSupplicantErrCode (*wpaP2pCliCmdP2pGetPeer)(
        WifiWpaP2pInterface *p, const char *bssid, P2pDeviceInfo *peerInfo);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetGroupConfig)(WifiWpaP2pInterface *p, const P2pWpaGroupConfigArgv *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdGetGroupConfig)(WifiWpaP2pInterface *p, P2pWpaGroupConfigArgv *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdAddNetwork)(WifiWpaP2pInterface *p, int *networkId);
    P2pSupplicantErrCode (*wpaP2pCliCmdHid2dConnect)(WifiWpaP2pInterface *p, Hid2dConnectInfo *info);
};

typedef struct stWifiWpaP2pGroupInterface WifiWpaP2pGroupInterface;
struct stWifiWpaP2pGroupInterface {
    WifiWpaP2pGroupInterface *next;
    char groupIfname[P2P_GROUP_IFACE_MAX_LENGTH];
    P2pSupplicantErrCode (*wpaP2pCliCmdWpsPbc)(WifiWpaP2pGroupInterface *p, const char *bssid);
    P2pSupplicantErrCode (*wpaP2pCliCmdWpsPin)(WifiWpaP2pGroupInterface *p, P2pWpsPinDisplayArgv *argv);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetPowerSave)(WifiWpaP2pGroupInterface *p, int enable);
    P2pSupplicantErrCode (*wpaP2pCliCmdSetGroupIdle)(WifiWpaP2pGroupInterface *p, int timeout);
};

WifiWpaP2pInterface *GetWifiWapP2pInterface(void);
void RelesaeWpaP2pInterface(void);
WifiErrorNo ConvertP2pErrCode(P2pSupplicantErrCode code);
WifiWpaP2pGroupInterface *GetWifiWpaP2pGroupInterface(const char *groupIfc);
void ReleaseWpaP2pGroupInterface(const char *groupIfc);

#ifdef __cplusplus
}
#endif
#endif