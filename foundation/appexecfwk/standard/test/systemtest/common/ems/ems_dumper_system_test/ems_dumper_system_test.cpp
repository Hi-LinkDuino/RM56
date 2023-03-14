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

#include <gtest/gtest.h>

#include "event_handler.h"
#include "event_queue.h"
#include "event_runner.h"
#include "inner_event.h"

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
bool g_isDump = false;
const int64_t DELAY_TIME = 10;
const std::string GET_TAG_SYSTEM_TEST1 = "GetTagSystemTest1";
const std::string GET_TAG_SYSTEM_TEST2 = " ";
const std::string GET_TAG_SYSTEM_TEST3 = "@#￥#3243adsafdf_中文";
const std::string GET_TAG_SYSTEM_TEST4 = std::to_string(0);
const std::string GET_TAG_SYSTEM_TEST5 = std::to_string(0) + "@#￥#3243adsafdf_中文";
}  // namespace

class DumpTest : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {
        g_isDump = true;
        GTEST_LOG_(INFO) << message;
    }

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return "DumpTest";
    }
};

class GetTagTest1 : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {}

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return GET_TAG_SYSTEM_TEST1;
    }
};

class GetTagTest2 : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {}

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return GET_TAG_SYSTEM_TEST2;
    }
};

class GetTagTest3 : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {}

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return GET_TAG_SYSTEM_TEST3;
    }
};

class GetTagTest4 : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {}

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return GET_TAG_SYSTEM_TEST4;
    }
};

class GetTagTest5 : public Dumper {
public:
    /**
     * Processes the content of a specified string.
     * @param message the content of a specified string.
     */
    void Dump(const std::string &message)
    {}

    /**
     * Obtains the tag information.
     * which is a prefix added to each string before the string content is processed.
     * @return the tag information.
     */
    std::string GetTag()
    {
        return GET_TAG_SYSTEM_TEST5;
    }
};

class EmsDumperSystemTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EmsDumperSystemTest::SetUpTestCase(void)
{}

void EmsDumperSystemTest::TearDownTestCase(void)
{}

void EmsDumperSystemTest::SetUp(void)
{}

void EmsDumperSystemTest::TearDown(void)
{}

/*
 * @tc.name: Dump001
 * @tc.desc: Check Dump
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, Dump001, TestSize.Level1)
{
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get();
    DumpTest dump001;
    usleep(100 * 1000);
    handler->Dump(dump001);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump002
 * @tc.desc: Check Dump after post task
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, Dump002, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto task = []() {; };
    DumpTest dump002;
    handler->PostTask(task, DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dump002);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump003
 * @tc.desc: Check Dump after send event with event id
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, Dump003, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get();
    DumpTest dump003;
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dump003);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump004
 * @tc.desc: Check Dump after send event with event id and param
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, Dump004, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get();
    DumpTest dump004;
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dump004);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: Dump005
 * @tc.desc: check when send event and post task dump success
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, Dump005, TestSize.Level1)
{
    g_isDump = false;
    auto runner = EventRunner::Create(true);
    auto handler = std::make_shared<EventHandler>(runner);
    auto event = InnerEvent::Get();
    auto task = []() {; };
    DumpTest dump005;
    handler->SendEvent(event, DELAY_TIME, EventQueue::Priority::LOW);
    handler->PostTask(task, DELAY_TIME * 2, EventQueue::Priority::LOW);
    usleep(100 * 1000);
    handler->Dump(dump005);
    EXPECT_TRUE(g_isDump);
}

/*
 * @tc.name: GetTag001
 * @tc.desc: Check GetTagTest1 process GetTag success
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, GetTag001, TestSize.Level1)
{
    GetTagTest1 getTagTest;
    std::string result = getTagTest.GetTag();
    EXPECT_EQ(result, GET_TAG_SYSTEM_TEST1);
}

/*
 * @tc.name: GetTag002
 * @tc.desc: Check GetTagTest2 process GetTag success
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, GetTag002, TestSize.Level1)
{
    GetTagTest2 getTagTest;
    std::string result = getTagTest.GetTag();
    EXPECT_EQ(result, GET_TAG_SYSTEM_TEST2);
}

/*
 * @tc.name: GetTag003
 * @tc.desc: Check GetTagTest3 process GetTag success
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, GetTag003, TestSize.Level1)
{
    GetTagTest3 getTagTest;
    std::string result = getTagTest.GetTag();
    EXPECT_EQ(result, GET_TAG_SYSTEM_TEST3);
}

/*
 * @tc.name: GetTag004
 * @tc.desc: Check GetTagTest4 process GetTag success
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, GetTag004, TestSize.Level1)
{
    GetTagTest4 getTagTest;
    std::string result = getTagTest.GetTag();
    EXPECT_EQ(result, GET_TAG_SYSTEM_TEST4);
}

/*
 * @tc.name: GetTag005
 * @tc.desc: Check GetTagTest5 process GetTag success
 * @tc.type: FUNC
 */
HWTEST_F(EmsDumperSystemTest, GetTag005, TestSize.Level1)
{
    GetTagTest5 getTagTest;
    std::string result = getTagTest.GetTag();
    EXPECT_EQ(result, GET_TAG_SYSTEM_TEST5);
}