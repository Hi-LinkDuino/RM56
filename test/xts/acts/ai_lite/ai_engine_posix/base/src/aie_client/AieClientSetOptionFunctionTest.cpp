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
    const int  g_setGetFirst = 1;
    const int  g_setGetSecond = 2;
    const int  g_setGetThird = 3;
    const int  INT_LENGTH = 65535;
}

class AieClientSetOptionFunctionTest : public testing::Test {};

/**
 * Tests TestAieClientSetOption.
 * Invoke sequence: AieClientInit-AieClientPrepare-AieClientSetOption-AieClientGetOption.
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * setAndGetTimes The attempts for invoking AieClientSetOption and AieClientGetOption.
 */
static void TestAieClientSetOption(bool isAsync, int setAndGetTimes)
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

    IClientCb *callback = nullptr;
    if (isAsync) {
        AIE_NEW(callback, ClientCallback());
    }
    int prepareReturnCode = AieClientPrepare(clientInfo, algorithmInfo, prepareInputInfo, prepareOutputInfo, callback);
    EXPECT_EQ(RETCODE_SUCCESS, prepareReturnCode) << "AieClientPrepare is failed!";
    EXPECT_EQ(true, prepareOutputInfo.data != nullptr) << "Prepare outputInfo is incorrect!";

    // Step 3: Invokes AieClientSetOption and AieClientGetOption.
    int optionType = GetRandomInt(INT_LENGTH);
    for (int i = 0; i < setAndGetTimes; i++) {
        // Step 3.1: Invokes AieClientSetOption.
        std::string setOptionString = std::string(INPUT_INFO_SET_OPTION) + "_" + std::to_string(i + 1);
        char *setOptionInputData = const_cast<char*>(setOptionString.c_str());
        int setOptionLength = strlen(setOptionInputData) + 1;

        DataInfo setOptionInputInfo = {
            .data = (unsigned char*)setOptionInputData,
            .length = setOptionLength,
        };

        int setOptionReturnCode = AieClientSetOption(clientInfo, optionType, setOptionInputInfo);
        EXPECT_EQ(RETCODE_SUCCESS, setOptionReturnCode) << "AieClientSetOption is failed!";

        // Step 3.2: Invokes AieClientGetOption.
        bool isGetOptionInputInfoNull = GetRandomBool();
        DataInfo getOptionInputInfo = GetDataInfo(isGetOptionInputInfoNull, INPUT_INFO_GET_OPTION);
        DataInfo getOptionOutputInfo = GetDataInfo();

        int getOptionReturnCode = AieClientGetOption(clientInfo, optionType, getOptionInputInfo, getOptionOutputInfo);
        EXPECT_EQ(RETCODE_SUCCESS, getOptionReturnCode) << "AieClientSetOption is failed!";

        std::string setOptionInputDataToString = "";
        setOptionInputDataToString.append(reinterpret_cast<const char*>(setOptionInputInfo.data));
        std::string getOptionOutputInfoToString = "";
        getOptionOutputInfoToString.append(reinterpret_cast<const char*>(getOptionOutputInfo.data));
        EXPECT_EQ(setOptionInputDataToString, getOptionOutputInfoToString) << "GetOption outputInfo is incorrect!";
    }

    // Step 4: Invokes AieClientRelease.
    if (prepareReturnCode == RETCODE_SUCCESS) {
        bool isReleaseInputInfoNull = GetRandomBool();
        DataInfo releaseInputInfo = GetDataInfo(isReleaseInputInfoNull, INPUT_INFO_RELEASE);
        int releaseReturnCode = AieClientRelease(clientInfo, algorithmInfo, releaseInputInfo);
        EXPECT_EQ(RETCODE_SUCCESS, releaseReturnCode) << "AieClientRelease is failed!";
    }

    // Step5: Invoke AieClientDestroy().
    if (initReturnCode == RETCODE_SUCCESS) {
        int destroyReturnCode = AieClientDestroy(clientInfo);
        EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";
    }
    // Step 6: Deletes callback.
    AIE_DELETE(cb);
    AIE_DELETE(callback);
}

