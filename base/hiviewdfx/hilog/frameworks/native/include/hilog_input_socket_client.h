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

#ifndef HILOG_INPUT_SOCKET_CLIENT_H
#define HILOG_INPUT_SOCKET_CLIENT_H

#include "hilog_common.h"
#include "dgram_socket_client.h"

namespace OHOS {
namespace HiviewDFX {
class HilogInputSocketClient : DgramSocketClient {
public:
    HilogInputSocketClient() : DgramSocketClient(INPUT_SOCKET_NAME, SOCK_NONBLOCK | SOCK_CLOEXEC) {}
    int WriteLogMessage(HilogMsg *header, const char *tag, uint16_t tagLen, const char *fmt, uint16_t fmtLen);
    ~HilogInputSocketClient() = default;
};
} // namespace HiviewDFX
} // namespace OHOS

extern "C" int HilogWriteLogMessage(HilogMsg *header, const char *tag, uint16_t tagLen, const char *fmt,
    uint16_t fmtLen);
#endif /* HILOG_INPUT_SOCKET_CLIENT_H */
