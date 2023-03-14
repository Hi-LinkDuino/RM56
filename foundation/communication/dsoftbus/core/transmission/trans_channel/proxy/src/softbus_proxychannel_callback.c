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

#include "softbus_proxychannel_callback.h"

#include "softbus_def.h"
#include "softbus_errcode.h"

static IServerChannelCallBack g_channelCb;

int32_t TransProxySetCallBack(const IServerChannelCallBack *cb)
{
    if (cb == NULL) {
        return SOFTBUS_INVALID_PARAM;
    }
    g_channelCb = *cb;
    return SOFTBUS_OK;
}

int32_t TransProxyOnChannelOpened(const char *pkgName, const char *sessionName,
    const ChannelInfo *channel)
{
    return g_channelCb.OnChannelOpened(pkgName, sessionName, channel);
}

int32_t TransProxyOnChannelClosed(const char *pkgName, int32_t channelId)
{
    return g_channelCb.OnChannelClosed(pkgName, channelId, CHANNEL_TYPE_PROXY);
}

int32_t TransProxyOnChannelOpenFailed(const char *pkgName, int32_t channelId)
{
    return g_channelCb.OnChannelOpenFailed(pkgName, channelId, CHANNEL_TYPE_PROXY);
}

int32_t TransProxyOnMsgReceived(const char *pkgName, int32_t channelId,
    const void *data, uint32_t len, int32_t type)
{
    return g_channelCb.OnDataReceived(pkgName, channelId, CHANNEL_TYPE_PROXY, data, len, type);
}

int32_t TransProxyGetPkgName(const char *sessionName, char *pkgName, uint16_t len)
{
    return g_channelCb.GetPkgNameBySessionName(sessionName, pkgName, len);
}

