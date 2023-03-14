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

#ifndef SOCKET_LISTENER_H
#define SOCKET_LISTENER_H

#include <map>
#include <mutex>
#include <thread>
#include "sock/socket.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief This class is used to create a listener thread.
 */
class SocketThread final {
public:
    static SocketThread &GetInstance();

    using SocketOptionCallback = void (*)(Socket &sock);
    /**
     * @brief Initialize a socket thread.
     * @details Initialize variables,create socketpair,and add cmd fd to poll.
     * @return Return int type, greater than 0 success, less than 0 failure.
     */
    bool Initialize();
    /**
     * @brief Exit a socket thread.
     * @return bool
     */
    bool Uninitialize();
    /**
     * @brief Add fd and register callback.
     * @details Add FD to the thread and register the desired listening state.
     *          (read, write, exception), and return when the state changes.
     * @param fd Socket fd.
     * @param flag Listen event.
     * @param sock Socket object.
     * @return bool
     */
    bool AddSocket(int fd, int flag, Socket &sock);
    /**
     * @brief Delete fd from epoll.
     * @param sock Socket object.
     * @return bool
     */
    bool DeleteSocket(Socket &sock);

private:
    static const int MAX_EPOLL = 66;
    SocketThread() = default;
    ~SocketThread() = default;

    /**
     * @brief This function listens to the state of fd in poll all the time.
     * @details When creating a thread, it will be passed as an argument to
     *          the function pthread_create().
     *          It must be void * (*__ start_ Routine) (void *) type.
     * @return void*
     */
    void OnListenEvents(void);
    /**
     * @brief Convert the expected event to poll event.
     * @param flags Status to be detected.
     * @return unsigned int
     */
    uint32_t FlagsChangeEvents(int flags);
    /**
     * @brief Find fd from fdMap_.
     * @param sock Socket object.
     * @return int
     */
    int FindFd(Socket &sock);
    /**
     * @brief Delete fd from fdMap_.
     * @param sock Socket object.
     * @return void
     */
    void DeleteFd(Socket &sock);

    int epollFd_ {-1};
    int stopFd_ {-1};
    int count_ {0};

    std::mutex mutex_ {};
    std::mutex epollFdMutex_ {};
    std::unique_ptr<std::thread> thread_ {};

    std::map<Socket *, int> fdMap_ {};

    SocketOptionCallback readCallback_ {Socket::OnSocketReadReady};
    SocketOptionCallback writeCallback_ {Socket::OnSocketWriteReady};
    SocketOptionCallback exceptCallback_ {Socket::OnSocketException};
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // SOCKET_LISTENER_H