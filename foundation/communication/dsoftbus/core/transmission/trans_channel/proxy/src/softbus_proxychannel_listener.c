/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "softbus_proxychannel_listener.h"

#include <securec.h>

#include "bus_center_manager.h"
#include "softbus_conn_interface.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_proxychannel_callback.h"
#include "softbus_proxychannel_manager.h"
#include "softbus_proxychannel_network.h"
#include "softbus_proxychannel_session.h"
#include "softbus_utils.h"

static int32_t NotifyNormalChannelClosed(const char *pkgName, int32_t channelId)
{
    int32_t ret = TransProxyOnChannelClosed(pkgName, channelId);
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "proxy channel close, channelId = %d, ret = %d", channelId, ret);
    return ret;
}

static int32_t NotifyNormalChannelOpenFailed(const char *pkgName, int32_t channelId)
{
    int32_t ret = TransProxyOnChannelOpenFailed(pkgName, channelId);
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "proxy channel open fail, channelId = %d, ret = %d", channelId, ret);
    return ret;
}

static int32_t NotifyNormalChannelOpened(int32_t channelId, const AppInfo *appInfo, bool isServer)
{
    ChannelInfo info = {0};
    info.channelId = channelId;
    info.channelType = CHANNEL_TYPE_PROXY;
    info.isServer = isServer;
    info.isEnabled = true;
    info.groupId = (char*)appInfo->groupId;
    info.peerSessionName = (char*)appInfo->peerData.sessionName;
    info.peerPid = appInfo->peerData.pid;
    info.peerUid = appInfo->peerData.uid;
    char buf[NETWORK_ID_BUF_LEN] = {0};
    info.sessionKey = (char*)appInfo->sessionKey;
    info.keyLen = SESSION_KEY_LENGTH;

    int32_t ret;
    if (appInfo->appType != APP_TYPE_AUTH) {
        ret = LnnGetNetworkIdByUuid(appInfo->peerData.deviceId, buf, NETWORK_ID_BUF_LEN);
        if (ret != SOFTBUS_OK) {
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get info networkId fail.");
            return SOFTBUS_ERR;
        }
        info.peerDeviceId = buf;
    } else {
        info.peerDeviceId = (char *)appInfo->peerData.deviceId;
    }

    ret = TransProxyOnChannelOpened(appInfo->myData.pkgName, appInfo->myData.sessionName, &info);
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "proxy channel open, channelId = %d, ret = %d", channelId, ret);
    return ret;
}

int32_t OnProxyChannelOpened(int32_t channelId, const AppInfo *appInfo, unsigned char isServer)
{
    int32_t ret;

    if (appInfo == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "proxy channel opened: channeld=%d, appType=%d, isServer=%d",
        channelId, appInfo->appType, isServer);

    switch (appInfo->appType) {
        case APP_TYPE_NORMAL:
        case APP_TYPE_AUTH:
            ret = NotifyNormalChannelOpened(channelId, appInfo, isServer);
            break;
        case APP_TYPE_INNER:
            ret = NotifyNetworkingChannelOpened(channelId, appInfo, isServer);
            break;
        default:
            ret = SOFTBUS_ERR;
            break;
    }
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "open ret %d", ret);
    return ret;
}

int32_t OnProxyChannelOpenFailed(int32_t channelId, const AppInfo *appInfo)
{
    int32_t ret = SOFTBUS_OK;

    if (appInfo == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "param1: channelId=%d, appType=%d",
        channelId, appInfo->appType);

    switch (appInfo->appType) {
        case APP_TYPE_NORMAL:
        case APP_TYPE_AUTH:
            ret = NotifyNormalChannelOpenFailed(appInfo->myData.pkgName, channelId);
            break;
        case APP_TYPE_INNER:
            NotifyNetworkingChannelOpenFailed(channelId, appInfo->peerData.deviceId);
            break;
        default:
            ret = SOFTBUS_ERR;
            break;
    }
    return ret;
}

