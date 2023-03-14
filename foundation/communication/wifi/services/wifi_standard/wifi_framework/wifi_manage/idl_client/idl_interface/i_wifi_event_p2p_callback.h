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

#ifndef OHOS_IDL_IWIFI_EVENT_P2P_CALLBACK_H
#define OHOS_IDL_IWIFI_EVENT_P2P_CALLBACK_H

#include "i_wifi_struct.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IWifiEventP2pCallback {
    void (*onP2pSupplicantConnect)(int status);
    void (*onDeviceFound)(const P2pDeviceInfo *device);
    void (*onDeviceLost)(const char *p2pDeviceAddress);
    void (*onGoNegotiationRequest)(const char *srcAddress, short passwordId);
    void (*onGoNegotiationSuccess)(void);
    void (*onGoNegotiationFailure)(int status);
    void (*onInvitationReceived)(const P2pInvitationInfo *info);
    void (*onInvitationResult)(const char *bssid, int status);
    void (*onGroupFormationSuccess)(void);
    void (*onGroupFormationFailure)(const char *failureReason);
    void (*onGroupStarted)(const P2pGroupInfo *group);
    void (*onGroupRemoved)(const char *groupIfName, int isGo);
    void (*onProvisionDiscoveryPbcRequest)(const char *p2pDeviceAddress);
    void (*onProvisionDiscoveryPbcResponse)(const char *p2pDeviceAddress);
    void (*onProvisionDiscoveryEnterPin)(const char *p2pDeviceAddress);
    void (*onProvisionDiscoveryShowPin)(const char *p2pDeviceAddress, const char *generatedPin);
    void (*onProvisionDiscoveryFailure)(void);
    void (*onFindStopped)(void);
    void (*onServiceDiscoveryResponse)(
        const char *srcAddress, short updateIndicator, const unsigned char *tlvs, size_t tlvsLength);
    void (*onStaDeauthorized)(const char *p2pDeviceAddress);
    void (*onStaAuthorized)(const char *p2pDeviceAddress);
    void (*connectSupplicantFailed)(void);
    void (*onP2pServDiscReq)(const P2pServDiscReqInfo *info);
} IWifiEventP2pCallback;

#ifdef __cplusplus
}
#endif
#endif