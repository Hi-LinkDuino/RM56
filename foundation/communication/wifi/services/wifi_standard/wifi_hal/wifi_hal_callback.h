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

#ifndef OHOS_WIFI_HAL_CALLBACK_H
#define OHOS_WIFI_HAL_CALLBACK_H

#include "wifi_hal_struct.h"
#include "wifi_hal_p2p_struct.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @Description Wi-Fi Hal callback notification indicating that the scanning is complete.
 *
 * @param status
 */
void WifiHalCbNotifyScanEnd(int status);

/**
 * @Description Wi-Fi Hal callback notification of the connection change.
 *
 * @param status
 * @param networkId
 * @param pos
 */
void WifiHalCbNotifyConnectChanged(int status, int networkId, const char *pos);
/**
 * @Description The Wi-Fi Hal module notifies the WPA module of the status change.
 *
 * @param status
 */
void WifiHalCbNotifyWpaStateChange(int status);
/**
 * @Description Wi-Fi Hal callback notification error key.
 *
 * @param status
 */
void WifiHalCbNotifyWrongKey(int status);

/**
 * @Description Wi-Fi Hal callback notification connection full
 *
 * @param status
 */
void WifiHalCbNotifyConnectionFull(int status);

/**
 * @Description Wi-Fi Hal callback notification connection reject
 *
 * @param status
 */
void WifiHalCbNotifyConnectionReject(int status);

/**
 * @Description Wi-Fi Hal callback notification WPS overlaps.
 *
 * @param event
 */
void WifiHalCbNotifyWpsOverlap(int event);
/**
 * @Description Wi-Fi Hal callback notification WPS times out.
 *
 * @param event
 */
void WifiHalCbNotifyWpsTimeOut(int event);
/**
 * @Description Wi-Fi Hal calls back the STA to join the AP.
 *
 * @param content
 */
void WifiHalCbStaJoin(const char *content);

/**
 * @Description Wi-Fi Hal callback AP status.
 *
 * @param content
 */
void WifiHalCbApState(const char *content);

/**
 * @Description wpa_supplicant client connection result event
 *
 * @param state - event value
 */
void WifiP2pHalCbNotifyConnectSupplicant(int state);

/**
 * @Description Indicates that a P2P device has been found
 *
 * @param device
 */
void P2pHalCbDeviceFound(const P2pDeviceInfo *device);

/**
 * @Description Indicates that a P2P device is lost
 *
 * @param p2pDeviceAddress
 */
void P2pHalCbDeviceLost(const char *p2pDeviceAddress);

/**
 * @Description This parameter indicates that a P2P group owner negotiation request is received
 *
 * @param srcAddress
 * @param passwordId
 */
void P2pHalCbGoNegotiationRequest(const char *srcAddress, short passwordId);

/**
 * @Description Go negotiation succeeded
 *
 */
void P2pHalCbGoNegotiationSuccess();

/**
 * @Description Go negotiation failed
 *
 * @param status
 */
void P2pHalCbGoNegotiationFailure(int status);

/**
 * @Description Receive a P2P invitation
 *
 * @param info
 */
void P2pHalCbInvitationReceived(const P2pInvitationInfo *info);

/**
 * @Description Indicates the result of a P2P invitation request
 *
 * @param bssid
 * @param status
 */
void P2pHalCbInvitationResult(const char *bssid, int status);

/**
 * @Description Indicates that a P2P group is successfully formed
 *
 */
void P2pHalCbGroupFormationSuccess();

/**
 * @Description Group Failure and Cause
 *
 * @param reason
 */
void P2pHalCbGroupFormationFailure(const char *reason);

/**
 * @Description This parameter specifies the start of a P2P group
 *
 * @param info
 */
void P2pHalCbGroupStarted(const P2pGroupInfo *info);

/**
 * @Description Deletes a P2P group
 *
 * @param groupIfName
 * @param isGo
 */
void P2pHalCbGroupRemoved(const char *groupIfName, int isGo);

/**
 * @Description Sets the PBC discovery request
 *
 * @param address
 */
void P2pHalCbProvisionDiscoveryPbcRequest(const char *address);

/**
 * @Description Provision Discovery Pbc Response
 *
 * @param address
 */
void P2pHalCbProvisionDiscoveryPbcResponse(const char *address);

/**
 * @Description Sets the discovery access PIN
 *
 * @param address
 */
void P2pHalCbProvisionDiscoveryEnterPin(const char *address);

/**
 * @Description Setting the PIN to be displayed during discovery
 *
 * @param address
 * @param pin
 */
void P2pHalCbProvisionDiscoveryShowPin(const char *address, const char *pin);

/**
 * @Description Failed to discover the Provision
 *
 */
void P2pHalCbProvisionDiscoveryFailure();

/**
 * @Description Indicates the termination of the P2P find operation
 *
 */
void P2pHalCbFindStopped();

/**
 * @Description Indicates that a P2P service discovery response is received
 *
 * @param srcAddress
 * @param updateIndicator
 * @param tlvs
 * @param tlvsLength
 */
void P2pHalCbServiceDiscoveryResponse(const P2pServDiscRespInfo *info);

/**
 * @Description Indicates when a STA device is connected/disconnected to this device
 *
 * @param p2pDeviceAddress
 * @param type - 0 disconnect, 1 connected
 */
void P2pHalCbStaConnectState(const char *p2pDeviceAddress, int state);

/**
 * @Description Reporting Link Failure Events
 *
 * @param iface
 */
void P2pHalCbConnectSupplicantFailed();

/**
 * @Description Service Discovery Request
 *
 * @param info
 */
void P2pHalCbServDiscReq(const P2pServDiscReqInfo *info);
#ifdef __cplusplus
}
#endif
#endif