int32_t OnProxyChannelClosed(int32_t channelId, const AppInfo *appInfo)
{
    if (appInfo == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "param2: channelId=%d, appType=%d", channelId, appInfo->appType);

    int32_t ret = SOFTBUS_OK;
    switch (appInfo->appType) {
        case APP_TYPE_NORMAL:
        case APP_TYPE_AUTH:
            ret = NotifyNormalChannelClosed(appInfo->myData.pkgName, channelId);
            break;
        case APP_TYPE_INNER:
            NotifyNetworkingChannelClosed(channelId);
            break;
        default:
            ret = SOFTBUS_ERR;
            break;
    }
    return ret;
}

int32_t OnProxyChannelMsgReceived(int32_t channelId, const AppInfo *appInfo,
                                  const char *data, uint32_t len)
{
    int32_t ret = SOFTBUS_OK;

    if (appInfo == NULL || data == NULL || len == 0) {
        return SOFTBUS_INVALID_PARAM;
    }

    switch (appInfo->appType) {
        case APP_TYPE_NORMAL:
            TransOnNormalMsgReceived(appInfo->myData.pkgName, channelId, data, len);
            break;
        case APP_TYPE_AUTH:
            TransOnAuthMsgReceived(appInfo->myData.pkgName, channelId, data, len);
            break;
        case APP_TYPE_INNER:
            NotifyNetworkingMsgReceived(channelId, data, len);
            break;
        default:
            ret = SOFTBUS_ERR;
            break;
    }
    return ret;
}

static int32_t TransProxyGetAppInfo(const char *sessionName, const char *peerNetworkId, AppInfo *appInfo)
{
    int ret;

    appInfo->appType = APP_TYPE_INNER;
    appInfo->myData.apiVersion = API_V2;
    ret = LnnGetLocalStrInfo(STRING_KEY_UUID, appInfo->myData.deviceId,
                             sizeof(appInfo->myData.deviceId));
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get local devid fail %d", ret);
        return SOFTBUS_ERR;
    }
    if (strcpy_s(appInfo->myData.sessionName, sizeof(appInfo->myData.sessionName), sessionName) != 0) {
        return SOFTBUS_ERR;
    }
    appInfo->peerData.apiVersion = API_V2;
    if (strcpy_s(appInfo->peerData.sessionName, sizeof(appInfo->peerData.sessionName), sessionName) != 0) {
        return SOFTBUS_ERR;
    }

    ret = LnnGetRemoteStrInfo(peerNetworkId, STRING_KEY_UUID,
                              appInfo->peerData.deviceId, sizeof(appInfo->peerData.deviceId));
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get remote node uuid err %d", ret);
        return SOFTBUS_GET_REMOTE_UUID_ERR;
    }

    return SOFTBUS_OK;
}

static int32_t TransParseConnectOption(const ConnectionAddr *connAddr, ConnectOption *connOpt)
{
    ConnectionAddrType type = connAddr->type;
    if (type == CONNECTION_ADDR_WLAN || type == CONNECTION_ADDR_ETH) {
        connOpt->type = CONNECT_TCP;
        connOpt->info.ipOption.port = (int32_t)connAddr->info.ip.port;
        if (strcpy_s(connOpt->info.ipOption.ip, sizeof(connOpt->info.ipOption.ip), connAddr->info.ip.ip) != EOK) {
            return SOFTBUS_ERR;
        }
        return SOFTBUS_OK;
    } else if (type == CONNECTION_ADDR_BR) {
        connOpt->type = CONNECT_BR;
        if (strcpy_s(connOpt->info.brOption.brMac, sizeof(connOpt->info.brOption.brMac),
            connAddr->info.br.brMac) != EOK) {
            return SOFTBUS_ERR;
        }
        return SOFTBUS_OK;
    } else {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "get conn opt err: type=%d", type);
        return SOFTBUS_ERR;
    }
}

