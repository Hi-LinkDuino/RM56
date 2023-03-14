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
#include "wifi_idl_inner_interface.h"
#include "wifi_chip_hal_interface.h"
#include "wifi_ap_hal_interface.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_supplicant_hal_interface.h"
#include "wifi_p2p_hal_interface.h"
#include "wifi_global_func.h"

RpcClient *GetChipRpcClient(void)
{
    return OHOS::Wifi::WifiChipHalInterface::GetInstance().mIdlClient->pRpcClient;
}

RpcClient *GetApRpcClient(void)
{
    return OHOS::Wifi::WifiApHalInterface::GetInstance().mIdlClient->pRpcClient;
}

void OnApStaJoinOrLeave(const CStationInfo *info)
{
    if (info == nullptr) {
        return;
    }
    const OHOS::Wifi::IWifiApMonitorEventCallback &cbk =
        OHOS::Wifi::WifiApHalInterface::GetInstance().GetApCallbackInst();
    if (cbk.onStaJoinOrLeave) {
        OHOS::Wifi::WifiApConnectionNofify cbInfo;
        cbInfo.type = info->type;
        cbInfo.mac = info->mac;
        cbk.onStaJoinOrLeave(cbInfo);
    }
}

void OnApEnableOrDisable(int satus)
{
    const OHOS::Wifi::IWifiApMonitorEventCallback &cbk =
        OHOS::Wifi::WifiApHalInterface::GetInstance().GetApCallbackInst();
    if (cbk.onApEnableOrDisable) {
        cbk.onApEnableOrDisable(satus);
    }
}

RpcClient *GetStaRpcClient(void)
{
    return OHOS::Wifi::WifiStaHalInterface::GetInstance().mIdlClient->pRpcClient;
}

void OnConnectChanged(int status, int networkId, const char *mac)
{
    if (mac == nullptr) {
        return;
    }
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onConnectChanged) {
        cbk.onConnectChanged(status, networkId, mac);
    }
}

void OnWpaStateChanged(int status)
{
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onWpaStateChanged) {
        cbk.onWpaStateChanged(status);
    }
}

void OnWpaSsidWrongKey(int status)
{
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onWpaSsidWrongKey) {
        cbk.onWpaSsidWrongKey(status);
    }
}

void onWpaConnectionFull(int status)
{
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onWpaConnectionFull) {
        cbk.onWpaConnectionFull(status);
    }
}

void onWpaConnectionReject(int status)
{
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onWpaConnectionReject) {
        cbk.onWpaConnectionReject(status);
    }
}

void OnWpsOverlap(int status)
{
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onWpsOverlap) {
        cbk.onWpsOverlap(status);
    }
}

void OnWpsTimeOut(int status)
{
    const OHOS::Wifi::WifiEventCallback &cbk = OHOS::Wifi::WifiStaHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onWpsTimeOut) {
        cbk.onWpsTimeOut(status);
    }
}

RpcClient *GetSupplicantRpcClient(void)
{
    return OHOS::Wifi::WifiSupplicantHalInterface::GetInstance().mIdlClient->pRpcClient;
}

void OnScanNotify(int status)
{
    const OHOS::Wifi::SupplicantEventCallback &cbk =
        OHOS::Wifi::WifiSupplicantHalInterface::GetInstance().GetCallbackInst();
    if (cbk.onScanNotify) {
        cbk.onScanNotify(status);
    }
}

RpcClient *GetP2pRpcClient(void)
{
    return OHOS::Wifi::WifiP2PHalInterface::GetInstance().mIdlClient->pRpcClient;
}

void OnP2pConnectSupplicant(int state)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onConnectSupplicant) {
        cbk.onConnectSupplicant(state);
    }
}

void OnP2pDeviceFound(const P2pDeviceInfo *info)
{
    if (info == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onDeviceFound) {
        OHOS::Wifi::IdlP2pDeviceFound cbInfo;
        cbInfo.srcAddress = info->srcAddress;
        cbInfo.p2pDeviceAddress = info->p2pDeviceAddress;
        cbInfo.primaryDeviceType = info->primaryDeviceType;
        cbInfo.deviceName = info->deviceName;
        cbInfo.configMethods = info->configMethods;
        cbInfo.deviceCapabilities = info->deviceCapabilities;
        cbInfo.groupCapabilities = info->groupCapabilities;
        const int wfdLength = 14; /* wfd info type: 0x000000000000 */
        const int wfdStartPos = 2; /* skip 0x */
        if (info->wfdLength >= wfdLength && strlen(info->wfdDeviceInfo) >= wfdLength) {
            OHOS::Wifi::HexStringToVec(info->wfdDeviceInfo + wfdStartPos, cbInfo.wfdDeviceInfo);
        }
        cbk.onDeviceFound(cbInfo);
    }
}

void OnP2pDeviceLost(const char *p2pDeviceAddress)
{
    if (p2pDeviceAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onDeviceLost) {
        cbk.onDeviceLost(p2pDeviceAddress);
    }
}

void OnP2pGoNegotiationRequest(const char *srcAddress, short passwordId)
{
    if (srcAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGoNegotiationRequest) {
        cbk.onGoNegotiationRequest(srcAddress, passwordId);
    }
}

void OnP2pGoNegotiationSuccess(void)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGoNegotiationSuccess) {
        cbk.onGoNegotiationSuccess();
    }
}

void OnP2pGoNegotiationFailure(int status)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGoNegotiationFailure) {
        cbk.onGoNegotiationFailure(status);
    }
}

