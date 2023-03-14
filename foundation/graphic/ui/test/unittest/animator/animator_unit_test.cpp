/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "animator/animator_manager.h"

#include <climits>
#include <gtest/gtest.h>

#include "animator/easing_equation.h"
#include "common/task_manager.h"

using namespace testing::ext;
namespace OHOS {
namespace {
const int16_t START_POS = 0;
const int16_t END_POS = 100;
const uint16_t TIME = 300;
} // namespace
class TestAnimatorCallback : public AnimatorCallback {
public:
    explicit TestAnimatorCallback(UIView* view) : view_(view), animator_(nullptr) {}

    virtual ~TestAnimatorCallback()
    {
        if (animator_ != nullptr) {
            delete animator_;
            animator_ = nullptr;
        }
    }

    bool Init()
    {
        if (animator_ == nullptr) {
            animator_ = new Animator(this, view_, TIME, false);
            if (animator_ == nullptr) {
                return false;
            }
            return true;
        }
        return false;
    }

    void Callback(UIView* view) override
    {
        int16_t pos = EasingEquation::LinearEaseNone(START_POS, END_POS, animator_->GetRunTime(), animator_->GetTime());
        view_->SetX(pos);
    }

    void OnStop(UIView& view) override
    {
        view_->SetX(END_POS);
    }

    Animator* GetAnimator() const
    {
        return animator_;
    }

protected:
    UIView* view_;
    Animator* animator_;
};

class AnimatorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static Animator* animator_;
};

Animator* AnimatorTest::animator_ = nullptr;

void AnimatorTest::SetUpTestCase(void)
{
    if (animator_ == nullptr) {
        animator_ = new Animator();
    }
}

void AnimatorTest::TearDownTestCase(void)
{
    if (animator_ != nullptr) {
        delete animator_;
        animator_ = nullptr;
    }
}

/**
 * @tc.name: AnimatorGetState_001
 * @tc.desc: Verify Start function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(AnimatorTest, AnimatorGetState_001, TestSize.Level1)
{
    if (animator_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    animator_->Start();
    EXPECT_EQ(animator_->GetState(), Animator::START);
    animator_->Stop();
    EXPECT_EQ(animator_->GetState(), Animator::STOP);
    animator_->Pause();
    EXPECT_EQ(animator_->GetState(), Animator::PAUSE);
    animator_->Resume();
    EXPECT_EQ(animator_->GetState(), Animator::START);
}

/**
 * @tc.name: AnimatorSetState_001
 * @tc.desc: Verify SetState function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(AnimatorTest, AnimatorSetState_001, TestSize.Level1)
{
    if (animator_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    animator_->SetState(Animator::START);
    EXPECT_EQ(animator_->GetState(), Animator::START);
    animator_->SetState(Animator::STOP);
    EXPECT_EQ(animator_->GetState(), Animator::STOP);
    animator_->SetState(Animator::PAUSE);
    EXPECT_EQ(animator_->GetState(), Animator::PAUSE);
}

/**
 * @tc.name: AnimatorSetTime_001
 * @tc.desc: Verify SetTime function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(AnimatorTest, AnimatorSetTime_001, TestSize.Level1)
{
    if (animator_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    animator_->SetTime(TIME);
    EXPECT_EQ(animator_->GetTime(), TIME);
}

/**
 * @tc.name: AnimatorSetRunTime_001
 * @tc.desc: Verify SetRunTime function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(AnimatorTest, AnimatorSetRunTime_001, TestSize.Level1)
{
    if (animator_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    animator_->SetRunTime(TIME);
    EXPECT_EQ(animator_->GetRunTime(), TIME);
}

/**
 * @tc.name: AnimatorIsRepeat_001
 * @tc.desc: Verify IsRepeat function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(AnimatorTest, AnimatorIsRepeat_001, TestSize.Level1)
{
    if (animator_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(animator_->IsRepeat(), false);
}

/**
 * @tc.name: AnimatorManagerAddAndRemove_001
 * @tc.desc: Verify AddAndRemove function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(AnimatorTest, AnimatorManagerAddAndRemove_001, TestSize.Level0)
{
    UIView* view = new UIView();
    view->SetX(START_POS);
    auto callback = new TestAnimatorCallback(view);
    if (!callback->Init()) {
        EXPECT_NE(0, 0);
        return;
    }
    Animator* animator = callback->GetAnimator();
    AnimatorManager::GetInstance()->Init();
    animator->Start();
    EXPECT_EQ(animator->GetState(), Animator::START);
    TaskManager::GetInstance()->SetTaskRun(true);
    while (1) {
        TaskManager::GetInstance()->TaskHandler();
        if (animator->GetState() == Animator::STOP) {
            break;
        }
    }
    EXPECT_EQ(view->GetX(), END_POS);

    view->SetX(START_POS);
    animator->Start();
    EXPECT_EQ(animator->GetState(), Animator::START);
    for (uint16_t i = 0; i < TIME; i++) {
        TaskManager::GetInstance()->TaskHandler();
    }
    EXPECT_EQ(view->GetX(), START_POS);
    TaskManager::GetInstance()->SetTaskRun(false);
    TaskManager::GetInstance()->Remove(AnimatorManager::GetInstance());
    delete callback;
    delete view;
}
} // namespace OHOS
