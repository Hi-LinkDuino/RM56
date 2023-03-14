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

#include "i_wifi.h"
#include "client.h" /* RPC client.h */
#include "serial.h"
#include "wifi_log.h"
#include "wifi_idl_define.h"
#include "wifi_idl_inner_interface.h"
#include "i_wifi_chip.h"
#include "i_wifi_chip_event_callback.h"
#include "i_wifi_event_callback.h"
#include "i_wifi_hotspot_iface.h"
#include "i_wifi_sta_iface.h"
#include "i_wifi_supplicant_iface.h"
#include "i_wifi_event_p2p_callback.h"
#include "i_wifi_p2p_iface.h"
#include "i_wifi_public_func.h"

#undef LOG_TAG
#define LOG_TAG "WifiIdlIWifi"

WifiErrorNo GetWifiChip(uint8_t id, IWifiChip *chip)
{
    RpcClient *client = GetChipRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetWifiChip");
    WriteInt(context, id);
    WriteEnd(context);
    if (RpcClientCall(client, "GetWifiChip") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetWifiChip deal failed!");
    } else {
        /* read IWifiChip struct */
        ReadInt(context, &(chip->i));
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo GetWifiChipIds(uint8_t *ids, int32_t *size)
{
    RpcClient *client = GetChipRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "GetWifiChipIds");
    WriteInt(context, *size);
    WriteEnd(context);
    if (RpcClientCall(client, "GetWifiChipIds") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    if (result != WIFI_IDL_OPT_OK) {
        LOGE("server GetWifiChipIds deal failed!");
    } else {
        ReadInt(context, size);
        for (int i = 0; i < *size; ++i) {
            ReadInt(context, (int *)(ids + i));
        }
    }
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo Start(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "Start");
    WriteEnd(context);
    if (RpcClientCall(client, "Start") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo Stop(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "Stop");
    WriteEnd(context);
    if (RpcClientCall(client, "Stop") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

WifiErrorNo NotifyClear(void)
{
    RpcClient *client = GetStaRpcClient();
    LockRpcClient(client);
    Context *context = client->context;
    WriteBegin(context, 0);
    WriteFunc(context, "NotifyClear");
    WriteEnd(context);
    if (RpcClientCall(client, "NotifyClear") != WIFI_IDL_OPT_OK) {
        return WIFI_IDL_OPT_FAILED;
    }
    int result = WIFI_IDL_OPT_FAILED;
    ReadInt(context, &result);
    ReadClientEnd(client);
    UnlockRpcClient(client);
    return result;
}

/* Defines the callback processing function. */
static void IdlCbkAddRemoveIface(Context *context, int event)
{
    int type = 0;
    if (ReadInt(context, &type) < 0) {
        return;
    }
    char *iface = NULL;
    int len = ReadStr(context, iface, 0);
    if (len < 0) {
        return;
    }
    iface = (char *)calloc(len + 1, sizeof(char));
    if (iface == NULL) {
        return;
    }
    if (ReadStr(context, iface, len + 1) < 0) {
        free(iface);
        iface = NULL;
        return;
    }
    IWifiChipEventCallback *callback = GetWifiChipEventCallback();
    if (callback != NULL) {
        if (event == WIFI_IDL_CBK_CMD_ADD_IFACE && callback->onIfaceAdded != NULL) {
            callback->onIfaceAdded(type, iface);
        } else if (event == WIFI_IDL_CBK_CMD_REMOVE_IFACE && callback->onIfaceRemoved != NULL) {
            callback->onIfaceRemoved(type, iface);
        }
    }
    free(iface);
    iface = NULL;
    return;
}

static void IdlCbkStaJoinLeave(Context *context)
{
    CStationInfo info = {0};
    char *reason = NULL;
    if (ReadInt(context, &info.type) < 0) {
        return;
    }
    int len = ReadStr(context, reason, 0);
    if (len < 0) {
        return;
    }
    reason = (char *)calloc(len + 1, sizeof(char));
    if (reason == NULL) {
        return;
    }
    if (ReadStr(context, reason, len + 1) < 0) {
        free(reason);
        reason = NULL;
        return;
    }
    if (strncpy_s(info.mac, sizeof(info.mac), reason, sizeof(info.mac) - 1) != EOK) {
        free(reason);
        reason = NULL;
        return;
    }
    IWifiApEventCallback *callback = GetWifiApEventCallback();
    if (callback != NULL && callback->onStaJoinOrLeave != NULL) {
        callback->onStaJoinOrLeave(&info);
    }
    free(reason);
    reason = NULL;
    return;
}

static void IdlCbkScanInfoNotify(Context *context)
{
    int result = 0;
    if (ReadInt(context, &result) < 0) {
        return;
    }
    ISupplicantEventCallback *callback = GetSupplicantEventCallback();
    if (callback != NULL && callback->onScanNotify != NULL) {
        callback->onScanNotify(result);
    }
    return;
}

static void IdlCbkConnectChanged(Context *context)
{
    int status = 0;
    int networkId = 0;
    char pMac[WIFI_BSSID_LENGTH] = {0};
    if (ReadInt(context, &status) < 0 || ReadInt(context, &networkId) < 0 ||
        ReadStr(context, pMac, sizeof(pMac)) != 0) {
        return;
    }
    IWifiEventCallback *callback = GetWifiEventCallback();
    if (callback != NULL && callback->onConnectChanged != NULL) {
        callback->onConnectChanged(status, networkId, pMac);
    }
    return;
}

static void IdlCbkApStateChange(int event)
{
    IWifiApEventCallback *callback = GetWifiApEventCallback();
    if (callback != NULL && callback->onApEnableOrDisable != NULL) {
        callback->onApEnableOrDisable(event);
    }
    return;
}

static void IdlCbkWpaEventDeal(Context *context, int event)
{
    int status = 0;
    if (ReadInt(context, &status) < 0) {
        return;
    }
    IWifiEventCallback *callback = GetWifiEventCallback();
    if (callback == NULL) {
        return;
    }
    if (event == WIFI_IDL_CBK_CMD_WPS_TIME_OUT && callback->onWpsTimeOut != NULL) {
        callback->onWpsTimeOut(status);
    }
    if (event == WIFI_IDL_CBK_CMD_WPS_OVERLAP && callback->onWpsOverlap != NULL) {
        callback->onWpsOverlap(status);
    }
    if (event == WIFI_IDL_CBK_CMD_SSID_WRONG_KEY && callback->onSsidWrongkey != NULL) {
        callback->onSsidWrongkey(status);
    }
    if (event == WIFI_IDL_CBK_CMD_WPA_STATE_CHANGEM && callback->onWpaStateChanged != NULL) {
        callback->onWpaStateChanged(status);
    }
    if (event == WIFI_IDL_CBK_CMD_WPS_CONNECTION_FULL && callback->onWpsConnectionFull != NULL) {
        callback->onWpsConnectionFull(status);
    }
    if (event == WIFI_IDL_CBK_CMD_WPS_CONNECTION_REJECT && callback->onWpsConnectionReject != NULL) {
        callback->onWpsConnectionReject(status);
    }
    return;
}

static int IdlDealStaApEvent(Context *context, int event)
{
    LOGI("OnTransact deal sta/ap event: %{public}d", event);
    switch (event) {
        case WIFI_IDL_CBK_CMD_FAILURE:
        case WIFI_IDL_CBK_CMD_STARTED:
        case WIFI_IDL_CBK_CMD_STOPED:
            break;
        case WIFI_IDL_CBK_CMD_ADD_IFACE:
        case WIFI_IDL_CBK_CMD_REMOVE_IFACE:
            IdlCbkAddRemoveIface(context, event);
            break;
        case WIFI_IDL_CBK_CMD_STA_JOIN:
        case WIFI_IDL_CBK_CMD_STA_LEAVE:
            IdlCbkStaJoinLeave(context);
            break;
        case WIFI_IDL_CBK_CMD_SCAN_INFO_NOTIFY:
            IdlCbkScanInfoNotify(context);
            break;
        case WIFI_IDL_CBK_CMD_CONNECT_CHANGED:
            IdlCbkConnectChanged(context);
            break;
        case WIFI_IDL_CBK_CMD_AP_ENABLE:
        case WIFI_IDL_CBK_CMD_AP_DISABLE:
            IdlCbkApStateChange(event);
            break;
        case WIFI_IDL_CBK_CMD_WPA_STATE_CHANGEM:
        case WIFI_IDL_CBK_CMD_SSID_WRONG_KEY:
        case WIFI_IDL_CBK_CMD_WPS_OVERLAP:
        case WIFI_IDL_CBK_CMD_WPS_TIME_OUT:
        case WIFI_IDL_CBK_CMD_WPS_CONNECTION_FULL:
        case WIFI_IDL_CBK_CMD_WPS_CONNECTION_REJECT:
            IdlCbkWpaEventDeal(context, event);
            break;
        default:
            return -1;
    }
    return 0;
}

static void IdlCbP2pEventDeal(Context *context)
{
    int status = 0;
    if (ReadInt(context, &status) < 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onP2pSupplicantConnect != NULL) {
        callback->onP2pSupplicantConnect(status);
    }
    return;
}

static void IdlCbP2pSupConnFailedEvent()
{
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->connectSupplicantFailed != NULL) {
        callback->connectSupplicantFailed();
    }
    return;
}

static void IdlCbP2pDeviceFoundEventDeal(Context *context)
{
    P2pDeviceInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    if (ReadInt(context, (int *)&info.configMethods) < 0 || ReadInt(context, &info.deviceCapabilities) < 0 ||
        ReadInt(context, &info.groupCapabilities) < 0 || ReadInt(context, (int *)&info.wfdLength) < 0 ||
        ReadStr(context, info.srcAddress, sizeof(info.srcAddress)) != 0 ||
        ReadStr(context, info.p2pDeviceAddress, sizeof(info.p2pDeviceAddress)) != 0 ||
        ReadStr(context, info.primaryDeviceType, sizeof(info.primaryDeviceType)) != 0 ||
        ReadStr(context, info.deviceName, sizeof(info.deviceName)) != 0 ||
        ReadStr(context, info.wfdDeviceInfo, sizeof(info.wfdDeviceInfo)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onDeviceFound != NULL) {
        callback->onDeviceFound(&info);
    }
    return;
}

static void IdlCbP2pDeviceLostEventDeal(Context *context)
{
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    if (ReadStr(context, address, sizeof(address)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onDeviceLost != NULL) {
        callback->onDeviceLost(address);
    }
    return;
}

static void IdlCbP2pGoNegotiationRequestEvent(Context *context)
{
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    int passId = 0;
    if (ReadInt(context, &passId) < 0 || ReadStr(context, address, sizeof(address)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGoNegotiationRequest != NULL) {
        callback->onGoNegotiationRequest(address, passId);
    }
    return;
}

static void IdlCbP2pGoNegotiationSuccessEvent()
{
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGoNegotiationSuccess != NULL) {
        callback->onGoNegotiationSuccess();
    }
    return;
}

static void IdlCbP2pGoNegotiationFailureEvent(Context *context)
{
    int status = 0;
    if (ReadInt(context, &status) < 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGoNegotiationFailure != NULL) {
        callback->onGoNegotiationFailure(status);
    }
    return;
}

static void IdlCbP2pInvitationReceivedEvent(Context *context)
{
    P2pInvitationInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    if (ReadInt(context, &info.type) < 0 || ReadInt(context, &info.persistentNetworkId) < 0 ||
        ReadInt(context, &info.operatingFrequency) < 0 ||
        ReadStr(context, info.srcAddress, sizeof(info.srcAddress)) != 0 ||
        ReadStr(context, info.goDeviceAddress, sizeof(info.goDeviceAddress)) != 0 ||
        ReadStr(context, info.bssid, sizeof(info.bssid)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onInvitationReceived != NULL) {
        callback->onInvitationReceived(&info);
    }
    return;
}

static void IdlCbP2pInvitationResultEvent(Context *context)
{
    int status = 0;
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    if (ReadInt(context, &status) < 0 || ReadStr(context, address, sizeof(address)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onInvitationResult != NULL) {
        callback->onInvitationResult(address, status);
    }
    return;
}

static void IdlCbP2pGroupFormationSuccessEvent()
{
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGroupFormationSuccess != NULL) {
        callback->onGroupFormationSuccess();
    }
    return;
}

static void IdlCbP2pGroupFormationFailureEvent(Context *context)
{
    char reason[WIFI_P2P_TMP_MSG_LENGTH_128] = {0};
    if (ReadStr(context, reason, sizeof(reason)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGroupFormationFailure != NULL) {
        callback->onGroupFormationFailure(reason);
    }
    return;
}

static void IdlCbP2pGroupStartedEvent(Context *context)
{
    P2pGroupInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    if (ReadInt(context, &info.isGo) < 0 || ReadInt(context, &info.isPersistent) < 0 ||
        ReadInt(context, &info.frequency) < 0 || ReadStr(context, info.groupName, sizeof(info.groupName)) != 0 ||
        ReadStr(context, info.ssid, sizeof(info.ssid)) != 0 || ReadStr(context, info.psk, sizeof(info.psk)) != 0 ||
        ReadStr(context, info.passphrase, sizeof(info.passphrase)) != 0 ||
        ReadStr(context, info.goDeviceAddress, sizeof(info.goDeviceAddress)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGroupStarted != NULL) {
        callback->onGroupStarted(&info);
    }
    return;
}

static void IdlCbP2pGroupRemovedEvent(Context *context)
{
    int isGo = 0;
    char groupName[WIFI_P2P_MAX_GROUP_IFACE_NAME_LENGTH] = {0};
    if (ReadInt(context, &isGo) < 0 || ReadStr(context, groupName, sizeof(groupName)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onGroupRemoved != NULL) {
        callback->onGroupRemoved(groupName, isGo);
    }
    return;
}

static void IdlCbP2pProvDiscEvent(Context *context, int event)
{
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    if (ReadStr(context, address, sizeof(address)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback == NULL) {
        return;
    }
    if (event == WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_REQ_EVENT && callback->onProvisionDiscoveryPbcRequest != NULL) {
        callback->onProvisionDiscoveryPbcRequest(address);
    }
    if (event == WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_RSP_EVENT && callback->onProvisionDiscoveryPbcResponse != NULL) {
        callback->onProvisionDiscoveryPbcResponse(address);
    }
    if (event == WIFI_IDL_CBK_CMD_P2P_PROV_DISC_ENTER_PIN_EVENT && callback->onProvisionDiscoveryEnterPin != NULL) {
        callback->onProvisionDiscoveryEnterPin(address);
    }
    return;
}

static void IdlCbP2pProDiscShowPinEvent(Context *context)
{
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    char pin[WIFI_PIN_CODE_LENGTH + 1] = {0};
    if (ReadStr(context, address, sizeof(address)) != 0 || ReadStr(context, pin, sizeof(pin)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onProvisionDiscoveryShowPin != NULL) {
        callback->onProvisionDiscoveryShowPin(address, pin);
    }
    return;
}

static void IdlCbP2pFindStopEvent()
{
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onFindStopped != NULL) {
        callback->onFindStopped();
    }
    return;
}

static int NumStrToNumArry(unsigned char *src, int *size)
{
    if (src == NULL || size == NULL) {
        return -1;
    }
    if (*size < 0 || ((unsigned)*size & 1) != 0) {
        return -1;
    }
    const int base = 10;
    const int shiftNum = 4;
    int len = *size;
    int pos = 0;
    for (int i = 0; i < len; ++i) {
        unsigned char c = src[i];
        unsigned char n;
        if (c >= '0' && c <= '9') {
            n = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            n = c - 'A' + base;
        } else if (c >= 'a' && c <= 'f') {
            n = c - 'a' + base;
        } else {
            return -1;
        }
        if ((i & 0x1) == 0) {
            src[pos] = n;
        } else {
            src[pos] <<= shiftNum;
            src[pos] |= n;
            ++pos;
        }
    }
    src[pos] = 0;
    *size = pos;
    return 0;
}

static void IdlCbP2pServDiscRespEvent(Context *context)
{
    int updataIndicator = 0;
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    int tlvsLength = 0;
    if (ReadInt(context, &updataIndicator) < 0 || ReadStr(context, address, sizeof(address)) != 0 ||
        ReadInt(context, &tlvsLength) < 0) {
        return;
    }
    unsigned char *tlvs = NULL;
    if (tlvsLength > 0) {
        tlvs = (unsigned char *)calloc(tlvsLength + 1, sizeof(unsigned char));
        if (tlvs == NULL) {
            return;
        }
        if (ReadStr(context, (char *)tlvs, tlvsLength + 1) != 0) {
            free(tlvs);
            tlvs = NULL;
            return;
        }
        if (NumStrToNumArry(tlvs, &tlvsLength) < 0) {
            LOGE("Failed to convert tlvs hex string to byte list.");
            free(tlvs);
            tlvs = NULL;
            return;
        }
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onServiceDiscoveryResponse != NULL) {
        callback->onServiceDiscoveryResponse(address, updataIndicator, tlvs, tlvsLength);
    }
    free(tlvs);
    tlvs = NULL;
    return;
}

static void IdlCbP2pProvServDiscFailureEvent()
{
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onProvisionDiscoveryFailure != NULL) {
        callback->onProvisionDiscoveryFailure();
    }
    return;
}

static void IdlCbP2pApStaConnectEvent(Context *context, int event)
{
    char address[WIFI_MAC_ADDR_LENGTH + 1] = {0};
    if (ReadStr(context, address, sizeof(address)) != 0) {
        return;
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback == NULL) {
        return;
    }
    if (event == WIFI_IDL_CBK_CMD_AP_STA_DISCONNECTED_EVENT && callback->onStaDeauthorized != NULL) {
        callback->onStaDeauthorized(address);
    }
    if (event == WIFI_IDL_CBK_CMD_AP_STA_CONNECTED_EVENT && callback->onStaAuthorized != NULL) {
        callback->onStaAuthorized(address);
    }
    return;
}

static void IdlCbP2pServDiscReqEvent(Context *context)
{
    P2pServDiscReqInfo info;
    if (memset_s(&info, sizeof(info), 0, sizeof(info)) != EOK) {
        return;
    }
    if (ReadInt(context, &info.freq) < 0 || ReadInt(context, &info.dialogToken) < 0 ||
        ReadInt(context, &info.updateIndic) < 0 || ReadStr(context, info.mac, sizeof(info.mac)) != 0 ||
        ReadInt(context, &info.tlvsLength) < 0) {
        return;
    }
    if (info.tlvsLength > 0) {
        info.tlvs = (unsigned char *)calloc(info.tlvsLength + 1, sizeof(unsigned char));
        if (info.tlvs == NULL) {
            return;
        }
        ReadStr(context, (char *)info.tlvs, info.tlvsLength + 1);
        if (NumStrToNumArry(info.tlvs, &info.tlvsLength) < 0) {
            LOGE("Failed to convert tlvs hex string to byte list.");
            free(info.tlvs);
            info.tlvs = NULL;
            return;
        }
    }
    IWifiEventP2pCallback *callback = GetWifiP2pEventCallback();
    if (callback != NULL && callback->onP2pServDiscReq != NULL) {
        callback->onP2pServDiscReq(&info);
    }
    free(info.tlvs);
    info.tlvs = NULL;
    return;
}

static int IdlDealP2pEventFirst(Context *context, int event)
{
    switch (event) {
        case WIFI_IDL_CBK_CMD_P2P_SUPPLICANT_CONNECT:
            IdlCbP2pEventDeal(context);
            break;
        case WIFI_IDL_CBK_CMD_SUP_CONN_FAILED_EVENT:
            IdlCbP2pSupConnFailedEvent();
            break;
        case WIFI_IDL_CBK_CMD_P2P_DEVICE_FOUND_EVENT:
            IdlCbP2pDeviceFoundEventDeal(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_DEVICE_LOST_EVENT:
            IdlCbP2pDeviceLostEventDeal(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_REQUEST_EVENT:
            IdlCbP2pGoNegotiationRequestEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_SUCCESS_EVENT:
            IdlCbP2pGoNegotiationSuccessEvent();
            break;
        case WIFI_IDL_CBK_CMD_P2P_GO_NEGOTIATION_FAILURE_EVENT:
            IdlCbP2pGoNegotiationFailureEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_INVITATION_RECEIVED_EVENT:
            IdlCbP2pInvitationReceivedEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_INVITATION_RESULT_EVENT:
            IdlCbP2pInvitationResultEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_GROUP_FORMATION_SUCCESS_EVENT:
            IdlCbP2pGroupFormationSuccessEvent();
            break;
        case WIFI_IDL_CBK_CMD_P2P_GROUP_FORMATION_FAILURE_EVENT:
            IdlCbP2pGroupFormationFailureEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_GROUP_STARTED_EVENT:
            IdlCbP2pGroupStartedEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_GROUP_REMOVED_EVENT:
            IdlCbP2pGroupRemovedEvent(context);
            break;
        default:
            return -1;
    }
    return 0;
}

static int IdlDealP2pEventSecond(Context *context, int event)
{
    switch (event) {
        case WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_REQ_EVENT:
        case WIFI_IDL_CBK_CMD_P2P_PROV_DISC_PBC_RSP_EVENT:
        case WIFI_IDL_CBK_CMD_P2P_PROV_DISC_ENTER_PIN_EVENT:
            IdlCbP2pProvDiscEvent(context, event);
            break;
        case WIFI_IDL_CBK_CMD_P2P_PROV_DISC_SHOW_PIN_EVENT:
            IdlCbP2pProDiscShowPinEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_FIND_STOPPED_EVENT:
            IdlCbP2pFindStopEvent();
            break;
        case WIFI_IDL_CBK_CMD_P2P_SERV_DISC_RESP_EVENT:
            IdlCbP2pServDiscRespEvent(context);
            break;
        case WIFI_IDL_CBK_CMD_P2P_PROV_DISC_FAILURE_EVENT:
            IdlCbP2pProvServDiscFailureEvent();
            break;
        case WIFI_IDL_CBK_CMD_AP_STA_DISCONNECTED_EVENT:
        case WIFI_IDL_CBK_CMD_AP_STA_CONNECTED_EVENT:
            IdlCbP2pApStaConnectEvent(context, event);
            break;
        case WIFI_IDL_CBK_CMD_P2P_SERV_DISC_REQ_EVENT:
            IdlCbP2pServDiscReqEvent(context);
            break;
        default:
            return -1;
    }
    return 0;
}

static int IdlDealP2pEvent(Context *context, int event)
{
    LOGI("OnTransact deal p2p event: %{public}d", event);
    if (IdlDealP2pEventFirst(context, event) == 0 || IdlDealP2pEventSecond(context, event) == 0) {
        return 0;
    }
    return -1;
}

int OnTransact(Context *context)
{
    int event = 0;
    if (ReadInt(context, &event) < 0) {
        LOGE("OnTransact read event failed!");
        return -1;
    }
    if (IdlDealStaApEvent(context, event) < 0 && IdlDealP2pEvent(context, event) < 0) {
        LOGI("unsupport call back events: %{public}d", event);
    }
    return 0;
}
