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

#include "client_trans_stream.h"
#include "softbus_errcode.h"

void RegisterStreamCb(const UdpChannelMgrCb *cb)
{
    (void)cb;
}

void UnregisterStreamCb(void)
{
    return;
}

int32_t TransOnstreamChannelOpened(const ChannelInfo *channel, int32_t *streamPort)
{
    (void)channel;
    (void)streamPort;
    return SOFTBUS_OK;
}

int32_t TransSendStream(int32_t channelId, const StreamData *data,
    const StreamData *ext, const StreamFrameInfo *param)
{
    (void)channelId;
    (void)data;
    (void)ext;
    (void)param;
    return SOFTBUS_OK;
}

int32_t TransCloseStreamChannel(int32_t channelId)
{
    (void)channelId;
    return SOFTBUS_OK;
}
