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

#include <fcntl.h>

#include "event_handler_test_common.h"

#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::AppExecFwk;

namespace {
const uint32_t TEST_TIMES = 10000;
uint32_t g_sendCount = 0;
uint32_t g_readDataSize = 0;
uint32_t g_writeDataSize = 0;

inline uint32_t RandomInTen()
{
    uint32_t remainderNum = 10;
    return static_cast<uint32_t>(std::rand() % remainderNum);
}

std::string CreateData(size_t dataSize)
{
    std::string message = "";
    for (size_t i = 0; i < dataSize; i++) {
        message += std::to_string(RandomInTen());
    }
    return message;
}
}  // unnamed namespace

class EventHandlerPressModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerPressModuleTest::SetUpTestCase(void)
{}

void EventHandlerPressModuleTest::TearDownTestCase(void)
{}

void EventHandlerPressModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the value of test flags to the default.
     */
    CommonUtils::EventRunSet(false);
    CommonUtils::EventRunCountReset();
    g_sendCount = 0;
    g_readDataSize = 0;
    g_writeDataSize = 0;
}

void EventHandlerPressModuleTest::TearDown(void)
{}

class MyFileDescriptorListener : public FileDescriptorListener {
public:
    MyFileDescriptorListener()
    {}
    ~MyFileDescriptorListener()
    {}

    void OnReadable(int32_t fileDescriptor) override
    {
        uint32_t messageSize = 1024;
        char message[messageSize];
        ssize_t retVal = read(fileDescriptor, message, sizeof(message) - 1);
        if (retVal > 0) {
            message[retVal] = '\0';
            g_readDataSize += retVal;
        }
    }

    void OnWritable(int32_t fileDescriptor) override
    {
        auto handler = GetOwner();
        if (handler) {
            handler->RemoveFileDescriptorListener(fileDescriptor);
            size_t dataSize = 10;
            int64_t delayTime = 10;
            uint32_t writeTimes = 1000;
            for (uint32_t i = 0; i < writeTimes; ++i) {
                std::string message = CreateData(dataSize);
                auto f = [fileDescriptor, message]() { write(fileDescriptor, message.c_str(), message.size()); };
                handler->PostTask(f, delayTime * (i + 1));
                g_writeDataSize += message.size();
            }
        }
    }
};

/**
 * @tc.name: SendPress001
 * @tc.desc: Send the same event for 10000 times
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerPressModuleTest, SendPress001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. Send event for 10000 times.
     * @tc.expected: step1. Send successfully and the event handled, and the succeeded times equals to test times.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    for (uint32_t i = 0; i < TEST_TIMES; ++i) {
        bool sentResult = handler->SendEvent(RUN_EVENT_ID);
        handler->SendEvent(STOP_EVENT_ID);
        myRunner->Run();
        if (sentResult && CommonUtils::EventRunGet()) {
            g_sendCount++;
        }
    }
    EXPECT_EQ(TEST_TIMES, g_sendCount);
}

/**
 * @tc.name: SendPress002
 * @tc.desc: Send event by different handler for 10000 times
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerPressModuleTest, SendPress002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. Send event by different handler for 10000 times.
     * @tc.expected: step1. Send successfully and the event handled,
     *                      and the succeeded times equals to test times.
     *                      the test result is right, equals to test times multiply by 2.
     */
    auto myRunner = EventRunner::Create(false);
    auto handler1 = std::make_shared<MyEventHandler>(myRunner);
    auto handler2 = std::make_shared<MyEventHandler>(myRunner);

    for (uint32_t i = 0; i < TEST_TIMES; ++i) {
        bool sentResult1 = handler1->SendEvent(RUN_EVENT_ID);
        bool sentResult2 = handler2->SendEvent(RUN_EVENT_ID);
        handler2->SendEvent(STOP_EVENT_ID);
        myRunner->Run();
        if (sentResult1 && sentResult2) {
            g_sendCount++;
        }
    }
    EXPECT_EQ(TEST_TIMES, g_sendCount);
    EXPECT_EQ(TEST_TIMES * 2, CommonUtils::EventRunCount());
}

/**
 * @tc.name: SendPress003
 * @tc.desc: Send different event for 10000 times
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerPressModuleTest, SendPress003, TestSize.Level3)
{
    /**
     * @tc.steps: step1. Send different event for 10000 times.
     * @tc.expected: step1. Send successfully and the event handled, and the succeeded times equals to test times.
     */
    auto myRunner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    for (uint32_t i = 0; i < TEST_TIMES; ++i) {
        bool sentResult = handler->SendEvent(Random() + 1);
        if (sentResult) {
            g_sendCount++;
        }
    }
    handler->SendEvent(STOP_EVENT_ID);
    EXPECT_EQ(TEST_TIMES, g_sendCount);
}

/**
 * @tc.name: FdListenerPress001
 * @tc.desc: Press test of listener via epoll
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerPressModuleTest, FdListenerPress001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. Add fd listener, write and read data for 1000 times.
     * @tc.expected: step1. Add fd listener of input and output successfully, write and read the data correctly.
     */
    int32_t fds[] = {-1, -1};
    int32_t pipe = pipe2(fds, O_NONBLOCK);
    EXPECT_GE(pipe, 0);

    auto listener = std::make_shared<MyFileDescriptorListener>();
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto inResult = handler->AddFileDescriptorListener(fds[0], FILE_DESCRIPTOR_INPUT_EVENT, listener);
    EXPECT_EQ(inResult, ERR_OK);
    auto outResult = handler->AddFileDescriptorListener(fds[1], FILE_DESCRIPTOR_OUTPUT_EVENT, listener);
    EXPECT_EQ(outResult, ERR_OK);

    int64_t delayTime = 20000;
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime);
    myRunner->Run();
    EXPECT_EQ(g_writeDataSize, g_readDataSize);
}