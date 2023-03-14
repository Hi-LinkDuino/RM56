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
#include "communication_adapter/include/sa_async_handler.h"
#include "communication_adapter/include/sa_client.h"
#include "platform/time/include/time.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "server_executor/include/server_executor.h"
#include "utils/aie_client_callback.h"
#include "utils/aie_client_common.h"
#include "utils/aie_client_const.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"
#include "utils/service_dead_cb.h"
#include "utils/utils.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

namespace {
    const int WAIT_CALLBACK_TIME_MS = 2000;
}

class AieClientAsyncProcessFunctionTest : public testing::Test {};

/**
 * Tests AieClientAsyncProcess().
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * isAsyncProcessInputInfoNull  Whether the input parameter InputInfo of AieClientAsyncProcess is null or not.
 * isAsyncProcessSuccess  Whether the expected result of AieClientAsyncProcess is successful or not.
 * isExpectedAsyncProcessCbNotNull Whether the expected cb of AieClientAsyncProcess is not null or not.
*/
static void TestAieClientAsyncProcess(bool isAsync, bool isAsyncProcessInputInfoNull, bool isAsyncProcessSuccess,
    bool isExpectedAsyncProcessCbNotNull)
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

    // Step 3: Invokes AieClientAsyncProcess.
    DataInfo asyncProcessInputInfo = GetDataInfo(isAsyncProcessInputInfoNull, INPUT_INFO_ASYNC_PROCESS);
    int asyncProcessReturnCode = AieClientAsyncProcess(clientInfo, algorithmInfo, asyncProcessInputInfo);
    EXPECT_EQ(isAsyncProcessSuccess, asyncProcessReturnCode == 0) << "AieClientAsyncProcess is failed!";

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
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientAsyncProcess_01_0100
 * @tc.name   : 01. algorithmInfo中isAsync=true，inputInfo不为空，调用AieClientAsyncProcess返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientAsyncProcessFunctionTest, testAieClientAsyncProcessFunction0101, Function | MediumTest | Level2)
{
    HILOGI("[Test]testAieClientAsyncProcessFunction0101.");
    TestAieClientAsyncProcess(true, false, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientAsyncProcess_01_0200
 * @tc.name   : 02. algorithmInfo中isAsync=true，inputInfo为空，调用AieClientAsyncProcess返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientAsyncProcessFunctionTest, testAieClientAsyncProcessFunction0102, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientAsyncProcessFunction0102.");
    TestAieClientAsyncProcess(true, true, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientAsyncProcess_02_0100
 * @tc.name   : 01. algorithmInfo中isAsync=false，调用AieClientAsyncProcess返回失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientAsyncProcessFunctionTest, testAieClientAsyncProcessFunction0201, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientAsyncProcessFunction0201.");
    bool isAsyncProcessInputInfoNull = GetRandomBool();
    TestAieClientAsyncProcess(false, isAsyncProcessInputInfoNull, false, false);
}
