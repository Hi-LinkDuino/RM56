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

static std::string g_testMsg = "";

class EventHandlerFdListenerModuleTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void EventHandlerFdListenerModuleTest::SetUpTestCase(void)
{}

void EventHandlerFdListenerModuleTest::TearDownTestCase(void)
{}

void EventHandlerFdListenerModuleTest::SetUp(void)
{
    /**
     * @tc.setup: Set the test message to null string.
     */
    g_testMsg = "";
}

void EventHandlerFdListenerModuleTest::TearDown(void)
{}

class MyFileDescriptorListener : public FileDescriptorListener {
public:
    explicit MyFileDescriptorListener(int32_t fd = -1) : fd_(fd)
    {}

    ~MyFileDescriptorListener()
    {}

    void OnReadable(int32_t fileDescriptor) override
    {
        if (fileDescriptor < 0) {
            g_testMsg = "bad fd";
            return;
        }

        FileDescriptorListener::OnReadable(fileDescriptor);

        uint32_t messageSize = 64;
        char message[messageSize];
        ssize_t retVal = read(fileDescriptor, message, sizeof(message) - 1);
        if (retVal > 0) {
            message[retVal] = '\0';
            g_testMsg = message;
        }
    }

    void OnWritable(int32_t fileDescriptor) override
    {
        if (fileDescriptor < 0) {
            g_testMsg = "bad fd";
            return;
        }

        FileDescriptorListener::OnWritable(fileDescriptor);

        if (fd_ >= 0) {
            close(fd_);
            return;
        }

        auto handler = GetOwner();
        if (handler) {
            handler->RemoveFileDescriptorListener(fileDescriptor);
            auto f = [fileDescriptor]() { write(fileDescriptor, "test", sizeof("test")); };
            int64_t delayTime = 50;
            handler->PostTask(f, delayTime);
        }
    }

    void OnShutdown(int32_t fileDescriptor) override
    {
        if (fileDescriptor < 0) {
            g_testMsg = "bad fd";
            return;
        }

        g_testMsg = "OnShutdown";
        FileDescriptorListener::OnShutdown(fileDescriptor);
    }

    void OnException(int32_t fileDescriptor) override
    {
        if (fileDescriptor < 0) {
            g_testMsg = "bad fd";
            return;
        }

        g_testMsg = "OnException";
        FileDescriptorListener::OnException(fileDescriptor);
    }

private:
    int32_t fd_;
};

/**
 * @tc.name: AddListener001
 * @tc.desc: Add listener of input event and output event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, AddListener001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener, write and read the data.
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

    int64_t delayTime = 100;
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime);
    myRunner->Run();
    EXPECT_EQ("test", g_testMsg);
}

/**
 * @tc.name: AddListener002
 * @tc.desc: Add listener with forbidden fd
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, AddListener002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener with forbidden fd.
     * @tc.expected: step1. Add fd listener failed and return error code.
     */
    auto listener = std::make_shared<MyFileDescriptorListener>();
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    int32_t fd = -1;
    auto inResult = handler->AddFileDescriptorListener(fd, FILE_DESCRIPTOR_INPUT_EVENT, listener);
    EXPECT_EQ(EVENT_HANDLER_ERR_INVALID_PARAM, inResult);
}

/**
 * @tc.name: AddListener003
 * @tc.desc: Add listener without event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, AddListener003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener without event.
     * @tc.expected: step1. Add fd listener failed and return error code.
     */
    int32_t fds[] = {-1, -1};
    int32_t pipe = pipe2(fds, O_NONBLOCK);
    EXPECT_GE(pipe, 0);

    auto listener = std::make_shared<MyFileDescriptorListener>();
    auto myRunner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    uint32_t event = 0;
    auto inResult = handler->AddFileDescriptorListener(fds[0], event, listener);
    EXPECT_EQ(EVENT_HANDLER_ERR_INVALID_PARAM, inResult);
}

