/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
 *
 */

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_REGISTER_HDC_JDWP_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_REGISTER_HDC_JDWP_H

#include "frameworks/core/common/register/define.h"

namespace OHOS::Ace {
class HdcJdwpSimulator;

class HdcJdwpSimulator {
public:
    explicit HdcJdwpSimulator(uv_loop_t *loopIn, string pkgName);
    ~HdcJdwpSimulator();
    bool Connect();
    uv_loop_t *loop_;

protected:
    struct ContextJdwpSimulator {
        uv_pipe_t pipe;
        uv_tcp_t newFd;
        bool hasNewFd;
        HdcJdwpSimulator *thisClass;
    };
    using HCtxJdwpSimulator = struct ContextJdwpSimulator *;

private:
    struct JsMsgHeader {
        uint32_t msgLen;
        uint32_t pid;
    };
    void *MallocContext();
    static void ConnectJdwp(uv_connect_t *connection, int status);
    static void FinishWriteCallback(uv_write_t *req, int status);
#ifndef JS_JDWP_CONNECT
    static void ReceiveNewFd(uv_stream_t *q, ssize_t nread, const uv_buf_t *buf);
    static void ProcessIncoming(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);
#endif // JS_JDWP_CONNECT
    static RetErrCode SendToStream(uv_stream_t *handleStream, const uint8_t *buf, const int bufLen,
                                   const void *finishCallback);
    bool exit_ = false;
    HCtxJdwpSimulator ctxPoint_;
    string pkgName_;
    uv_connect_t *connect_;
};
} // namespace OHOS::Ace
#endif  // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_REGISTER_HDC_JDWP_H
