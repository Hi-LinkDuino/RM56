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

#ifndef OHOS_WIFI_IDL_INNER_INTERFACE_H
#define OHOS_WIFI_IDL_INNER_INTERFACE_H

#include "client.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Get the Ap Rpc Client object.
 *
 * @return RpcClient*
 */
RpcClient *GetApRpcClient(void);

void OnApStaJoinOrLeave(const CStationInfo *info);
void OnApEnableOrDisable(int satus);

/**
 * @Description Get the Chip Rpc Client object.
 *
 * @return RpcClient*
 */
RpcClient *GetChipRpcClient(void);

/**
 * @Description Get the Sta Rpc Client object.
 *
 * @return RpcClient*
 */
RpcClient *GetStaRpcClient(void);

void OnConnectChanged(int status, int networkId, const char *mac);
void OnWpaStateChanged(int status);
void OnWpaSsidWrongKey(int status);
void OnWpsOverlap(int status);
void OnWpsTimeOut(int status);
void onWpaConnectionFull(int status);
void onWpaConnectionReject(int status);

/**
 * @Description Get the Supplicant Rpc Client object.
 *
 * @return RpcClient*
 */
RpcClient *GetSupplicantRpcClient(void);

void OnScanNotify(int status);

/**
 * @Description Get the P2p Rpc Client object.
 *
 * @return RpcClient*
 */
RpcClient *GetP2pRpcClient(void);

void OnP2pConnectSupplicant(int state);
void OnP2pDeviceFound(const P2pDeviceInfo *info);
void OnP2pDeviceLost(const char *p2pDeviceAddress);
void OnP2pGoNegotiationRequest(const char *srcAddress, short passwordId);
void OnP2pGoNegotiationSuccess(void);
void OnP2pGoNegotiationFailure(int status);
void OnP2pInvitationResult(const char *bssid, int status);
void OnP2pInvitationReceived(const P2pInvitationInfo *info);
void OnP2pGroupFormationSuccess(void);
void OnP2pGroupFormationFailure(const char *failureReason);
void OnP2pGroupStarted(const P2pGroupInfo *group);
void OnP2pGroupRemoved(const char *groupIfName, int isGo);
void OnP2pProvisionDiscoveryPbcRequest(const char *p2pDeviceAddress);
void OnP2pProvisionDiscoveryPbcResponse(const char *p2pDeviceAddress);
void OnP2pProvisionDiscoveryEnterPin(const char *p2pDeviceAddress);
void OnP2pProvisionDiscoveryShowPin(const char *p2pDeviceAddress, const char *generatedPin);
void OnP2pProvisionDiscoveryFailure(void);
void OnP2pFindStopped(void);
void OnP2pServiceDiscoveryResponse(
    const char *srcAddress, short updateIndicator, const unsigned char *tlvs, size_t tlvsLength);
void OnP2pStaDeauthorized(const char *p2pDeviceAddress);
void OnP2pStaAuthorized(const char *p2pDeviceAddress);
void OnP2pConnectSupplicantFailed(void);
void OnP2pServDiscReq(const P2pServDiscReqInfo *info);
#ifdef __cplusplus
}
#endif
#endif