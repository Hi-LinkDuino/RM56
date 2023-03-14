/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "channel_manager.h"

#include "callback_manager.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "soft_bus_channel.h"

static bool g_initialized = false;

int32_t InitChannelManager(void)
{
    if (g_initialized || !IsSoftBusChannelSupported()) {
        return HC_SUCCESS;
    }
    int32_t res = InitSoftBusChannelModule();
    if (res == HC_SUCCESS) {
        g_initialized = true;
    }
    return res;
}

void DestroyChannelManager(void)
{
    if (g_initialized && IsSoftBusChannelSupported()) {
        DestroySoftBusChannelModule();
        g_initialized = false;
    }
}

ChannelType GetChannelType(const DeviceAuthCallback *callback, const CJson *jsonParams)
{
    if (IsSoftBusChannelSupported()) {
        const char *connectParams = GetStringFromJson(jsonParams, FIELD_CONNECT_PARAMS);
        if (connectParams != NULL) {
            return SOFT_BUS;
        }
    }
    if ((callback != NULL) && (callback->onTransmit != NULL)) {
        return SERVICE_CHANNEL;
    }
    return NO_CHANNEL;
}

int32_t OpenChannel(ChannelType channelType, const CJson *jsonParams, int64_t requestId, int64_t *returnChannelId)
{
    if (channelType == SERVICE_CHANNEL) {
        *returnChannelId = DEFAULT_CHANNEL_ID;
        return HC_SUCCESS;
    } else if (channelType == SOFT_BUS) {
        const char *connectParams = GetStringFromJson(jsonParams, FIELD_CONNECT_PARAMS);
        if (connectParams == NULL) {
            LOGE("Failed to get connectParams from jsonParams!");
            return HC_ERR_JSON_GET;
        }
        int64_t channelId = DEFAULT_CHANNEL_ID;
        int32_t result = GetSoftBusInstance()->openChannel(connectParams, requestId, &channelId);
        if (result != HC_SUCCESS) {
            return HC_ERR_CHANNEL_NOT_EXIST;
        }
        *returnChannelId = channelId;
        return HC_SUCCESS;
    } else {
        return HC_ERR_CHANNEL_NOT_EXIST;
    }
}

void CloseChannel(ChannelType channelType, int64_t channelId)
{
    if (channelType == SOFT_BUS) {
        GetSoftBusInstance()->closeChannel(channelId);
    }
}

int32_t HcSendMsg(ChannelType channelType, int64_t requestId, int64_t channelId,
    const DeviceAuthCallback *callback, const char *data)
{
    if (channelType == SERVICE_CHANNEL) {
        if (ProcessTransmitCallback(requestId, (uint8_t *)data, HcStrlen(data) + 1, callback)) {
            return HC_SUCCESS;
        }
        return HC_ERR_TRANSMIT_FAIL;
    } else if (channelType == SOFT_BUS) {
        return GetSoftBusInstance()->sendMsg(channelId, (uint8_t *)data, HcStrlen(data) + 1);
    } else {
        return HC_ERR_CHANNEL_NOT_EXIST;
    }
}

void NotifyBindResult(ChannelType channelType, int64_t channelId)
{
    if (channelType == SOFT_BUS) {
        GetSoftBusInstance()->notifyResult(channelId);
    }
}
