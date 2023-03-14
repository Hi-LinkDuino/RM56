/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include <cstring>
#include <unistd.h>

#include "client_executor/include/i_aie_client.inl"
#include "platform/time/include/time.h"
#include "utils/aie_client_callback.h"
#include "utils/aie_client_common.h"
#include "utils/aie_client_const.h"
#include "utils/log/aie_log.h"
#include "utils/service_dead_cb.h"
#include "utils/utils.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

namespace {
    const int WAIT_CALLBACK_TIME_MS = 2000;
}

class AieClientSyncProcessShareMemoryFunctionTest : public testing::Test {};

/**
 * Tests AieClientSyncProcess().
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * isSyncProcessInputInfoNull  Whether the input parameter InputInfo of AieClientSyncProcess is null or not.
 * isSyncProcessSuccess  Whether the expected result of AieClientSyncProcess is successful or not.
 * isExpectedSyncProcessOutputInfoNotNull Whether the expected cb of AieClientSyncProcess is not null or not.
*/
static void TestAieClientSyncProcess(bool isAsync, bool isSyncProcessInputInfoNull, bool isSyncProcessSuccess,
    bool isExpectedSyncProcessOutputInfoNotNull)
{
    // Step 0: Defines variables.
    ConfigInfo configInfo;
    GetConfigInfo(configInfo);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    AlgorithmInfo algorithmInfo;
    int algorithmType = isAsync ? ALGORITHM_TYPE_ASYNC : ALGORITHM_TYPE_SYNC;
    GetSyncAlgorithmInfo(algorithmInfo, isAsync, algorithmType);

    ServiceDeadCb *cb = nullptr;
    AIE_NEW(cb, ServiceDeadCb());

    // Step 1: Invokes AieClientInit.
    int initReturnCode = AieClientInit(configInfo, clientInfo, algorithmInfo, cb);
    EXPECT_EQ(RETCODE_SUCCESS, initReturnCode) << "AieClientInit is failed!";
    EXPECT_EQ(true, clientInfo.clientId > 0) << "clientId(" << std::to_string(clientInfo.clientId) << ") is incorrect!";
    EXPECT_EQ(true, clientInfo.sessionId > 0) << "sessionId(" << std::to_string(clientInfo.sessionId)
                                              << ") is incorrect!";

    // Step 2: Invokes AieClientPrepare.
    bool isPrepareInputInfoNull = GetRandomBool();
    DataInfo prepareInputInfo = GetDataInfo(isPrepareInputInfoNull, INPUT_INFO_PREPARE);
    DataInfo prepareOutputInfo = GetDataInfo();

    ClientCallback *callback = nullptr;
    if (isAsync) {
        AIE_NEW(callback, ClientCallback());
    }

    int prepareReturnCode = AieClientPrepare(clientInfo, algorithmInfo, prepareInputInfo, prepareOutputInfo, callback);
    EXPECT_EQ(RETCODE_SUCCESS, prepareReturnCode) << "AieClientPrepare is failed!";
    EXPECT_EQ(true, prepareOutputInfo.data != nullptr) << "Prepare outputInfo is incorrect!";

    // Step 3: Invokes AieClientSyncProcess.
    DataInfo syncProcessInputInfo = GetBigDataInfo(isSyncProcessInputInfoNull);
    DataInfo syncProcessOutputInfo = GetDataInfo();
    int syncProcessReturnCode = AieClientSyncProcess(clientInfo, algorithmInfo, syncProcessInputInfo,
        syncProcessOutputInfo);
    EXPECT_EQ(isSyncProcessSuccess, syncProcessReturnCode == RETCODE_SUCCESS) << "AieClientSyncProcess is failed!";
    EXPECT_EQ(isExpectedSyncProcessOutputInfoNotNull, syncProcessOutputInfo.data != nullptr)
        << "AieClientSyncProcess outputInfo is incorrect!";

    // Step 4: Sleeps.
    StepSleepMs(WAIT_CALLBACK_TIME_MS);

    // Step 5: Invokes AieClientRelease.
    DataInfo releaseInputInfo = GetDataInfo(false, INPUT_INFO_RELEASE);
    int releaseReturnCode =  AieClientRelease(clientInfo, algorithmInfo, releaseInputInfo);
    EXPECT_EQ(RETCODE_SUCCESS, releaseReturnCode) << "AieClientRelease is failed!";

    // Step 6: Invokes AieClientDestroy.
    int destroyReturnCode =  AieClientDestroy(clientInfo);
    EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";

    // Step 7: Deletes callback.
    AIE_DELETE(cb);
    AIE_DELETE(callback);

    // Step 8: Release data info.
    FreeDataInfo(syncProcessInputInfo);
    FreeDataInfo(syncProcessOutputInfo);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSyncProcessShareMemory_0100
 * @tc.name   : 01. algorithmInfo中isAsync=false，inputInfo不为空，调用AieClientSyncProcess返回成功_共享内存传大数据
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSyncProcessShareMemoryFunctionTest, testAieClientSyncProcessShareMemoryFunction0101,
    Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSyncProcessShareMemoryFunction0101.");
    TestAieClientSyncProcess(false, false, true, true);
}