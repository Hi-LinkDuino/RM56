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

#include "frameworks/core/common/debugger/unix_socket.h"

#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "base/log/log.h"
#include "securec.h"

namespace OHOS::Ace {
static constexpr uint32_t SLEEP_TIME_MS = 500U;
static constexpr int CONTROL_SOCK_SEND_TIMEOUT = 10U;

int32_t UnixSocketClient::UnixSocketConn()
{
    uint32_t sleep_ms = SLEEP_TIME_MS;
    const uint32_t SLEEP_MAX_MS = 4 * SLEEP_TIME_MS;
    control_sock = TEMP_FAILURE_RETRY(socket(AF_UNIX, SOCK_STREAM, 0));
    if (control_sock < 0) {
        LOGE("Could not create control socket");
        return FAIL_CAUSE_SOCKET_NO_CLIENT;
    }
    errno_t errRet = memset_s(&controlAddrUn, sizeof(controlAddrUn), 0, sizeof(controlAddrUn));
    if (errRet != EOK) {
        LOGE("Socket memset_s fail");
        return FAIL_CAUSE_SOCKET_NO_CLIENT;
    }
    controlAddrUn.sun_family = AF_UNIX;
    errno_t errNum = memcpy_s(controlAddrUn.sun_path, JDWP_CONTROL_NAME_LEN, JDWP_CONTROL_NAME, JDWP_CONTROL_NAME_LEN);
    if (errNum != EOK) {
        LOGE("Socket memcpy_s fail");
        return FAIL_CAUSE_SOCKET_NO_CLIENT;
    }
    control_addr_len = sizeof(controlAddrUn.sun_family) + JDWP_CONTROL_NAME_LEN;
    int keepalive = 1;
    setsockopt(control_sock, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof(keepalive));
    while (true) {
        int ret = TEMP_FAILURE_RETRY(connect(control_sock, (struct sockaddr *)&controlAddrUn, control_addr_len));
        if (ret >= 0) {
            LOGI("Connect Successful");
            return SOCKET_SUCCESS;
        }
        const int sleep_times = 2;
        usleep(sleep_ms * SLEEP_TIME_MS * sleep_times);
        sleep_ms += (sleep_ms >> 1);
        if (sleep_ms > SLEEP_MAX_MS) {
            sleep_ms = SLEEP_MAX_MS;
        }
        return FAIL_CAUSE_SOCKET_COMMON_FAIL;
    }
    return FAIL_CAUSE_SOCKET_COMMON_FAIL;
}

int32_t UnixSocketClient::SendMessage(int32_t pid)
{
    std::string pidStr = std::to_string(pid);
    int pidSize = pidStr.size();
    int buffSize = pidSize + 1;
    char buff[buffSize];
    if (control_sock < 0) {
        LOGE("Error Occur!");
        return FAIL_CAUSE_SEND_MSG_FAIL;
    }
    errno_t errRet = strncpy_s(buff, buffSize, pidStr.c_str(), pidSize);
    if (errRet != EOK) {
        LOGE("Send Message fail");
        return FAIL_CAUSE_SEND_MSG_FAIL;
    }
    struct timeval timeout {
    };
    timeout.tv_sec = CONTROL_SOCK_SEND_TIMEOUT;
    timeout.tv_usec = 0;
    setsockopt(control_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    int ret = TEMP_FAILURE_RETRY(send(control_sock, buff, pidSize, 0));
    if (ret >= 0) {
        LOGI("PID sent as '%s' to HDC", buff);
        return SOCKET_SUCCESS;
    }
    LOGE("Weird, can't send JDWP process pid to HDC");
    return FAIL_CAUSE_SEND_MSG_FAIL;
}

void UnixSocketClient::UnixSocketClose()
{
    close(control_sock);
}

} // namespace OHOS::Ace

