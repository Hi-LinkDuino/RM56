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

#include "event_dispatch_queue_test.h"

using namespace testing::ext;
using namespace OHOS::HiviewDFX;

void EventDispatchQueueTest::SetUp()
{
    /**
     * @tc.setup: create order and unordered event dispatch queue
     */
    printf("SetUp.\n");
}

void EventDispatchQueueTest::TearDown()
{
    /**
     * @tc.teardown: destroy the event dispatch queue we have created
     */
    printf("TearDown.\n");
}

std::shared_ptr<Event> EventDispatchQueueTest::CreateEvent(const std::string& name, int32_t id,
                                                           const std::string& message, Event::MessageType type)
{
    auto event = std::make_shared<Event>(name);
    event->messageType_ = type;
    event->eventId_ = id;
    event->SetValue("message", message);
    return event;
}

void ExtendEventListener::OnUnorderedEvent(const OHOS::HiviewDFX::Event& msg)
{
    printf("cur listener:%s OnUnorderedEvent eventId_:%u \n", name_.c_str(), msg.eventId_);
    unorderEventCount_++;
    auto message = msg.GetValue("message");
    processedUnorderedEvents_[message] = msg.sender_;
}

std::string ExtendEventListener::GetListenerName()
{
    return name_;
}
/**
 * @tc.name: EventDispatchQueueCreateTest001
 * @tc.desc: create and init an event dispatch queue
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventDispatchQueueTest, EventDispatchQueueCreateTest001, TestSize.Level3)
{
    printf("EventDispatchQueueTest.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    if (!platform.InitEnvironment()) {
        printf("Fail to init environment. \n");
    }
    auto unorderQueue = std::make_shared<EventDispatchQueue>("test1", Event::ManageType::UNORDERED, &platform);
    ASSERT_EQ(false, unorderQueue->IsRunning());
    unorderQueue->Start();
    sleep(1);
    ASSERT_EQ(true, unorderQueue->IsRunning());
    unorderQueue->Stop();
    ASSERT_EQ(false, unorderQueue->IsRunning());
}

/**
 * @tc.name: UnorderEventDispatchTest001
 * @tc.desc: create event and send it to a unorder dispatch queue
 * @tc.type: FUNC
 * @tc.require: AR000DPTSU
 */
HWTEST_F(EventDispatchQueueTest, UnorderEventDispatchTest001, TestSize.Level3)
{
    printf("EventDispatchQueueTest.\n");
    OHOS::HiviewDFX::HiviewPlatform platform;
    if (!platform.InitEnvironment()) {
        printf("Fail to init environment. \n");
    }
    auto unorder = std::make_shared<EventDispatchQueue>("disp-unorder", Event::ManageType::UNORDERED, &platform);
    unorder->Start();

    auto listener1 = std::make_shared<ExtendEventListener>("listener1");
    platform.RegisterUnorderedEventListener(listener1);
    listener1->AddListenerInfo(Event::MessageType::RAW_EVENT, EventListener::EventIdRange(EVENT_ID_0, EVENT_ID_2));
    auto listener2 = std::make_shared<ExtendEventListener>("listener2");
    platform.RegisterUnorderedEventListener(listener2);
    listener2->AddListenerInfo(Event::MessageType::RAW_EVENT, EVENT_ID_2);

    std::set<EventListener::EventIdRange> listenerInfo1;
    auto ret1 = platform.GetListenerInfo(Event::MessageType::RAW_EVENT, listener1->GetListenerName(), listenerInfo1);
    for (auto& temp : listenerInfo1) {
        printf("listenerInfo1 begin == %d end == %d\n", temp.begin, temp.end);
    }
    ASSERT_EQ(ret1, true);

    std::set<EventListener::EventIdRange> listenerInfo2;
    auto ret2 = platform.GetListenerInfo(Event::MessageType::RAW_EVENT, listener2->GetListenerName(), listenerInfo2);
    for (auto& temp : listenerInfo2) {
        printf("listenerInfo2 begin == %d end == %d\n", temp.begin, temp.end);
    }
    ASSERT_EQ(ret2, true);

    auto event1 = CreateEvent("testEvent1", EVENT_ID_0, "test", Event::MessageType::RAW_EVENT);
    unorder->Enqueue(event1);
    sleep(1);
    ASSERT_EQ(listener1->unorderEventCount_, 1ul);
    ASSERT_EQ(listener2->unorderEventCount_, 0ul);
    auto event2 = CreateEvent("testEvent1", EVENT_ID_2, "test", Event::MessageType::RAW_EVENT);
    unorder->Enqueue(event2);
    sleep(1);
    ASSERT_EQ(listener1->unorderEventCount_, 2ul);
    ASSERT_EQ(listener2->unorderEventCount_, 1ul);
    auto event3 = CreateEvent("testEvent1", EVENT_ID_3, "test", Event::MessageType::RAW_EVENT);
    unorder->Enqueue(event3);
    sleep(1);
    ASSERT_EQ(listener1->unorderEventCount_, 2ul);
    ASSERT_EQ(listener2->unorderEventCount_, 1ul);
    auto event4 = CreateEvent("testEvent1", EVENT_ID_2, "test", Event::MessageType::FAULT_EVENT);
    unorder->Enqueue(event4);
    sleep(1);
    ASSERT_EQ(listener1->unorderEventCount_, 2ul);
    ASSERT_EQ(listener2->unorderEventCount_, 1ul);
}
