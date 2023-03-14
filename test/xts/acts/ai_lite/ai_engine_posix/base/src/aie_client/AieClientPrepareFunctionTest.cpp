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
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "server_executor/include/server_executor.h"
#include "utils/aie_client_callback.h"
#include "utils/aie_client_common.h"
#include "utils/aie_client_const.h"
#include "utils/aie_macros.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"
#include "utils/service_dead_cb.h"
#include "utils/utils.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

class AieClientPrepareFunctionTest : public testing::Test {};

/**
 * Tests the input parameters algorithmInfo and inputInfo in AieClientPrepare().
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * isPrepareInputInfoNull  Whether the input parameter InputInfo of AieClientPrepare is null or not.
 * isPrepareCbNull Whether the cb input parameter cb of AieClientPrepare is null or not.
 * isPrepareSuccess Whether the expected result of AieClientPrepare is successful or not.
 * isOutputInfoDataCorrect Whether the expected OutputInfoData of AieClientPrepare is correct or not.
*/
static void TestAieClientPrepare(bool isAsync, bool isPrepareInputInfoNull, bool isPrepareCbNull,
    bool isPrepareSuccess, bool isOutputInfoDataCorrect)
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
    DataInfo inputInfo = {
        .data = nullptr,
        .length = 0,
    };
    if (!isPrepareInputInfoNull) {
        const char *str = INPUT_INFO_PREPARE;
        char *inputData = const_cast<char*>(str);
        int length = strlen(str) + 1;

        inputInfo = {
            .data = (unsigned char *) inputData,
            .length = length,
        };
    }

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0,
    };

    ClientCallback *callback = nullptr;
    if (!isPrepareCbNull) {
        AIE_NEW(callback, ClientCallback());
    }
    int prepareReturnCode = AieClientPrepare(clientInfo, algorithmInfo, inputInfo, outputInfo, callback);
    EXPECT_EQ(isPrepareSuccess, prepareReturnCode == RETCODE_SUCCESS) << "AieClientPrepare is failed!";
    EXPECT_EQ(isOutputInfoDataCorrect, outputInfo.data != nullptr) << "Prepare outputInfo is incorrect!";

    // Step 3: Invokes AieClientRelease.
    if (prepareReturnCode == RETCODE_SUCCESS) {
        DataInfo releaseInputInfo = GetDataInfo(false, INPUT_INFO_RELEASE);
        int releaseReturnCode =  AieClientRelease(clientInfo, algorithmInfo, releaseInputInfo);
        EXPECT_EQ(RETCODE_SUCCESS, releaseReturnCode) << "AieClientRelease is failed!";
    }

    // Step 4: Invokes AieClientDestroy.
    if (initReturnCode == RETCODE_SUCCESS) {
        int destroyReturnCode =  AieClientDestroy(clientInfo);
        EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";
    }

    // Step 5: Deletes callback.
    AIE_DELETE(cb);
    if (!isPrepareCbNull) {
        AIE_DELETE(callback);
    }
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Sync_0100
 * @tc.name   : 01. algorithmInfo中isAsync=false，inputInfo不为空，cb为空，调用AieClientPrepare返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0101, Function | MediumTest | Level2)
{
    HILOGI("[Test]testAieClientPrepareFunction0101.");
    TestAieClientPrepare(false, false, true, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Sync_0200
 * @tc.name   : 02. algorithmInfo中isAsync=false，inputInfo为空，cb为空，调用AieClientPrepare返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0102, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0102.");
    TestAieClientPrepare(false, true, true, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Sync_0300
 * @tc.name   : 03. algorithmInfo中isAsync=false，inputInfo不为空，cb不为空，调用AieClientPrepare返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0103, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0103.");
    TestAieClientPrepare(false, false, false, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Sync_0400
 * @tc.name   : 04. algorithmInfo中isAsync=false，inputInfo为空，cb不为空，调用AieClientPrepare返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0104, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0104.");
    TestAieClientPrepare(false, true, false, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Async_0100
 * @tc.name   : 01. algorithmInfo中isAsync=true，inputInfo不为空，cb不为空，调用AieClientPrepare返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0201, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0201.");
    TestAieClientPrepare(true, false, false, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Async_0200
 * @tc.name   : 02. algorithmInfo中isAsync=true，inputInfo为空，cb不为空，调用AieClientPrepare返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0202, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0202.");
    TestAieClientPrepare(true, true, false, true, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Async_0300
 * @tc.name   : 03. algorithmInfo中isAsync=true，inputInfo不为空，cb为空，调用AieClientPrepare返回失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0203, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0203.");
    TestAieClientPrepare(true, false, true, false, false);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientPrepare_Async_0400
 * @tc.name   : 04. algorithmInfo中isAsync=true，inputInfo为空，cb为空，调用AieClientPrepare返回失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientPrepareFunctionTest, testAieClientPrepareFunction0204, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientPrepareFunction0204.");
    TestAieClientPrepare(true, true, true, false, false);
}