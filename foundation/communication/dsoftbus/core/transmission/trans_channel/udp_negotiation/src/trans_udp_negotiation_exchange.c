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

#include "trans_udp_negotiation_exchange.h"

#include "softbus_adapter_crypto.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_json_utils.h"
#include "softbus_log.h"

#define BASE64_SESSION_KEY_LEN 45
typedef enum {
    CODE_EXCHANGE_UDP_INFO = 6,
} CodeType;

int32_t TransUnpackReplyUdpInfo(const cJSON *msg, AppInfo *appInfo)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "unpack reply udp info in negotiation.");
    if (msg == NULL || appInfo == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid param.");
        return SOFTBUS_ERR;
    }

    (void)GetJsonObjectStringItem(msg, "PKG_NAME", appInfo->peerData.pkgName, PKG_NAME_SIZE_MAX);
    (void)GetJsonObjectNumberItem(msg, "UID", &(appInfo->peerData.uid));
    (void)GetJsonObjectNumberItem(msg, "PID", &(appInfo->peerData.pid));
    (void)GetJsonObjectNumberItem(msg, "BUSINESS_TYPE", (int*)&(appInfo->businessType));

    switch (appInfo->udpChannelOptType) {
        case TYPE_UDP_CHANNEL_OPEN:
            (void)GetJsonObjectNumber64Item(msg, "MY_CHANNEL_ID", &(appInfo->peerData.channelId));
            (void)GetJsonObjectNumberItem(msg, "MY_PORT", &(appInfo->peerData.port));
            (void)GetJsonObjectStringItem(msg, "MY_IP", appInfo->peerData.ip, IP_LEN);
            break;
        case TYPE_UDP_CHANNEL_CLOSE:
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid udp channel type.");
            return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t TransUnpackRequestUdpInfo(const cJSON *msg, AppInfo *appInfo)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "unpack request udp info in negotiation.");
    if (msg == NULL || appInfo == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid param.");
        return SOFTBUS_ERR;
    }
    unsigned char encodeSessionKey[BASE64_SESSION_KEY_LEN] = {0};
    size_t len = 0;
    (void)GetJsonObjectStringItem(msg, "SESSION_KEY", (char*)encodeSessionKey, BASE64_SESSION_KEY_LEN);
    int32_t ret = SoftBusBase64Decode((unsigned char*)appInfo->sessionKey, sizeof(appInfo->sessionKey), &len,
        (unsigned char*)encodeSessionKey, strlen((char*)encodeSessionKey));
    if (len != sizeof(appInfo->sessionKey) || ret != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "mbedtls base64 decode failed.");
        return SOFTBUS_ERR;
    }

    (void)GetJsonObjectStringItem(msg, "PKG_NAME", appInfo->peerData.pkgName, PKG_NAME_SIZE_MAX);
    (void)GetJsonObjectStringItem(msg, "BUS_NAME", appInfo->myData.sessionName, SESSION_NAME_SIZE_MAX);
    (void)GetJsonObjectStringItem(msg, "CLIENT_BUS_NAME", appInfo->peerData.sessionName, SESSION_NAME_SIZE_MAX);
    (void)GetJsonObjectStringItem(msg, "GROUP_ID", appInfo->groupId, GROUP_ID_SIZE_MAX);

    (void)GetJsonObjectNumberItem(msg, "API_VERSION", (int*)&(appInfo->peerData.apiVersion));
    (void)GetJsonObjectNumberItem(msg, "PID", &(appInfo->peerData.pid));
    (void)GetJsonObjectNumberItem(msg, "UID", &(appInfo->peerData.uid));
    (void)GetJsonObjectNumberItem(msg, "BUSINESS_TYPE", (int*)&(appInfo->businessType));
    (void)GetJsonObjectNumberItem(msg, "STREAM_TYPE", (int*)&(appInfo->streamType));
    (void)GetJsonObjectNumberItem(msg, "CHANNEL_TYPE", &(appInfo->udpChannelOptType));
    (void)GetJsonObjectNumberItem(msg, "UDP_CONN_TYPE", &(appInfo->udpConnType));

    switch (appInfo->udpChannelOptType) {
        case TYPE_UDP_CHANNEL_OPEN:
            (void)GetJsonObjectNumber64Item(msg, "MY_CHANNEL_ID", &(appInfo->peerData.channelId));
            (void)GetJsonObjectStringItem(msg, "MY_IP", appInfo->peerData.ip, IP_LEN);
            break;
        case TYPE_UDP_CHANNEL_CLOSE:
            (void)GetJsonObjectNumber64Item(msg, "PEER_CHANNEL_ID", &(appInfo->myData.channelId));
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid udp channel type.");
            return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

int32_t TransPackRequestUdpInfo(cJSON *msg, const AppInfo *appInfo)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "pack request udp info in negotiation.");
    if (msg == NULL || appInfo == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid param.");
        return SOFTBUS_ERR;
    }

    switch (appInfo->udpChannelOptType) {
        case TYPE_UDP_CHANNEL_OPEN:
            (void)AddNumber64ToJsonObject(msg, "MY_CHANNEL_ID", appInfo->myData.channelId);
            (void)AddStringToJsonObject(msg, "MY_IP", appInfo->myData.ip);
            break;
        case TYPE_UDP_CHANNEL_CLOSE:
            (void)AddNumber64ToJsonObject(msg, "PEER_CHANNEL_ID", appInfo->peerData.channelId);
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid udp channel type.");
            return SOFTBUS_ERR;
    }
    char encodeSessionKey[BASE64_SESSION_KEY_LEN] = {0};
    size_t len = 0;
    int32_t ret = SoftBusBase64Encode((unsigned char*)encodeSessionKey, BASE64_SESSION_KEY_LEN, &len,
        (unsigned char*)appInfo->sessionKey, sizeof(appInfo->sessionKey));
    if (ret != 0) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "mbedtls base64 encode failed.");
        return SOFTBUS_ERR;
    }
    (void)AddStringToJsonObject(msg, "SESSION_KEY", encodeSessionKey);

    (void)AddNumberToJsonObject(msg, "CODE", CODE_EXCHANGE_UDP_INFO);
    (void)AddNumberToJsonObject(msg, "API_VERSION", appInfo->myData.apiVersion);
    (void)AddNumberToJsonObject(msg, "UID", appInfo->myData.uid);
    (void)AddNumberToJsonObject(msg, "PID", appInfo->myData.pid);
    (void)AddNumberToJsonObject(msg, "BUSINESS_TYPE", appInfo->businessType);
    (void)AddNumberToJsonObject(msg, "STREAM_TYPE", appInfo->streamType);
    (void)AddNumberToJsonObject(msg, "CHANNEL_TYPE", appInfo->udpChannelOptType);
    (void)AddNumberToJsonObject(msg, "UDP_CONN_TYPE", appInfo->udpConnType);

    (void)AddStringToJsonObject(msg, "BUS_NAME", appInfo->peerData.sessionName);
    (void)AddStringToJsonObject(msg, "CLIENT_BUS_NAME", appInfo->myData.sessionName);
    (void)AddStringToJsonObject(msg, "GROUP_ID", appInfo->groupId);
    (void)AddStringToJsonObject(msg, "PKG_NAME", appInfo->myData.pkgName);

    return SOFTBUS_OK;
}

