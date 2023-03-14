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
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include "client_executor/include/i_aie_client.inl"
#include "utils/aie_client_common.h"
#include "utils/aie_client_const.h"
#include "utils/service_dead_cb.h"
#include "utils/utils.h"

using namespace ::testing;
using namespace testing::ext;
using namespace OHOS::AI;

namespace {
    const int DESCRIPTION_LENGTH_20 = 21;
    const int DESCRIPTION_LENGTH_128 = 129;
}

class AieClientInitFunctionTest : public testing::Test {};

/**
 * Tests AieClientInit.
 *
 * configDesc  The value or the input parameter ConfigInfo.description of AieClientInit.
 * isAsync  The value or the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * bool isDeadCbNotNull Whether the dead callback parameter is not null.
 */
static void TestAieClientInit(const char *configDesc, bool isAsync, bool isDeadCbNotNull)
{
    // Step 0: Defines variables.
    ConfigInfo configInfo;
    GetConfigInfo2(configInfo, configDesc);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    AlgorithmInfo algorithmInfo;
    int algorithmType = isAsync ? ALGORITHM_TYPE_ASYNC : ALGORITHM_TYPE_SYNC;
    GetSyncAlgorithmInfo(algorithmInfo, isAsync, algorithmType);

    ServiceDeadCb *cb = nullptr;
    if (isDeadCbNotNull) {
        AIE_NEW(cb, ServiceDeadCb());
    }

    // Step 1: Invokes AieClientInit.
    int initReturnCode = AieClientInit(configInfo, clientInfo, algorithmInfo, cb);
    EXPECT_EQ(RETCODE_SUCCESS, initReturnCode) << "AieClientInit is failed!";
    EXPECT_EQ(true, clientInfo.clientId > 0) << "clientId(" << std::to_string(clientInfo.clientId) << ") is incorrect!";
    EXPECT_EQ(true, clientInfo.sessionId > 0) << "sessionId(" << std::to_string(clientInfo.sessionId)
                                              << ") is incorrect!";

    // Step 2: Invoke AieClientDestroy().
    int destroyReturnCode = AieClientDestroy(clientInfo);
    EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";

    AIE_DELETE(cb);
}

/**
 * @tc.number    : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientInit_configInfo_0100
 * @tc.name      : 01. configInfo中的description设置为20位字符串，调用AieClientInit返回成功
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientInitFunctionTest, testAieClientInitFunction0101, Function | MediumTest | Level1)
{
    HILOGI("[Test]testAieClientInitFunction0101.");
    char configDesc[DESCRIPTION_LENGTH_20];
    GetRandomString(DESCRIPTION_LENGTH_20, configDesc);
    bool isAsync = GetRandomBool();
    TestAieClientInit(configDesc, isAsync, true);
}

/**
 * @tc.number    : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientInit_configInfo_0200
 * @tc.name      : 02. configInfo中的description设置为128位字符串，调用AieClientInit返回成功
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientInitFunctionTest, testAieClientInitFunction0102, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientInitFunction0102.");
    char configDesc[DESCRIPTION_LENGTH_128];
    GetRandomString(DESCRIPTION_LENGTH_128, configDesc);
    bool isAsync = GetRandomBool();
    TestAieClientInit(configDesc, isAsync, true);
}

/**
 * @tc.number    : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientInit_configInfo_0300
 * @tc.name      : 03. configInfo中的description设置为空，调用AieClientInit返回成功
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientInitFunctionTest, testAieClientInitFunction0103, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientInitFunction0103.");
    const char* configDesc = "";
    bool isAsync = GetRandomBool();
    TestAieClientInit(configDesc, isAsync, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientInit_algorithmInfo_0100
 * @tc.name   : 01. algorithmInfo中isAsync=false，调用AieClientInit返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientInitFunctionTest, testAieClientInitFunction0201, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientInitFunction0201.");
    TestAieClientInit(CONFIG_DESCRIPTION, false, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientInit_algorithmInfo_0200
 * @tc.name   : 02. algorithmInfo中isAsync=true，调用AieClientInit返回成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientInitFunctionTest, testAieClientInitFunction0202, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientInitFunction0202.");
    TestAieClientInit(CONFIG_DESCRIPTION, true, true);
}

/**
 * @tc.number    : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientInit_cb_0100
 * @tc.name      : 01. cb=空，调用AieClientInit返回成功
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientInitFunctionTest, testAieClientInitFunction0301, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientInitFunction0301.");
    bool isAsync = GetRandomBool();
    TestAieClientInit(CONFIG_DESCRIPTION, isAsync, false);
}
