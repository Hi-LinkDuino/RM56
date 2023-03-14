/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#include <pthread.h>

#include "rpc_errno.h"
#include "rpc_log.h"
#include "ipc_proxy.h"
#include "ipc_skeleton.h"
#include "serializer.h"

namespace {
constexpr uint32_t IPC_TEST_TIME_INTERVAL = 120;
SvcIdentity *sid = nullptr;
MessageOption g_option = {
    .flags = TF_OP_SYNC
};

void CallAnonymosFunc(const char *str)
{
    if (sid == nullptr) {
        RPC_LOG_INFO("invalid anonymous client");
        return;
    }
    RPC_LOG_INFO("now server call client anonymous func");
    IpcIo data;
    uint8_t tmpData1[IPC_MAX_SIZE];
    IpcIoInit(&data, tmpData1, IPC_MAX_SIZE, 1);
    WriteString(&data, str);

    IpcIo reply;
    MessageOption option = {
        .flags = TF_OP_ASYNC
    };
    SendRequest(*sid, CLIENT_OP_PRINT, &data, &reply, option, nullptr);
}

int32_t RemoteRequestOne(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option)
{
    int32_t result = ERR_NONE;
    switch (code) {
        case SERVER_OP_ADD: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            WriteInt32(reply, a + b);
            break;
        }
        case SERVER_OP_SUB: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            WriteInt32(reply, a - b);
            break;
        }
        case SERVER_OP_MULTI: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            WriteInt32(reply, a * b);
            break;
        }
        case SERVER_OP_ADD_SERVICE: {
            sid = (SvcIdentity *)calloc(1, sizeof(SvcIdentity));
            ReadRemoteObject(data, sid);
            const char *str = "server call anonymos service one.";
            break;
        }
        default:
            RPC_LOG_ERROR("unknown code %d", code);
            break;
    }
    return result;
}

int32_t RemoteRequestTwo(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option)
{
    int32_t result = ERR_NONE;
    RPC_LOG_INFO("server OnRemoteRequestTwo called....");
    switch (code) {
        case SERVER_OP_ADD: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            WriteInt32(reply, a + b);
            break;
        }
        case SERVER_OP_SUB: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            WriteInt32(reply, a - b);
            break;
        }
        case SERVER_OP_MULTI: {
            int32_t a;
            ReadInt32(data, &a);
            int32_t b;
            ReadInt32(data, &b);
            WriteInt32(reply, a * b);
            break;
        }
        default:
            RPC_LOG_ERROR("unknown code %u", code);
            break;
    }
    return result;
}

void *ThreadHandler(void *args)
{
    sleep(IPC_TEST_TIME_INTERVAL); // sleep 2 min
    const char *str = "server call anonymos service new thread.";
    CallAnonymosFunc(str);
    return nullptr;
}

IpcObjectStub objectStubOne = {
    .func = RemoteRequestOne,
    .isRemote = false
};

IpcObjectStub objectStubTwo = {
    .func = RemoteRequestTwo,
    .isRemote = false
};

SvcIdentity svcOne = {
    .handle = -1,
    .token  = (uintptr_t)&objectStubOne,
    .cookie = (uintptr_t)&objectStubOne
};

SvcIdentity svcTwo = {
    .handle = -1,
    .token  = (uintptr_t)&objectStubTwo,
    .cookie = (uintptr_t)&objectStubTwo
};

SvcIdentity sidOne;
}

using namespace testing::ext;

namespace OHOS {
class IpcServerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        RPC_LOG_INFO("----------test case for ipc server start-------------\n");
    }
    static void TearDownTestCase() {}

    void SetUp() {}
    void TearDown()
    {
        sleep(1);
    }
};

HWTEST_F(IpcServerTest, IpcServerTest001, TestSize.Level1)
{
    IpcIo data;
    uint8_t tmpData1[IPC_MAX_SIZE];
    IpcIoInit(&data, tmpData1, IPC_MAX_SIZE, 1);
    WriteInt32(&data, SERVER_SA_ID1);
    bool res = WriteRemoteObject(nullptr, &svcOne);
    EXPECT_EQ(res, false);
}