int32_t TransPackReplyUdpInfo(cJSON *msg, const AppInfo *appInfo)
{
    SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_INFO, "pack reply udp info in negotiation.");
    if (msg == NULL || appInfo == NULL) {
        SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid param.");
        return SOFTBUS_ERR;
    }

    switch (appInfo->udpChannelOptType) {
        case TYPE_UDP_CHANNEL_OPEN:
            (void)AddNumber64ToJsonObject(msg, "MY_CHANNEL_ID", appInfo->myData.channelId);
            (void)AddNumberToJsonObject(msg, "MY_PORT", appInfo->myData.port);
            (void)AddStringToJsonObject(msg, "MY_IP", appInfo->myData.ip);
            break;
        case TYPE_UDP_CHANNEL_CLOSE:
            break;
        default:
            SoftBusLog(SOFTBUS_LOG_TRAN, SOFTBUS_LOG_ERROR, "invalid udp channel type.");
            return SOFTBUS_ERR;
    }

    (void)AddNumberToJsonObject(msg, "CODE", CODE_EXCHANGE_UDP_INFO);
    (void)AddStringToJsonObject(msg, "PKG_NAME", appInfo->myData.pkgName);
    (void)AddNumberToJsonObject(msg, "UID", appInfo->myData.uid);
    (void)AddNumberToJsonObject(msg, "PID", appInfo->myData.pid);
    (void)AddNumberToJsonObject(msg, "BUSINESS_TYPE", appInfo->businessType);

    return SOFTBUS_OK;
}