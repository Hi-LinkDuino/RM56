/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <thread>
#include "dm_common_event_manager.h"
#include "dm_constants.h"
#include "hichain_connector.h"
#include "UTTest_dm_common_event_manager.h"

namespace OHOS {
namespace DistributedHardware {
void DmCommonEventManagerTest::SetUp()
{
}

void DmCommonEventManagerTest::TearDown()
{
}

void DmCommonEventManagerTest::SetUpTestCase()
{
}

void DmCommonEventManagerTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: DmCommonEventManager::~DmCommonEventManager_001
 * @tc.desc: Destructor of DmCommonEventManager
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmCommonEventManagerTest, DeleteDmCommonEventManager_001, testing::ext::TestSize.Level0)
{
    DmCommonEventManager::GetInstance().~DmCommonEventManager();
}

/**
 * @tc.name: DmCommonEventManager::SubscribeServiceEvent_001
 * @tc.desc: call SubscribeServiceEvent(), pass parameters： strEvent and callbackNode.callback_
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmCommonEventManagerTest, SubscribeServiceEvent_001, testing::ext::TestSize.Level0)
{
    std::string strEvent = "test";
    CommomEventCallbackNode callbackNode;
    callbackNode.callback_ = nullptr;
    bool result = DmCommonEventManager::GetInstance().SubscribeServiceEvent(strEvent, callbackNode.callback_);
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: DmCommonEventManager::SubscribeServiceEvent_002
 * @tc.desc: call SubscribeServiceEvent(), pass parameters： strEvent and callbackNode.callback_
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmCommonEventManagerTest, SubscribeServiceEvent_002, testing::ext::TestSize.Level0)
{
    std::string strEvent = "test";
    CommomEventCallbackNode callbackNode;
    std::shared_ptr<DmCommonEventManager::EventSubscriber> subscriber = nullptr;
    bool result = DmCommonEventManager::GetInstance().SubscribeServiceEvent(strEvent, callbackNode.callback_);
    EXPECT_EQ(false, result);
}

/**
 * @tc.name: DmCommonEventManager::UnsubscribeServiceEvent_001
 * @tc.desc: call UnsubscribeServiceEvent(), pass parameters： strEvent
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmCommonEventManagerTest, UnsubscribeServiceEvent_001, testing::ext::TestSize.Level0)
{
    std::string event = "123";
    CommomEventCallbackNode callbackNode;
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(event);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    std::shared_ptr<DmCommonEventManager::EventSubscriber> subscriber =
        std::make_shared<DmCommonEventManager::EventSubscriber>(subscriberInfo, callbackNode.callback_, event);
    DmCommonEventManager::GetInstance().dmEventSubscriber_[event] = subscriber;
    std::string strEvent = "123";
    bool result = DmCommonEventManager::GetInstance().UnsubscribeServiceEvent(strEvent);
    EXPECT_EQ(true, result);
}

/**
 * @tc.name: DmCommonEventManager::EventSubscriber::OnReceiveEvent_001
 * @tc.desc: call OnReceiveEvent(), pass parameters： data
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmCommonEventManagerTest, OnReceiveEvent_001, testing::ext::TestSize.Level0)
{
    EventFwk::CommonEventData data;
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    CommomEventCallbackNode callbackNode;
    std::string strEvent = "test";
    std::shared_ptr<DmCommonEventManager::EventSubscriber> eventSubscriber =
        std::make_shared<DmCommonEventManager::EventSubscriber>(subscribeInfo, callbackNode.callback_, strEvent);
    eventSubscriber->OnReceiveEvent(data);
}
} // namespace
} // namespace DistributedHardware
} // namespace OHOS
