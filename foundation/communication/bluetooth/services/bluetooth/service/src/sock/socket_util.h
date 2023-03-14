/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SOCKET_UTIL_H
#define SOCKET_UTIL_H

#include <stdint.h>

namespace OHOS {
namespace bluetooth {
/**
 * @brief This SocketUtil class provides a set of methods that send
 *        message to app.
 */
class SocketUtil {
public:
    /**
     * @brief Send data to the upper app through fd.
     *
     * @param sock_fd socket
     * @param buf data buffer.
     * @param len data buffer length.
     * @return int
     */
    static int SocketSendData(int sockFd, const uint8_t *buf, int len);

    /**
     * @brief Send accept socket fd to the upper app through fd.
     *
     * @param sock_fd socket.
     * @param buf data buffer.
     * @param len data buffer length.
     * @param int server accept fd.
     * @return int
     */
    static int SocketSendFd(int sockFd, const uint8_t *buf, int len, int acceptFd);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // SOCKET_UTIL_H