/**
 * Tests TestAieClientSetOption.
 * Invoke sequence: AieClientInit-AieClientSetOption-AieClientGetOption.
 *
 * isAsync  The value of the input parameter AlgorithmInfo.isAsync of AieClientInit.
 * isPerformSetOption Whether to invoke AieClientSetOption or not.
 */
static void TestAieClientSetOptionWithoutPrepare(bool isAsync, bool isPerformSetOption)
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

    // Step 3: Invokes AieClientSetOption.
    int optionType = 1;
    if (isPerformSetOption) {
        const char *setOptionString = INPUT_INFO_SET_OPTION;
        char *setOptionInputData = const_cast<char*>(setOptionString);
        int setOptionLength = strlen(setOptionString) + 1;
        DataInfo inputInfo2 = {
            .data = (unsigned char*)setOptionInputData,
            .length = setOptionLength,
        };
        int setOptionReturnCode = AieClientSetOption(clientInfo, optionType, inputInfo2);
        EXPECT_EQ(true, setOptionReturnCode != RETCODE_SUCCESS) << "AieClientSetOption is failed!";
    }

    // Step 4: Invokes AieClientGetOption.
    bool isGetOptionInputInfoNull = GetRandomBool();
    DataInfo getOptionInputInfo = GetDataInfo(isGetOptionInputInfoNull, INPUT_INFO_GET_OPTION);
    DataInfo getOptionOutputInfo = GetDataInfo();

    int getOptionReturnCode = AieClientGetOption(clientInfo, optionType, getOptionInputInfo, getOptionOutputInfo);
    EXPECT_EQ(true, getOptionReturnCode != RETCODE_SUCCESS) << "AieClientSetOption is failed!";
    EXPECT_EQ(true, getOptionOutputInfo.data == nullptr) << "GetOption outputInfo is incorrect!";

    // Step5: Invoke AieClientDestroy().
    if (initReturnCode == RETCODE_SUCCESS) {
        int destroyReturnCode = AieClientDestroy(clientInfo);
        EXPECT_EQ(RETCODE_SUCCESS, destroyReturnCode) << "AieClientDestroy is failed!";
    }
    // Step 6: Deletes callback.
    AIE_DELETE(cb);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSetOption_01_0100
 * @tc.name   : 01. 加载算法插件之后，执行1次设置参数值和获取参数值，成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSetOptionFunctionTest, testAieClientSetOptionFunction0101, Function | MediumTest | Level2)
{
    HILOGI("[Test]testAieClientSetOptionFunction0101.");
    bool isAsync = GetRandomBool();
    TestAieClientSetOption(isAsync, g_setGetFirst);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSetOption_01_0200
 * @tc.name   : 02. 加载算法插件之后，执行2次设置参数值和获取参数值，成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSetOptionFunctionTest, testAieClientSetOptionFunction0102, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSetOptionFunction0102.");
    bool isAsync = GetRandomBool();
    TestAieClientSetOption(isAsync, g_setGetSecond);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSetOption_01_0300
 * @tc.name   : 03. 加载算法插件之后，执行3次设置参数值和获取参数值，成功
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSetOptionFunctionTest, testAieClientSetOptionFunction0103, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSetOptionFunction0103.");
    bool isAsync = GetRandomBool();
    TestAieClientSetOption(isAsync, g_setGetThird);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSetOption_02_0100
 * @tc.name   : 01. 不加载算法插件，设置参数值，获取参数值，失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSetOptionFunctionTest, testAieClientSetOptionFunction0201, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSetOptionFunction0201.");
    bool isAsync = GetRandomBool();
    TestAieClientSetOptionWithoutPrepare(isAsync, true);
}

/**
 * @tc.number : SUB_AI_AIDistributedAbility_HiAiEngine_Lite_Function_HiAiAPI_AIEClient_AieClientSetOption_02_0200
 * @tc.name   : 02. 不加载算法插件，获取参数值，失败
 * @tc.desc   : [C- SOFTWARE -0200]
 */
HWTEST_F(AieClientSetOptionFunctionTest, testAieClientSetOptionFunction0202, Function | MediumTest | Level3)
{
    HILOGI("[Test]testAieClientSetOptionFunction0202.");
    bool isAsync = GetRandomBool();
    TestAieClientSetOptionWithoutPrepare(isAsync, false);
}