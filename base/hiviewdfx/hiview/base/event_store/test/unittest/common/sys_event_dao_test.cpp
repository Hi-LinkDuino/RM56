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
#include "sys_event_dao_test.h"

#include <ctime>
#include <iostream>
#include <memory>

#include <gmock/gmock.h>
#include "event.h"
#include "hiview_platform.h"
#include "sys_event.h"
#include "sys_event_dao.h"
namespace OHOS {
namespace HiviewDFX {
void SysEventDaoTest::SetUpTestCase()
{
    OHOS::HiviewDFX::HiviewPlatform &platform = HiviewPlatform::GetInstance();
    std::string defaultDir = "/data/test/test_data/hiview_platform_config";
    if (!platform.InitEnvironment(defaultDir)) {
        std::cout << "fail to init environment" << std::endl;
    } else {
        std::cout << "init environment successful" << std::endl;
    }
}

void SysEventDaoTest::TearDownTestCase()
{
}

void SysEventDaoTest::SetUp()
{
}

void SysEventDaoTest::TearDown()
{
}

/**
 * @tc.name: TestSysEventDaoInsert_001
 * @tc.desc: save event to doc store
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q3
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestSysEventDaoInsert_001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    std::string jsonStr = R"~({"domain_":"demo","name_":"SysEventDaoTest_001","type_":1,"tz_":8,"time_":1620271291188,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97,"keyShort":-100,"keyInt":-200,"KeyLong":-300,"KeyLongLong":-400,"keyUnsignedChar":97,
        "keyUnsignedShort":100,"keyUnsignedInt":200,"keyUnsignedLong":300,"keyUnsignedLongLong":400,"keyFloat":1.1,
        "keyDouble":2.2,"keyString1":"abc","keyString2":"efg","keyBools":[1,1,0],"keyChars":[97,98,99],
        "keyUnsignedChars":[97,98,99],"keyShorts":[-100,-200,-300],"keyUnsignedShorts":[100,200,300],
        "keyInts":[-1000,-2000,-3000],"keyUnsignedInts":[1000,2000,3000],"keyLongs":[-10000,-20000,-30000],
        "keyUnsignedLongs":[10000,20000,30000],"keyLongLongs":[-100000,-200000,-300000],
        "keyUnsignedLongLongs":[100000,200000,300000],"keyFloats":[1.1,2.2,3.3],
        "keyDoubles":[10.1,20.2,30.3],"keyStrings":["a","b","c"]})~";
    auto sysEvent = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr);
    ASSERT_TRUE(sysEvent->PaserJson() == 0);
    int retCode = EventStore::SysEventDao::Insert(sysEvent);
    ASSERT_TRUE(retCode == 0);
}

/**
 * @tc.name: TestEventDaoQuery_002
 * @tc.desc: query event from doc store
 * @tc.type: FUNC
 * @tc.require: AR000FT2PO
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestEventDaoQuery_002, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    std::string jsonStr1 = R"~({"domain_":"demo","name_":"SysEventDaoTest_002","type_":1,"tz_":8,"time_":162027129100,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97,"keyShort":-100,"keyInt":-200,"KeyLong":-300,"KeyLongLong":-400,"keyUnsignedChar":97,
        "keyUnsignedShort":100,"keyUnsignedInt":200,"keyUnsignedLong":300,"keyUnsignedLongLong":400,"keyFloat":1.1,
        "keyDouble":2.2,"keyString1":"abc","keyString2":"efg","keyBools":[1,1,0],"keyChars":[97,98,99],
        "keyUnsignedChars":[97,98,99],"keyShorts":[-100,-200,-300],"keyUnsignedShorts":[100,200,300],
        "keyInts":[-1000,-2000,-3000],"keyUnsignedInts":[1000,2000,3000],"keyLongs":[-10000,-20000,-30000],
        "keyUnsignedLongs":[10000,20000,30000],"keyLongLongs":[-100000,-200000,-300000],
        "keyUnsignedLongLongs":[100000,200000,300000],"keyFloats":[1.1,2.2,3.3],
        "keyDoubles":[10.1,20.2,30.3],"keyStrings":["a","b","c"]})~";
    auto sysEvent1 = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr1);
    ASSERT_TRUE(sysEvent1->PaserJson() == 0);
    int retCode1 = EventStore::SysEventDao::Insert(sysEvent1);
    ASSERT_TRUE(retCode1 == 0);

    std::string jsonStr2 = R"~({"domain_":"demo","name_":"SysEventDaoTest_002","type_":1,"tz_":8,"time_":162027129110,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97,"keyShort":-100,"keyInt":-200,"KeyLong":-300,"KeyLongLong":-400,"keyUnsignedChar":97,
        "keyUnsignedShort":100,"keyUnsignedInt":200,"keyUnsignedLong":300,"keyUnsignedLongLong":400,"keyFloat":1.1,
        "keyDouble":2.2,"keyString1":"abc","keyString2":"efg","keyBools":[1,1,0],"keyChars":[97,98,99],
        "keyUnsignedChars":[97,98,99],"keyShorts":[-100,-200,-300],"keyUnsignedShorts":[100,200,300],
        "keyInts":[-1000,-2000,-3000],"keyUnsignedInts":[1000,2000,3000],"keyLongs":[-10000,-20000,-30000],
        "keyUnsignedLongs":[10000,20000,30000],"keyLongLongs":[-100000,-200000,-300000],
        "keyUnsignedLongLongs":[100000,200000,300000],"keyFloats":[1.1,2.2,3.3],
        "keyDoubles":[10.1,20.2,30.3],"keyStrings":["a","b","c"]})~";
    auto sysEvent2 = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr2);
    ASSERT_TRUE(sysEvent2->PaserJson() == 0);
    int retCode2 = EventStore::SysEventDao::Insert(sysEvent2);
    ASSERT_TRUE(retCode2 == 0);

    EventStore::SysEventQuery sysEventQuery = EventStore::SysEventDao::BuildQuery();
    EventStore::ResultSet resultSet = sysEventQuery.Select({EventStore::EventCol::TS}).
        Where(EventStore::EventCol::TS, EventStore::Op::EQ, 162027129100).Execute();
    int count = 0;
    while (resultSet.HasNext()) {
        count++;
        EventStore::ResultSet::RecordIter it = resultSet.Next();
        ASSERT_TRUE(it->GetSeq() == sysEvent1->GetSeq());
        std::cout << "seq=" << it->GetSeq() << ", json=" << it->jsonExtraInfo_ << std::endl;
    }
    ASSERT_TRUE(count == 1);
}

/**
 * @tc.name: TestEventDaoDel_003
 * @tc.desc: delete event from doc store
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q2
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestEventDaoDel_003, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    std::string jsonStr = R"~({"domain_":"demo","name_":"SysEventDaoTest_003","type_":1,"tz_":8,"time_":1620271291200,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97})~";
    auto sysEvent = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr);
    ASSERT_TRUE(sysEvent->PaserJson() == 0);
    int retCode = EventStore::SysEventDao::Insert(sysEvent);
    ASSERT_TRUE(retCode == 0);
    EventStore::SysEventQuery sysEventQuery1 = EventStore::SysEventDao::BuildQuery();
    EventStore::ResultSet resultSet = sysEventQuery1.Select({EventStore::EventCol::TS}).
        Where(EventStore::EventCol::TS, EventStore::Op::EQ, 1620271291200).Execute();
    int count = 0;
    while (resultSet.HasNext()) {
        count++;
        EventStore::ResultSet::RecordIter it = resultSet.Next();
        ASSERT_TRUE(it->GetSeq() == sysEvent->GetSeq());
        std::cout << "seq=" << it->GetSeq() << ", json=" << it->jsonExtraInfo_ << std::endl;
    }
    ASSERT_TRUE(count == 1);

    EventStore::SysEventQuery delEventQuery = EventStore::SysEventDao::BuildQuery();
    delEventQuery.Where(EventStore::EventCol::TS, EventStore::Op::GT, 0);
    EventStore::SysEventDao::Delete(delEventQuery);

    count = 0;
    EventStore::SysEventQuery sysEventQuery2 = EventStore::SysEventDao::BuildQuery();
    sysEventQuery2.Where(EventStore::EventCol::TS, EventStore::Op::GT, 0).Execute();
    while (resultSet.HasNext()) {
        count++;
    }
    ASSERT_TRUE(count == 0);
}


/**
 * @tc.name: TestEventDaoQuery_004
 * @tc.desc: test embed sql
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q5
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestEventDaoQuery_004, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    EventStore::SysEventQuery sysEventQuery = EventStore::SysEventDao::BuildQuery();
    EventStore::Cond timeCond(EventStore::EventCol::TS, EventStore::Op::GE, 100);
    timeCond.And(EventStore::EventCol::TS, EventStore::Op::LT, 999);

    EventStore::Cond eventCond1(EventStore::EventCol::NAME, EventStore::Op::EQ, {"e11", "e12", "e13"});
    EventStore::Cond domainCond1(EventStore::EventCol::DOMAIN, EventStore::Op::EQ, "dA");
    domainCond1.And(eventCond1);

    EventStore::Cond eventCond2(EventStore::EventCol::NAME, EventStore::Op::EQ, {"e21", "e22", "e23"});
    EventStore::Cond domainCond2(EventStore::EventCol::DOMAIN, EventStore::Op::EQ, "dB");
    domainCond2.And(eventCond2);

    EventStore::Cond eventCond3(EventStore::EventCol::NAME, EventStore::Op::EQ, {"e31", "e32", "e33"});
    EventStore::Cond domainCond3(EventStore::EventCol::DOMAIN, EventStore::Op::EQ, "dC");
    domainCond3.And(eventCond3);

    EventStore::Cond domainCond;
    domainCond.Or(domainCond1).Or(domainCond2).Or(domainCond3);
    sysEventQuery.Where(timeCond).And(domainCond);
    EventStore::ResultSet resultSet = sysEventQuery.Execute();
    int count = 0;
    while (resultSet.HasNext()) {
        count++;
    }
    ASSERT_TRUE(count == 0);
}

/**
 * @tc.name: TestEventDaoQuery_005
 * @tc.desc: test embed sql
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q3
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestEventDaoQuery_005, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    EventStore::SysEventQuery sysEventQuery = EventStore::SysEventDao::BuildQuery();
    sysEventQuery.Where(EventStore::EventCol::DOMAIN, EventStore::Op::EQ, "d1")
        .And(EventStore::EventCol::NAME, EventStore::Op::EQ, "e1")
        .And(EventStore::EventCol::TS, EventStore::Op::EQ, 100)
        .And(EventStore::EventCol::TS, EventStore::Op::EQ, 100.1f)
        .Or(EventStore::EventCol::NAME, EventStore::Op::EQ, "e1")
        .Or(EventStore::EventCol::TS, EventStore::Op::EQ, 100)
        .Or(EventStore::EventCol::TS, EventStore::Op::EQ, 100.1f);
    EventStore::ResultSet resultSet = sysEventQuery.Execute();
    int count = 0;
    while (resultSet.HasNext()) {
        count++;
    }
    ASSERT_TRUE(count == 0);
}

/**
 * @tc.name: TestEventDaoUpdate_006
 * @tc.desc: test update sql
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q3
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestEventDaoUpdate_006, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    std::string jsonStr = R"~({"domain_":"demo","name_":"SysEventDaoTest_006","type_":1,"tz_":8,"time_":162027129100,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97})~";
    auto sysEvent = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr);
    ASSERT_TRUE(sysEvent->PaserJson() == 0);

    int retCode = EventStore::SysEventDao::Insert(sysEvent);
    ASSERT_TRUE(retCode == 0);

    sysEvent->jsonExtraInfo_ = R"~({"info_":"update"})~";
    retCode = EventStore::SysEventDao::Update(sysEvent);
    ASSERT_TRUE(retCode == 0);

    EventStore::SysEventQuery sysEventQuery= EventStore::SysEventDao::BuildQuery();
    EventStore::ResultSet resultSet = sysEventQuery
        .Select({EventStore::EventCol::TS})
        .Where(EventStore::EventCol::NAME, EventStore::Op::EQ, "SysEventDaoTest_006")
        .Execute();
    int count = 0;
    while (resultSet.HasNext()) {
        count++;
        EventStore::ResultSet::RecordIter it = resultSet.Next();
        ASSERT_TRUE(it->GetSeq() == sysEvent->GetSeq());
        std::cout << "seq=" << it->GetSeq() << ", json=" << it->jsonExtraInfo_ << std::endl;
        std::string info = it->GetEventValue("info_");
        ASSERT_TRUE(info == "update");
    }
    ASSERT_TRUE(count == 1);
}

/**
 * @tc.name: TestEventDaoHandleRecordDuringQuery_007
 * @tc.desc: test handle record during query
 * @tc.type: FUNC
 * @tc.require: AR000FT2Q3
 * @tc.author: zhouhaifeng
 */
HWTEST_F(SysEventDaoTest, TestEventDaoHandleRecordDuringQuery_007, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. create pipeline event and set event id
     * @tc.steps: step2. invoke OnEvent func
     * @tc.expected: all ASSERT_TRUE work through.
     */
    int retCode = 0;
    std::string jsonStr;
    jsonStr = R"~({"domain_":"demo","name_":"DuringQuery","type_":1,"tz_":8,"time_":162027129100,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97})~";
    auto sysEvent = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr);
    ASSERT_TRUE(sysEvent->PaserJson() == 0);

