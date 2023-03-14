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
#include "sys_event_test.h"

#include <ctime>
#include <iostream>
#include <memory>
#include <regex>
#include <vector>

#include "sys_event.h"
namespace OHOS {
namespace HiviewDFX {
void SysEventTest::SetUpTestCase()
{
}

void SysEventTest::TearDownTestCase()
{
}

void SysEventTest::SetUp()
{
}

void SysEventTest::TearDown()
{
}

/**
 * @tc.name: TestSendBaseType001
 * @tc.desc: create base sys event
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q2 AR000FT2Q3
 */
HWTEST_F(SysEventTest, TestSendBaseType001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create base sys event
     */
    std::cout << "TestSendBaseType001 test base type" << std::endl;
    SysEventCreator sysEventCreator("DEMO", "EVENT_NAME", SysEventCreator::FAULT);
    sysEventCreator.SetKeyValue("KEY", 1);
    std::shared_ptr<SysEvent> sysEvent = std::make_shared<SysEvent>("test", nullptr, sysEventCreator);
    std::regex expValue(R"~(\{"domain_":"DEMO","name_":"EVENT_NAME","type_":1,"time_":\d+,"tz_":"[\+\-]\d+","pid_":\d+,"tid_":\d+,"uid_":\d+,"KEY":1\})~");
    std::cout << "size=" << sysEvent->jsonExtraInfo_.size() << ", jsonStr:" << sysEvent->jsonExtraInfo_ << std::endl;
    std::smatch baseMatch;
    bool isMatch = std::regex_match(sysEvent->jsonExtraInfo_, baseMatch, expValue);
    ASSERT_TRUE(isMatch);
}

/**
 * @tc.name: TestSendIntVectorType002
 * @tc.desc: create base sys event
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q2 AR000FT2Q3
 */
HWTEST_F(SysEventTest, TestSendIntVectorType002, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create base sys event
     */
    std::cout << "TestSendIntVectorType002 test vector<int> type" << std::endl;
    SysEventCreator sysEventCreator("DEMO", "EVENT_NAME", SysEventCreator::FAULT);
    std::vector<int> values = {1, 2, 3};
    sysEventCreator.SetKeyValue("KEY", values);
    std::shared_ptr<SysEvent> sysEvent = std::make_shared<SysEvent>("test", nullptr, sysEventCreator);
    std::regex expValue(R"~(\{"domain_":"DEMO","name_":"EVENT_NAME","type_":1,"time_":\d+,"tz_":"[\+\-]\d+","pid_":\d+,"tid_":\d+,"uid_":\d+,"KEY":\[1,2,3\]\})~");
    std::cout << "size=" << sysEvent->jsonExtraInfo_.size() << ", jsonStr:" << sysEvent->jsonExtraInfo_ << std::endl;
    std::smatch baseMatch;
    bool isMatch = std::regex_match(sysEvent->jsonExtraInfo_, baseMatch, expValue);
    ASSERT_TRUE(isMatch);
}
} // HiviewDFX
} // OHOS