void OnP2pInvitationResult(const char *bssid, int status)
{
    if (bssid == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onInvitationResult) {
        cbk.onInvitationResult(bssid, status);
    }
}

void OnP2pInvitationReceived(const P2pInvitationInfo *info)
{
    if (info == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onInvitationReceived) {
        OHOS::Wifi::IdlP2pInvitationInfo cbInfo;
        cbInfo.type = info->type;
        cbInfo.persistentNetworkId = info->persistentNetworkId;
        cbInfo.operatingFrequency = info->operatingFrequency;
        cbInfo.srcAddress = info->srcAddress;
        cbInfo.goDeviceAddress = info->goDeviceAddress;
        cbInfo.bssid = info->bssid;
        cbk.onInvitationReceived(cbInfo);
    }
}

void OnP2pGroupFormationSuccess(void)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGroupFormationSuccess) {
        cbk.onGroupFormationSuccess();
    }
}

void OnP2pGroupFormationFailure(const char *failureReason)
{
    if (failureReason == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGroupFormationFailure) {
        cbk.onGroupFormationFailure(failureReason);
    }
}

void OnP2pGroupStarted(const P2pGroupInfo *group)
{
    if (group == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGroupStarted) {
        OHOS::Wifi::IdlP2pGroupInfo cbInfo;
        cbInfo.isGo = group->isGo;
        cbInfo.isPersistent = group->isPersistent;
        cbInfo.frequency = group->frequency;
        cbInfo.groupName = group->groupName;
        cbInfo.ssid = group->ssid;
        cbInfo.psk = group->psk;
        cbInfo.passphrase = group->passphrase;
        cbInfo.goDeviceAddress = group->goDeviceAddress;
        cbk.onGroupStarted(cbInfo);
    }
}

void OnP2pGroupRemoved(const char *groupIfName, int isGo)
{
    if (groupIfName == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onGroupRemoved) {
        cbk.onGroupRemoved(groupIfName, (isGo == 1));
    }
}

void OnP2pProvisionDiscoveryPbcRequest(const char *p2pDeviceAddress)
{
    if (p2pDeviceAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onProvisionDiscoveryPbcRequest) {
        cbk.onProvisionDiscoveryPbcRequest(p2pDeviceAddress);
    }
}

void OnP2pProvisionDiscoveryPbcResponse(const char *p2pDeviceAddress)
{
    if (p2pDeviceAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onProvisionDiscoveryPbcResponse) {
        cbk.onProvisionDiscoveryPbcResponse(p2pDeviceAddress);
    }
}

void OnP2pProvisionDiscoveryEnterPin(const char *p2pDeviceAddress)
{
    if (p2pDeviceAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onProvisionDiscoveryEnterPin) {
        cbk.onProvisionDiscoveryEnterPin(p2pDeviceAddress);
    }
}

void OnP2pProvisionDiscoveryShowPin(const char *p2pDeviceAddress, const char *generatedPin)
{
    if (p2pDeviceAddress == nullptr || generatedPin == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onProvisionDiscoveryShowPin) {
        cbk.onProvisionDiscoveryShowPin(p2pDeviceAddress, generatedPin);
    }
}

void OnP2pProvisionDiscoveryFailure(void)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onProvisionDiscoveryFailure) {
        cbk.onProvisionDiscoveryFailure();
    }
}

void OnP2pFindStopped(void)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onFindStopped) {
        cbk.onFindStopped();
    }
}

void OnP2pServiceDiscoveryResponse(
    const char *srcAddress, short updateIndicator, const unsigned char *tlvs, size_t tlvsLength)
{
    if (srcAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onServiceDiscoveryResponse) {
        std::vector<unsigned char> tlvList;
        if (tlvs != nullptr) {
            OHOS::Wifi::Char2Vec(tlvs, tlvsLength, tlvList);
        }
        cbk.onServiceDiscoveryResponse(srcAddress, updateIndicator, tlvList);
    }
}

void OnP2pStaDeauthorized(const char *p2pDeviceAddress)
{
    if (p2pDeviceAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onStaDeauthorized) {
        cbk.onStaDeauthorized(p2pDeviceAddress);
    }
}

void OnP2pStaAuthorized(const char *p2pDeviceAddress)
{
    if (p2pDeviceAddress == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onStaAuthorized) {
        cbk.onStaAuthorized(p2pDeviceAddress);
    }
}

void OnP2pConnectSupplicantFailed(void)
{
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.connectSupplicantFailed) {
        cbk.connectSupplicantFailed();
    }
}

void OnP2pServDiscReq(const P2pServDiscReqInfo *info)
{
    if (info == nullptr) {
        return;
    }
    const OHOS::Wifi::P2pHalCallback &cbk = OHOS::Wifi::WifiP2PHalInterface::GetInstance().GetP2pCallbackInst();
    if (cbk.onP2pServDiscReq) {
        OHOS::Wifi::IdlP2pServDiscReqInfo cbInfo;
        cbInfo.freq = info->freq;
        cbInfo.dialogToken = info->dialogToken;
        cbInfo.updateIndic = info->updateIndic;
        cbInfo.mac = info->mac;
        if (info->tlvsLength > 0 && info->tlvs != nullptr) {
            OHOS::Wifi::Char2Vec(info->tlvs, info->tlvsLength, cbInfo.tlvList);
        }
        cbk.onP2pServDiscReq(cbInfo);
    }
}
