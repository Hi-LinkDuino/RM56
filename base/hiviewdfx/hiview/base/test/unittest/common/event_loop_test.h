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
#ifndef HIVIEW_EVENT_LOOP_TEST_H
#define HIVIEW_EVENT_LOOP_TEST_H
#include <memory>

#include <sys/epoll.h>

#include <gtest/gtest.h>

#include "event_loop.h"

namespace OHOS {
namespace HiviewDFX {
class EventLoopTest : public testing::Test {
public:
    void SetUp();
    void TearDown();
    std::shared_ptr<OHOS::HiviewDFX::EventLoop> currentLooper_;
};

class RealEventHandler : public OHOS::HiviewDFX::EventHandler {
public:
    ~RealEventHandler() {};
    bool OnEvent(std::shared_ptr<OHOS::HiviewDFX::Event>& event) override;
    void DoTask();
    int32_t lastProcessId_ = 0;
    int32_t processedEventCount_ = 0;
    int32_t pid_ = 0;
    std::vector<uint16_t> receivedEventNo_;
};

class DataFileEventReader : public OHOS::HiviewDFX::FileDescriptorEventCallback {
public:
    ~DataFileEventReader() {};
    bool OnFileDescriptorEvent(int fd, int type) override;
    int32_t GetPollFd() override;
    int32_t GetPollType() override;
    const static inline std::string EVENT_LOG_PATH = "/data/log/DataFileEventReader";
    int inotifyFd_;
    std::map<std::string, int> fileMap_;
};

class OverheadCalculateEventHandler : public OHOS::HiviewDFX::EventHandler {
public:
    ~OverheadCalculateEventHandler() {};
    bool OnEvent(std::shared_ptr<OHOS::HiviewDFX::Event>& event) override;
    uint64_t totalDeliverOverHead_ = 0;
    uint64_t processedEventCount_ = 0;
};
}
}
#endif // HIVIEW_EVENT_LOOP_TEST_H
