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

#include "hilog_input_socket_client.h"

#include <sys/time.h>
#include <cstring>
#include <iostream>

namespace OHOS {
namespace HiviewDFX {
static HilogInputSocketClient g_hilogInputSocketClient;
extern "C" int HilogWriteLogMessage(HilogMsg *header, const char *tag, uint16_t tagLen, const char *fmt,
    uint16_t fmtLen)
{
    return g_hilogInputSocketClient.WriteLogMessage(header, tag, tagLen, fmt, fmtLen);
}

int HilogInputSocketClient::WriteLogMessage(HilogMsg *header, const char *tag, uint16_t tagLen, const char *fmt,
    uint16_t fmtLen)
{
    int ret = CheckSocket();
    if (ret < 0) {
        return ret;
    }

    struct timeval tv = {0};
    gettimeofday(&tv, nullptr);
    header->tv_sec = static_cast<uint32_t>(tv.tv_sec);
    header->tv_nsec = static_cast<uint32_t>(tv.tv_usec * 1000);     // 1000 : usec convert to nsec
    header->len = sizeof(HilogMsg) + tagLen + fmtLen;
    header->tag_len = tagLen;

    iovec vec[3];
    vec[0].iov_base = header;                                              // 0 : index of hos log header
    vec[0].iov_len = sizeof(HilogMsg);                                     // 0 : index of hos log header
    vec[1].iov_base = reinterpret_cast<void*>(const_cast<char*>(tag));     // 1 : index of log tag
    vec[1].iov_len = tagLen;                                               // 1 : index of log tag
    vec[2].iov_base = reinterpret_cast<void*>(const_cast<char*>(fmt));     // 2 : index of log content
    vec[2].iov_len = fmtLen;                                               // 2 : index of log content
    ret = WriteV(vec, 3);                                                  // 3 : written size of vector
    if (ret < 0) {
        Connect();
        ret = WriteV(vec, 3);                // 3 : written size of vector
    }

    return ret;
}
} // namespace HiviewDFX
} // namespace OHOS