static LnnLaneProperty TransGetLnnLaneProperty(SessionType type)
{
    switch (type) {
        case TYPE_MESSAGE:
            return LNN_MESSAGE_LANE;
        case TYPE_BYTES:
            return LNN_BYTES_LANE;
        case TYPE_FILE:
            return LNN_FILE_LANE;
        case TYPE_STREAM:
            return LNN_STREAM_LANE;
        default:
            return LNN_LANE_PROPERTY_BUTT;
    }
}

static int32_t TransGetLaneInfo(SessionType flags, const char *peerDeviceId, int32_t pid,
    LnnLanesObject **lanesObject, const LnnLaneInfo **laneInfo)
{
    LnnLaneProperty laneProperty = TransGetLnnLaneProperty(flags);
    if (laneProperty == LNN_LANE_PROPERTY_BUTT) {
        return SOFTBUS_TRANS_GET_LANE_INFO_ERR;
    }
    LnnLanesObject *object = LnnRequestLanesObject(peerDeviceId, pid, laneProperty, NULL, 1);
    if (object == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "get lne obj err");
        return SOFTBUS_TRANS_GET_LANE_INFO_ERR;
    }
    int32_t laneIndex = 0;
    int32_t laneId = LnnGetLaneId(object, laneIndex);
    const LnnLaneInfo *info = LnnGetLaneInfo(laneId);
    if (info == NULL) {
        LnnReleaseLanesObject(object);
        return SOFTBUS_TRANS_GET_LANE_INFO_ERR;
    }

    *lanesObject = object;
    *laneInfo = info;
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "get lane info ok: flags=%d", flags);
    return SOFTBUS_OK;
}


static int32_t TransGetConnectOption(const char *peerNetworkId, ConnectOption *connOpt)
{
    LnnLanesObject *object = NULL;
    const LnnLaneInfo *info = NULL;
#define DEFAULT_PID 0
    if (TransGetLaneInfo(TYPE_MESSAGE, peerNetworkId, DEFAULT_PID, &object, &info) != SOFTBUS_OK) {
        goto EXIT_ERR;
    }
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "net Channel lane info: udp=%d, proxy=%d, type=%d",
        info->isSupportUdp, info->isProxy, info->conOption.type);
    if (TransParseConnectOption(&info->conOption, connOpt) != SOFTBUS_OK) {
        goto EXIT_ERR;
    }
    LnnReleaseLanesObject(object);
    return SOFTBUS_OK;
 EXIT_ERR:
    if (object != NULL) {
        LnnReleaseLanesObject(object);
    }
    return SOFTBUS_TRANS_GET_LANE_INFO_ERR;
}

int32_t TransOpenNetWorkingChannel(const char *sessionName, const char *peerNetworkId)
{
    AppInfo appInfo;
    ConnectOption connOpt;
    int32_t channelId = INVALID_CHANNEL_ID;

    if (!IsValidString(sessionName, SESSION_NAME_SIZE_MAX) ||
        !IsValidString(peerNetworkId, DEVICE_ID_SIZE_MAX)) {
        return channelId;
    }
    if (TransGetConnectOption(peerNetworkId, &connOpt) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "networking get connect option fail");
        return channelId;
    }
    (void)memset_s(&appInfo, sizeof(AppInfo), 0, sizeof(AppInfo));
    if (TransProxyGetAppInfo(sessionName, peerNetworkId, &appInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "networking get app info fail");
        return channelId;
    }

    if (TransProxyOpenProxyChannel(&appInfo, &connOpt, &channelId) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "networking open channel fail");
        channelId = INVALID_CHANNEL_ID;
    }
    return channelId;
}

int32_t TransSendNetworkingMessage(int32_t channelId, const char *data, uint32_t dataLen, int32_t priority)
{
    return TransProxySendMsg(channelId, data, dataLen, priority);
}

int32_t TransCloseNetWorkingChannel(int32_t channelId)
{
    return TransProxyCloseProxyChannel(channelId);
}
