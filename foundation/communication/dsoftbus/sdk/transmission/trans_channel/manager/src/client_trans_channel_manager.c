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

#include "client_trans_channel_manager.h"

#include "client_trans_auth_manager.h"
#include "client_trans_proxy_manager.h"
#include "client_trans_session_callback.h"
#include "client_trans_tcp_direct_manager.h"
#include "client_trans_tcp_direct_message.h"
#include "client_trans_udp_manager.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

int32_t ClientTransChannelInit(void)
{
    IClientSessionCallBack *cb = GetClientSessionCb();
    if (cb == NULL) {
        return SOFTBUS_ERR;
    }
    if (TransTdcManagerInit(cb) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "trans tcp direct manager init failed.");
        return SOFTBUS_ERR;
    }
    if (ClientTransAuthInit(cb) != SOFTBUS_OK) {
        return SOFTBUS_ERR;
    }
    if (ClinetTransProxyInit(cb) != SOFTBUS_OK) {
        return SOFTBUS_ERR;
    }
    if (ClientTransUdpMgrInit(cb) != SOFTBUS_OK) {
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

void ClientTransChannelDeinit(void)
{
    TransTdcManagerDeinit();
    ClientTransUdpMgrDeinit();
    ClientTransProxyDeinit();
}

int32_t ClientTransCloseChannel(int32_t channelId, int32_t type)
{
    if (channelId < 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }
    int32_t ret = SOFTBUS_OK;
    switch (type) {
        case CHANNEL_TYPE_PROXY:
            ClientTransProxyCloseChannel(channelId);
            break;
        case CHANNEL_TYPE_TCP_DIRECT:
            TransDelDataBufNode(channelId);
            TransTdcCloseChannel(channelId);
            break;
        case CHANNEL_TYPE_UDP:
            ret = ClientTransCloseUdpChannel(channelId);
            break;
        case CHANNEL_TYPE_AUTH:
            ClientTransAuthCloseChannel(channelId);
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid type");
            return SOFTBUS_TRANS_INVALID_CHANNEL_TYPE;
    }
    return ret;
}

int32_t ClientTransChannelSendBytes(int32_t channelId, int32_t type, const void *data, uint32_t len)
{
    if ((data == NULL) || (len == 0)) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t ret = SOFTBUS_OK;
    switch (type) {
        case CHANNEL_TYPE_AUTH:
            ret = TransAuthChannelSendBytes(channelId, data, len);
            break;
        case CHANNEL_TYPE_PROXY:
            ret = TransProxyChannelSendBytes(channelId, data, len);
            break;
        case CHANNEL_TYPE_TCP_DIRECT:
            ret = TransTdcSendBytes(channelId, data, len);
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid type");
            return SOFTBUS_ERR;
    }
    return ret;
}

int32_t ClientTransChannelSendMessage(int32_t channelId, int32_t type, const void *data, uint32_t len)
{
    if ((data == NULL) || (len == 0)) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t ret = SOFTBUS_OK;
    switch (type) {
        case CHANNEL_TYPE_AUTH:
            ret = TransAuthChannelSendMessage(channelId, data, len);
            break;
        case CHANNEL_TYPE_PROXY:
            ret = TransProxyChannelSendMessage(channelId, data, len);
            break;
        case CHANNEL_TYPE_TCP_DIRECT:
            ret = TransTdcSendMessage(channelId, data, len);
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid type");
            return SOFTBUS_ERR;
    }
    return ret;
}

int32_t ClientTransChannelSendStream(int32_t channelId, int32_t type, const StreamData *data,
    const StreamData *ext, const StreamFrameInfo *param)
{
    if ((data == NULL) || (ext == NULL) || (param == NULL)) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "Invalid param");
        return SOFTBUS_INVALID_PARAM;
    }

    int32_t ret = SOFTBUS_OK;
    switch (type) {
        case CHANNEL_TYPE_UDP:
            ret = TransUdpChannelSendStream(channelId, data, ext, param);
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "unsupport channel type");
            return SOFTBUS_ERR;
    }
    return ret;
}

int32_t ClientTransChannelSendFile(int32_t channelId, int32_t type, const char *sFileList[],
    const char *dFileList[], uint32_t fileCnt)
{
    int32_t ret = SOFTBUS_OK;
    switch (type) {
        case CHANNEL_TYPE_UDP:
            ret = TransUdpChannelSendFile(channelId, sFileList, dFileList, fileCnt);
            break;
        case CHANNEL_TYPE_PROXY:
            ret = TransProxyChannelSendFile(channelId, sFileList, dFileList, fileCnt);
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "unsupport channel type");
            return SOFTBUS_ERR;
    }
    return ret;
}

void DeleteFileListener(const char *sessionName)
{
    return TransUdpDeleteFileListener(sessionName);
}

int32_t ClientGetSessionKey(int32_t channelId, char *key, unsigned int len)
{
    return TransTdcGetSessionKey(channelId, key, len);
}

int32_t ClientGetHandle(int32_t channelId, int *handle)
{
    return TransTdcGetHandle(channelId, handle);
}

int32_t ClientDisableSessionListener(int32_t channelId)
{
    return TransDisableSessionListener(channelId);
}