/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_UNIX_SOCKET_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_UNIX_SOCKET_H
#define FAIL_CAUSE_SOCKET_NO_CLIENT (-1003)
#define FAIL_CAUSE_SOCKET_COMMON_FAIL (-1004)
#define FAIL_CAUSE_SEND_MSG_FAIL (-1005)
#define JDWP_CONTROL_NAME    "\0jdwp-control"
#define JDWP_CONTROL_NAME_LEN (sizeof(JDWP_CONTROL_NAME)-1)
#define SOCKET_SUCCESS 0

#include <sys/un.h>
namespace OHOS::Ace {
class UnixSocketClient {
public:
    UnixSocketClient() = default;
    ~UnixSocketClient() {}
    int32_t UnixSocketConn();
    int32_t SendMessage(int32_t pid);
    void UnixSocketClose();
private:
    int32_t control_sock = -1;
    struct sockaddr_un controlAddrUn;
    int32_t control_addr_len;
};

} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_DEBUGGER_UNIX_SOCKET_H
