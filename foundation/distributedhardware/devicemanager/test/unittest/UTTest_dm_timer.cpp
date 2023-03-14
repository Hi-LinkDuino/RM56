/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <string>
#include <unistd.h>

#include "dm_constants.h"
#include "dm_timer.h"
#include "UTTest_dm_timer.h"

namespace OHOS {
namespace DistributedHardware {
void DmTimerTest::SetUp()
{
}

void DmTimerTest::TearDown()
{
}

void DmTimerTest::SetUpTestCase()
{
}

void DmTimerTest::TearDownTestCase()
{
}

namespace {
/**
 * @tc.name: DmTimerTest::DmTimer_001
 * @tc.desc: to check when name is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, DmTimer_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = "";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
}

/**
 * @tc.name: DmTimerTest::DmTimer_002
 * @tc.desc: to check when name is not empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, DmTimer_002, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "123";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    EXPECT_EQ(DmTimerStatus::DM_STATUS_INIT, Timer_->mStatus_);
    EXPECT_EQ(0, Timer_->mTimeOutSec_);
    EXPECT_EQ(nullptr, Timer_->mHandle_);
    EXPECT_EQ(nullptr, Timer_->mHandleData_);
    EXPECT_EQ(0, Timer_->mTimeFd_[1]);
    EXPECT_EQ(0, Timer_->mEv_.events);
    EXPECT_EQ(0, Timer_->mEvents_[0].events);
    EXPECT_EQ(0, Timer_->mEpFd_);
    EXPECT_EQ(DM_TIMER_TASK, Timer_->mTimerName_);
}

/**
 * @tc.name: DmTimerTest::Start_001
 * @tc.desc: to check when mTimerName_ is empty,handle is nullptr,data is nullptr
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, Start_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = "";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    uint32_t timeOut = 10;
    TimeoutHandle handle = nullptr;
    void *data = nullptr;
    DmTimerStatus timerStatus = Timer_->Start(timeOut, handle, data);
    EXPECT_EQ(DM_STATUS_FINISH, timerStatus);
}

/**
 * @tc.name: DmTimerTest::Start_002
 * @tc.desc: to check when mTimerName_ is not empty, handle is not nullptr, data is not nullptr,
 * @mStatus_ != DmTimerStatus::DM_STATUS_INIT
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
static void TimeOutTest(void *data, DmTimer& timer)
{
    LOGE("time out test");
}

HWTEST_F(DmTimerTest, Start_002, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "1234";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    uint32_t timeOut = 10;
    int idx = 1;
    void *data = &idx;
    Timer_->mStatus_ = DmTimerStatus::DM_STATUS_RUNNING;
    DmTimerStatus timerStatus = Timer_->Start(timeOut, TimeOutTest, data);
    EXPECT_EQ(DmTimerStatus::DM_STATUS_BUSY, timerStatus);
}

/**
 * @tc.name: DmTimerTest::Stop_001
 * @tc.desc: to check when mTimerName_ is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, Stop_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = "";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    int32_t code = 1;
    Timer_->Stop(code);
}

/**
 * @tc.name: DmTimerTest::Stop_002
 * @tc.desc: to check when mTimerName_ is not empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, Stop_002, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "111";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    int32_t code = 1;
    Timer_->Stop(code);
}

/**
 * @tc.name: DmTimerTest::Stop_003
 * @tc.desc: to check when mTimerName_ is not empty,mTimeFd_[1] is not 0
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, Stop_003, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "111";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    int32_t code = 1;
    Timer_->mTimeFd_[1] = 1;
    Timer_->Stop(code);
}

/**
 * @tc.name: DmTimerTest::WaitForTimeout_001
 * @tc.desc: to check when mTimerName_ is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, WaitForTimeout_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = "";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    Timer_->WaitForTimeout();
}

/**
 * @tc.name: DmTimerTest::WaitForTimeout_002
 * @tc.desc: to check when mTimerName_ is not empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, WaitForTimeout_002, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "111";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    Timer_->WaitForTimeout();
}

/**
 * @tc.name: DmTimerTest::CreateTimeFd_001
 * @tc.desc: to check when mTimerName_ is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, CreateTimeFd_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = "";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    int32_t result = Timer_->CreateTimeFd();
    EXPECT_EQ(DM_STATUS_FINISH, result);
}

/**
 * @tc.name: DmTimerTest::CreateTimeFd_002
 * @tc.desc: to check when mTimerName_ is not empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, CreateTimeFd_002, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "123";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    Timer_->CreateTimeFd();
}

/**
 * @tc.name: DmTimerTest::Release_001
 * @tc.desc: to check when mTimerName_ is empty
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, Release_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = "";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    Timer_->Release();
}

/**
 * @tc.name: DmTimerTest::Release_002
 * @tc.desc: to check when mTimerName_ is not empty,mStatus_ is 0
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, Release_002, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "111";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    Timer_->mStatus_ = DmTimerStatus::DM_STATUS_INIT;
    Timer_->Release();
}

/**
 * @tc.name: DmTimerTest::GetTimerName_001
 * @tc.desc: to check whether the return value is the same as mTimerName_
 * @tc.type: FUNC
 * @tc.require: AR000GHSJK
 */
HWTEST_F(DmTimerTest, GetTimerName_001, testing::ext::TestSize.Level0)
{
    const std::string DM_TIMER_TASK = TIMER_PREFIX + "111";
    std::shared_ptr<DmTimer> Timer_ = std::make_shared<DmTimer>(DM_TIMER_TASK);
    std::string strTimer = Timer_->GetTimerName();
    EXPECT_EQ(DM_TIMER_TASK, strTimer);
}
}
}
}
