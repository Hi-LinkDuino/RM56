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

#include "client_trans_auth_manager.h"

#include "softbus_errcode.h"
#include "softbus_log.h"
#include "trans_server_proxy.h"

static IClientSessionCallBack g_sessionCb;

int32_t ClientTransAuthInit(const IClientSessionCallBack *cb)
{
    if (cb == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }
    g_sessionCb = *cb;
    return SOFTBUS_OK;
}

int32_t ClientTransAuthOnChannelOpened(const char *sessionName, const ChannelInfo *channel)
{
    if (sessionName == NULL || channel == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }

    int ret = g_sessionCb.OnSessionOpened(sessionName, channel, TYPE_MESSAGE);
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "notify session open fail");
        return ret;
    }

    return SOFTBUS_OK;
}

int32_t ClientTransAuthOnChannelClosed(int32_t channelId)
{
    int ret = g_sessionCb.OnSessionClosed(channelId, CHANNEL_TYPE_AUTH);
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "notify session openfail err");
        return ret;
    }
    return SOFTBUS_OK;
}

int32_t ClientTransAuthOnChannelOpenFailed(int32_t channelId)
{
    int ret = g_sessionCb.OnSessionOpenFailed(channelId, CHANNEL_TYPE_AUTH);
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "notify session openfail err");
        return ret;
    }

    return SOFTBUS_OK;
}

int32_t ClientTransAuthOnDataReceived(int32_t channelId,
    const void *data, uint32_t len, SessionPktType type)
{
    if (data == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }
    int ret = g_sessionCb.OnDataReceived(channelId, CHANNEL_TYPE_AUTH, data, len, type);
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "notify data recv err");
        return ret;
    }
    return SOFTBUS_OK;
}

void ClientTransAuthCloseChannel(int32_t channelId)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "TransCloseAuthChannel, channelId [%d]", channelId);
    if (ServerIpcCloseChannel(channelId, CHANNEL_TYPE_AUTH) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "server close channel err");
    }
    if (ClientTransAuthOnChannelClosed(channelId) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "server close channel err");
    }
}

int32_t TransAuthChannelSendBytes(int32_t channelId, const void *data, uint32_t len)
{
    int ret = ServerIpcSendMessage(channelId, CHANNEL_TYPE_AUTH, data, len, TRANS_SESSION_BYTES);
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "send bytes: channelId=%d, ret=%d", channelId, ret);
    return ret;
}

int32_t TransAuthChannelSendMessage(int32_t channelId, const void *data, uint32_t len)
{
    int ret = ServerIpcSendMessage(channelId, CHANNEL_TYPE_AUTH, data, len, TRANS_SESSION_BYTES);
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "send msg: channelId=%d, ret=%d", channelId, ret);
    return ret;
}