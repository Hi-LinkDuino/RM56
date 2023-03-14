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


#include <stdio.h>
#include "gtest/gtest.h"
#include "log.h"

#undef LOG_TAG
#undef LOG_DOMAIN
#define LOG_TAG "Test_TAG"
#define LOG_DOMAIN 0xD002D00

using namespace OHOS::HiviewDFX;
static constexpr HiLogLabel LABEL = {LOG_CORE, 0xD002D00, "Test_TAG"};
using namespace std;
using namespace testing::ext;
	
class HiLogTest : public testing::Test {
protected:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0100
 * @tc.name      : HiLog_DEBUG parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0100, Function | MediumTest | Level0)
{
    HILOG_DEBUG(LOG_CORE, "Debug log of HiLog API test");
}


/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0200
 * @tc.name      : HILOG_INFO parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0200, Function | MediumTest | Level1)
{
    HILOG_INFO(LOG_CORE, "Info log of HiLog API test");
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0300
 * @tc.name      : HILOG_WARN parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0300, Function | MediumTest | Level1)
{
    HILOG_WARN(LOG_CORE, "Warn log of HiLog API test");
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0400
 * @tc.name      : HILOG_ERROR parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0400, Function | MediumTest | Level1)
{
    HILOG_ERROR(LOG_CORE, "Error log of HiLog API test");
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0500
 * @tc.name      : HILOG_FATAL parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0500, Function | MediumTest | Level1)
{
    HILOG_FATAL(LOG_CORE, "Fatal log of HiLog API test");
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0600
 * @tc.name      : HiLog::Debug parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0600, Function | MediumTest | Level1)
{
    bool ret = true;
    ret = HiLog::Debug(LABEL, "Debug log of HiLog API test");
    EXPECT_TRUE(ret);
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0700
 * @tc.name      : HiLog::Error parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0700, Function | MediumTest | Level1)
{
    bool ret = true;
    ret = HiLog::Error(LABEL, "Error log of HiLog API test");
    EXPECT_TRUE(ret);
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0800
 * @tc.name      : HiLog::Fatal parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0800, Function | MediumTest | Level1)
{
    bool ret = true;
    ret = HiLog::Fatal(LABEL, "Fatal log of HiLog API test");
    EXPECT_TRUE(ret);
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_0900
 * @tc.name      : HiLog::Info parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi0900, Function | MediumTest | Level1)
{
    bool ret = true;
    ret = HiLog::Info(LABEL, "Info log of HiLog API test");
    EXPECT_TRUE(ret);
}

/**
 * @tc.number    : Sub_Dfx_Dft_HiLog_Cortexaapi_1000
 * @tc.name      : HiLog::Warn parameter legal test (Cortex-A, C)
 * @tc.desc      : [C- SOFTWARE -0200]
 */
HWTEST_F(HiLogTest, subDfxDftHiLogCortexaapi1000, Function | MediumTest | Level1)
{
    bool ret = true;
    ret = HiLog::Warn(LABEL, "Warn log of HiLog API test");
    EXPECT_TRUE(ret);
}
