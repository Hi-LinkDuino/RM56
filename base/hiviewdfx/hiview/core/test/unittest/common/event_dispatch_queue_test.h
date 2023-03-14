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
#ifndef HIVIEW_PLATFORM_EVENT_DISPATCH_QUEUE_TEST_H
#define HIVIEW_PLATFORM_EVENT_DISPATCH_QUEUE_TEST_H

#include <map>
#include <memory>

#include <gtest/gtest.h>

#include "event.h"
#include "event_dispatch_queue.h"
#include "hiview_platform.h"

class EventDispatchQueueTest : public testing::Test {
public:
    EventDispatchQueueTest() {};
    ~EventDispatchQueueTest() {};
    void SetUp();
    void TearDown();
    std::shared_ptr<OHOS::HiviewDFX::Event> CreateEvent(const std::string& name, int32_t id,
                                                             const std::string& message,
                                                             OHOS::HiviewDFX::Event::MessageType type);
    static constexpr int EVENT_ID_0 = 0;
    static constexpr int EVENT_ID_1 = 1;
    static constexpr int EVENT_ID_2 = 2;
    static constexpr int EVENT_ID_3 = 3;
    static constexpr int EVENT_ID_4 = 4;
    static constexpr int EVENT_ID_5 = 5;
};

class ExtendEventListener : public OHOS::HiviewDFX::EventListener {
public:
    explicit ExtendEventListener(const std::string& name) : unorderEventCount_(0), name_(name){};
    ~ExtendEventListener(){};
    void OnUnorderedEvent(const OHOS::HiviewDFX::Event &msg) override;
    std::string GetListenerName() override;
    std::map<std::string, std::string> processedUnorderedEvents_;
    uint32_t unorderEventCount_;

private:
    std::string name_;
};
#endif // HIVIEW_PLATFORM_EVENT_DISPATCH_QUEUE_TEST_H
