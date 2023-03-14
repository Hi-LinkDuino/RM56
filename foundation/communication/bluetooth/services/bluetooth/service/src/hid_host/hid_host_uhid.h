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

#ifndef HID_HOST_UHID_H
#define HID_HOST_UHID_H

#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <string>
#include <linux/uhid.h>
#include "hid_host_defines.h"
#include "base_def.h"
#include "raw_address.h"

namespace OHOS {
namespace bluetooth {
static constexpr int MAX_POLLING_ATTEMPTS = 10;
static constexpr int POLLING_SLEEP_DURATION_US = 5000;
static constexpr int POLL_TIMEOUT = 50;

/**
 * @brief Class for l2cap connection.
 */
class HidHostUhid {
public:
    explicit HidHostUhid(std::string address);
    ~HidHostUhid();

    int Destroy();
    int SendHidInfo(const char* devName, PnpInformation& pnpInf, HidInformation& hidInf);
    int SendData(uint8_t* pRpt, uint16_t len);
    int SendControlData(uint8_t* pRpt, uint16_t len);
    int SendHandshake(uint16_t err);
    int Close();
    int Open();

    static void* PollEventThread(void* arg);
    void PollEventThread_();

private:
    int WriteUhid(int fd, const struct uhid_event* ev);
    int WritePackUhid(int fd, uint8_t* rpt, uint16_t len);
    int ClosePollThread();
    pthread_t CreateThread(void* (*startRoutine)(void*), void* arg);
    void SetUhidNonBlocking(int fd);
    int ReadUhidEvent();
    int SendGetReportReplyUhid(int fd, int id, uint16_t err, uint8_t* rpt, uint16_t len);
    int SendSetReportReplyUhid(int fd, int id, uint16_t err);
    void ReadUhidOutPut(uhid_event ev);
    void ReadUhidFeature(uhid_event ev);
    void ReadUhidSetReport(uhid_event ev);

    pthread_t pollThreadId_ = -1;
    int fd_ = -1;
    bool keepPolling_ = false;
    bool readyForData_ = false;
    std::string address_;
    int task_id_ = 0;
    int task_type_ = -1;

    BT_DISALLOW_COPY_AND_ASSIGN(HidHostUhid);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif // HID_HOST_UHID_H