    retCode = EventStore::SysEventDao::Insert(sysEvent);
    ASSERT_TRUE(retCode == 0);

    jsonStr = R"~({"domain_":"demo","name_":"DuringQuery","type_":1,"tz_":8,"time_":162027129200,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97})~";
    sysEvent = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr);
    ASSERT_TRUE(sysEvent->PaserJson() == 0);

    retCode = EventStore::SysEventDao::Insert(sysEvent);
    ASSERT_TRUE(retCode == 0);

    jsonStr = R"~({"domain_":"demo","name_":"DuringQuery","type_":1,"tz_":8,"time_":162027129300,
        "pid_":6527,"tid_":6527,"traceid_":"f0ed5160bb2df4b","spanid_":"10","pspanid_":"20","trace_flag_":4,"keyBool":1,
        "keyChar":97})~";
    sysEvent = std::make_shared<SysEvent>("SysEventSource", nullptr, jsonStr);
    ASSERT_TRUE(sysEvent->PaserJson() == 0);

    retCode = EventStore::SysEventDao::Insert(sysEvent);
    ASSERT_TRUE(retCode == 0);

    int count = 0;
    EventStore::SysEeventCallBack c = [&](SysEvent &sysEvent) -> int {
        count++;
        std::cout << "callback->" << sysEvent.happenTime_ << std::endl;
        if (sysEvent.happenTime_ >= 162027129200) {
            std::cout << "break without read all data" << std::endl;
            return 1;
        }
        return 0;
    };
    EventStore::SysEventQuery sysEventQuery= EventStore::SysEventDao::BuildQuery();
    sysEventQuery
        .Select({EventStore::EventCol::TS})
        .Where(EventStore::EventCol::NAME, EventStore::Op::EQ, "DuringQuery")
        .Order(EventStore::EventCol::TS)
        .ExecuteWithCallback(c, 10);

    ASSERT_TRUE(count == 2);
}
} // namespace HiviewDFX
} // namespace OHOS