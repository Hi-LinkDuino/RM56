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

#include <perf.h>
#include "sleep_ex.h"

#include <gtest/gtest.h>

using namespace testing::ext;
using namespace OHOS::TestAW;

static BaseLineManager m_baseline(PERF_BASELINE_CONFIG_PATH);
class SpentTimeTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp() {}
    void TearDown() {}
};

static void LoopMsleep(void* pMsec)
{
    if (pMsec == nullptr) {
        return;
    }

    int msec = *reinterpret_cast<int*> (pMsec);
    for (int index = 0; index < msec; index++) {
        Msleep(1);
    }
}

/**
 * @tc.name: SpentTime001
 * @tc.desc: Test spent time by ExpectSmaller.
 * @tc.type: PERF
 * @tc.require: AR000CQGNT
 */
HWTEST_F(SpentTimeTest, SpentTime001, TestSize.Level3)
{
    /**
     * @tc.steps: step1. check result is smaller than or equal baseline.
     * @tc.expected: step1. return true
     */

    double spentTime = 0;
    int millionsecond = 1000;

    GtestPerfTestCase ptc(&m_baseline, this, 1);
    spentTime = ElapsedTime(LoopMsleep, &millionsecond) / 1000;
    ptc.ExpectSmaller(spentTime);
}

/**
 * @tc.name: SpentTime002
 * @tc.desc: Test spent time by ExpectLarger.
 * @tc.type: PERF
 * @tc.require: AR000CQGNT
 */
HWTEST_F(SpentTimeTest, SpentTime002, TestSize.Level3)
{
    /**
     * @tc.steps: step1. check result is larger than or equal baseline
     * @tc.expected: step1. return true
     */

    double spentTime = 0;
    int millionsecond = 1000;
    
    GtestPerfTestCase ptc(&m_baseline, this, 1); // case_version is 1
    spentTime = ElapsedTime(LoopMsleep, &millionsecond) / 1000;
    ptc.ExpectLarger(spentTime);
}

/**
 * @tc.name: SpentTime003
 * @tc.desc: Test spent time by GetBaseLine.
 * @tc.type: PERF
 * @tc.require: AR000CQGNT
 */
HWTEST_F(SpentTimeTest, SpentTime003, TestSize.Level3)
{
    /**
     * @tc.steps: step1. check result is smaller than or equal baseline.
     * @tc.expected: step1. return true
     */
    double spentTime = 0;
    int millionsecond = 100;

    GtestPerfTestCase ptc(&m_baseline, this, 1); //case_version is 1
    spentTime = ElapsedTime(LoopMsleep, &millionsecond) / 1000;
    ptc.SaveResult(spentTime);

    if (ptc.HasBaseLine()) {
        EXPECT_LE(spentTime, ptc.GetBaseLine() * (1 + ptc.GetFloatRange()));
    } else {
        EXPECT_TRUE(ptc.HasBaseLine());
    }
}

/**
 * @tc.name: SpentTime004
 * @tc.desc: Test spent time by caseVersion.
 * @tc.type: PERF
 * @tc.require: AR000CQGNT
 */
HWTEST_F(SpentTimeTest, SpentTime004, TestSize.Level3)
{
    /**
     * @tc.steps: step1. check result is smaller than or equal baseline.
     * @tc.expected: step1. return true
     */

    double spentTime = 0;
    int millionsecond = 100;

    GtestPerfTestCase ptc(&m_baseline, this, 2, "", "ElapsedTime");
    spentTime = ElapsedTime(LoopMsleep, &millionsecond) / 1000;
    ptc.ExpectSmaller(spentTime);
}