HWTEST_F(IpcServerTest, IpcServerTest002, TestSize.Level1)
{
    IpcIo data;
    uint8_t tmpData1[IPC_MAX_SIZE];
    IpcIoInit(&data, tmpData1, IPC_MAX_SIZE, 1);
    WriteInt32(&data, SERVER_SA_ID1);
    bool res = WriteRemoteObject(&data, nullptr);
    EXPECT_EQ(res, false);
}

HWTEST_F(IpcServerTest, IpcServerTest003, TestSize.Level0)
{
    IpcIo data;
    uint8_t tmpData1[IPC_MAX_SIZE];
    IpcIoInit(&data, tmpData1, IPC_MAX_SIZE, 1);
    bool wr = WriteInt32(&data, SERVER_SA_ID1);
    EXPECT_TRUE(wr);
    wr = WriteRemoteObject(&data, &svcOne);
    EXPECT_TRUE(wr);
    IpcIo reply;
    uintptr_t ptr = 0;
    RPC_LOG_INFO("====== add ability one to samgr ======");
    SvcIdentity target = {
        .handle = 0
    };
    int ret = SendRequest(target, ADD_SYSTEM_ABILITY_TRANSACTION, &data, &reply, g_option, &ptr);
    int res = -1;
    ReadInt32(&reply, &res);
    FreeBuffer((void *)ptr);
    EXPECT_EQ(ret, ERR_NONE);
    EXPECT_EQ(res, ERR_NONE);
}

HWTEST_F(IpcServerTest, IpcServerTest004, TestSize.Level0)
{
    IpcIo data;
    uint8_t tmpData2[IPC_MAX_SIZE];
    IpcIoInit(&data, tmpData2, IPC_MAX_SIZE, 1);
    WriteInt32(&data, SERVER_SA_ID2);
    WriteRemoteObject(&data, &svcTwo);
    RPC_LOG_INFO("====== add ability two to samgr ======");
    uintptr_t ptr = 0;
    IpcIo reply;
    SvcIdentity target = {
        .handle = 0
    };
    int ret = SendRequest(target, ADD_SYSTEM_ABILITY_TRANSACTION, &data, &reply, g_option, &ptr);
    int res;
    ReadInt32(&reply, &res);
    FreeBuffer((void *)ptr);
    EXPECT_EQ(ret, ERR_NONE);
    EXPECT_EQ(res, ERR_NONE);
}

HWTEST_F(IpcServerTest, IpcServerTest005, TestSize.Level0)
{
    RPC_LOG_INFO("====== get ability one from samgr ======");
    IpcIo data1;
    uint8_t dataGet[IPC_MAX_SIZE];
    IpcIoInit(&data1, dataGet, IPC_MAX_SIZE, 0);
    WriteInt32(&data1, SERVER_SA_ID2);
    uintptr_t ptr = 0;
    IpcIo reply;
    const SvcIdentity *target = GetContextObject();
    int ret = SendRequest(*target, GET_SYSTEM_ABILITY_TRANSACTION, &data1, &reply, g_option, &ptr);
    ReadRemoteObject(&reply, &sidOne);
    FreeBuffer((void *)ptr);
    EXPECT_EQ(ret, ERR_NONE);
}

HWTEST_F(IpcServerTest, IpcServerTest006, TestSize.Level0)
{
    RPC_LOG_INFO("====== call serverone OP_MULTI ======");
    IpcIo data2;
    uint8_t dataMulti[IPC_MAX_SIZE];
    IpcIoInit(&data2, dataMulti, IPC_MAX_SIZE, 0);
    WriteInt32(&data2, OP_A);
    WriteInt32(&data2, OP_B);
    uintptr_t ptr = 0;
    IpcIo reply;
    int ret = SendRequest(sidOne, SERVER_OP_MULTI, &data2, &reply, g_option, &ptr);
    int res;
    ReadInt32(&reply, &res);
    RPC_LOG_INFO(" 12 * 17 = %d", res);
    FreeBuffer((void *)ptr);
    EXPECT_EQ(ret, ERR_NONE);
    int tmpMul = OP_A * OP_B;
    EXPECT_EQ(res, tmpMul);
}

HWTEST_F(IpcServerTest, IpcServerTest007, TestSize.Level0)
{
    pthread_t pid;
    pthread_create(&pid, nullptr, ThreadHandler, nullptr);
    pthread_detach(pid);

    JoinWorkThread();
}
}  // namespace OHOS