/**
 * @tc.name: AddListener004
 * @tc.desc: Add listener without listener
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, AddListener004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener without listener.
     * @tc.expected: step1. Add fd listener failed and return error code.
     */
    int32_t fds[] = {-1, -1};
    int32_t pipe = pipe2(fds, O_NONBLOCK);
    EXPECT_GE(pipe, 0);

    auto myRunner = EventRunner::Create(true);
    auto handler = std::make_shared<MyEventHandler>(myRunner);

    auto inResult = handler->AddFileDescriptorListener(fds[0], FILE_DESCRIPTOR_INPUT_EVENT, nullptr);
    EXPECT_EQ(EVENT_HANDLER_ERR_INVALID_PARAM, inResult);
}

/**
 * @tc.name: AddListener005
 * @tc.desc: Add listener without event runner
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, AddListener005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener without event runner.
     * @tc.expected: step1. Add fd listener failed and return error code.
     */
    int32_t fds[] = {-1, -1};
    int32_t pipe = pipe2(fds, O_NONBLOCK);
    EXPECT_GE(pipe, 0);

    auto listener = std::make_shared<MyFileDescriptorListener>();
    auto handler = std::make_shared<MyEventHandler>(nullptr);

    auto inResult = handler->AddFileDescriptorListener(fds[0], FILE_DESCRIPTOR_INPUT_EVENT, listener);
    EXPECT_EQ(EVENT_HANDLER_ERR_NO_EVENT_RUNNER, inResult);
}

/**
 * @tc.name: RemoveListener001
 * @tc.desc: Remove all listeners
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, RemoveListener001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener of write and read, and then remove all listener.
     * @tc.expected: step1. The data not written and read after removed.
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
    handler->RemoveAllFileDescriptorListeners();

    int64_t delayTime = 100;
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime);
    myRunner->Run();
    EXPECT_EQ("", g_testMsg);
}

/**
 * @tc.name: RemoveListener002
 * @tc.desc: Remove a listener by fd
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, RemoveListener002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener of write and read, and then remove the listener by fd.
     * @tc.expected: step1. The data not read after removed.
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
    handler->RemoveFileDescriptorListener(fds[0]);

    int64_t delayTime = 100;
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime);
    myRunner->Run();
    EXPECT_EQ("", g_testMsg);
}

/**
 * @tc.name: TriggerShutdown001
 * @tc.desc: Trigger shutdown event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, TriggerShutdown001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener of read, and then close the write fd.
     * @tc.expected: step1. The shutdown event appeared.
     */
    int32_t fds[] = {-1, -1};
    int32_t pipe = pipe2(fds, O_NONBLOCK);
    EXPECT_GE(pipe, 0);

    auto listener = std::make_shared<MyFileDescriptorListener>();
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto otherHandler = std::make_shared<MyEventHandler>(EventRunner::Create());
    auto inResult = handler->AddFileDescriptorListener(fds[0], FILE_DESCRIPTOR_INPUT_EVENT, listener);
    EXPECT_EQ(inResult, ERR_OK);

    int64_t delayTime = 100;
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime);
    auto task = [fds, otherHandler]() {
        auto f = [fds]() { close(fds[1]); };
        otherHandler->PostTask(f);
    };

    handler->PostTask(task);
    myRunner->Run();
    EXPECT_EQ("OnShutdown", g_testMsg);
}

/**
 * @tc.name: TriggerException001
 * @tc.desc: Trigger exception event
 * @tc.type: FUNC
 * @tc.require: SR000BTOPD SR000BTOPJ SR000BTOPM
 */
HWTEST_F(EventHandlerFdListenerModuleTest, TriggerException001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Add fd listener of write, and then run the runner.
     * @tc.expected: step1. The exception event appeared.
     */
    int32_t fds[] = {-1, -1};
    int32_t pipe = pipe2(fds, O_NONBLOCK);
    EXPECT_GE(pipe, 0);

    auto listener = std::make_shared<MyFileDescriptorListener>(fds[0]);
    auto myRunner = EventRunner::Create(false);
    auto handler = std::make_shared<MyEventHandler>(myRunner);
    auto inResult = handler->AddFileDescriptorListener(fds[1], FILE_DESCRIPTOR_OUTPUT_EVENT, listener);
    EXPECT_EQ(inResult, ERR_OK);

    int64_t delayTime = 100;
    int64_t param = 0;
    handler->SendEvent(STOP_EVENT_ID, param, delayTime);
    myRunner->Run();
    EXPECT_EQ("OnException", g_testMsg);
}