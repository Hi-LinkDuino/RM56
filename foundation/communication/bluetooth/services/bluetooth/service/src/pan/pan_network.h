/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef PAN_NETWORK_H
#define PAN_NETWORK_H

#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <unistd.h>
#include <condition_variable>
#include <mutex>
#include <string>
#include "base_def.h"
#include "pan_defines.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
static constexpr int POLL_TIMEOUT = 50;

/**
 * @brief Class for l2cap connection.
 */
class PanNetwork {
public:
    PanNetwork();
    ~PanNetwork();

    int Close();
    int Open();
    int WriteData(EthernetHeader head, uint8_t *data, int len);
    int ReceiveRemoteBusy(bool isBusy);

private:
    int TunSetIff();
    int SetMacAddress(int inetSocket);
    int SetIpAddress(int inetSocket);
    int SetIffUp(int inetSocket);
    int SetIffdown(int inetSocket);

    static void* PollEventThread(void* arg);
    void PollEventThread_();
    pthread_t CreateThread(void* (*startRoutine)(void*), void* arg);
    void SetPanNetworkNonBlocking(int fd);
    int ReadPanNetworkEvent();

    pthread_t pollThreadId_;
    int fd_ = -1;
    bool keepPolling_ = false;
    bool isRemoteDeviceBusy_ = false;
    std::mutex mutexBusyChanged_;
    std::condition_variable cvWaitBusyChanged_;

    BT_DISALLOW_COPY_AND_ASSIGN(PanNetwork);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // PAN_NETWORK_H