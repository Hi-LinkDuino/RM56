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

#include <stdio.h>

#include "client_trans_udp_stream_interface.h"
#include "session.h"

#define CHANNELID 1
#define CHANNELID2 2
#define PKGNAME   "test"
#define LONG_SLEEP    600

void SetStatus(int channelId, int status)
{
    printf("[server]:channelID:%d, status:%d\n", channelId, status);
}

void OnStreamReceived(int channelId, const StreamData *data, const StreamData *ext, const StreamFrameInfo *param)
{
    printf("[server]:OnStreamReceived, len:%d, extLen:%d\n", data->bufLen, data->bufLen);
    printf("[server]:channelID:%d, streamBuf:%.*s\n", channelId, data->bufLen, data->buf);

    StreamData tmpData = {
        "peipeipei\0",
        10,
    };
    StreamFrameInfo tmpf = {};
    int ret = SendVtpStream(channelId, &tmpData, NULL, &tmpf);
    printf("[server]:DstreamSendStream ret:%d\n", ret);
}

static IStreamListener g_callback = {
    .OnStatusChange = SetStatus,
    .OnStreamReceived = OnStreamReceived,
};

int main()
{
    int ret;

    VtpStreamOpenParam p1 = {
        PKGNAME,
        "127.0.0.1",
        NULL,
        -1,
        RAW_STREAM,
        "abcdefghabcdefghabcdefghabcdefgh",
    };

    VtpStreamOpenParam p2 = {
        PKGNAME,
        "127.0.0.1",
        NULL,
        -1,
        RAW_STREAM,
        "abcdefghabcdefghabcdefghabcdefgh",
    };

    ret = StartVtpStreamChannelServer(CHANNELID, &p1, &g_callback);
    printf("[server]:StartChannelServer ret:%d\n", ret);

    ret = StartVtpStreamChannelServer(CHANNELID2, &p2, &g_callback);
    printf("[server]:StartChannelServer2 ret:%d\n", ret);

    while (1) {
        printf("[server]:server is running.\n");
        sleep(LONG_SLEEP);
    }

    CloseVtpStreamChannel(CHANNELID, PKGNAME);
    CloseVtpStreamChannel(CHANNELID2, PKGNAME);

    return 0;
}
