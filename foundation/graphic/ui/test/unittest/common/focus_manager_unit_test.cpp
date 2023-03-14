/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_label.h"
#include "dock/focus_manager.h"

#include <climits>
#include <gtest/gtest.h>

#if ENABLE_FOCUS_MANAGER
using namespace testing::ext;
namespace OHOS {
class FocusManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIViewGroup* viewGroup_;
    static UILabel* label1_;
    static UILabel* label2_;
};

UIViewGroup* FocusManagerTest::viewGroup_ = nullptr;
UILabel* FocusManagerTest::label1_ = nullptr;
UILabel* FocusManagerTest::label2_ = nullptr;

void FocusManagerTest::SetUpTestCase(void)
{
    if (viewGroup_ == nullptr) {
        viewGroup_ = new UIViewGroup();
        viewGroup_->SetPosition(0, 0, 200, 200); // 200: width, 200: height
    }
    if (label1_ == nullptr) {
        label1_ = new UILabel();
        label1_->SetFocusable(true);
        label1_->SetPosition(50, 50, 50, 50); // 50: x, 50: y, 50: width, 50: height
    }
    if (label2_ == nullptr) {
        label2_ = new UILabel();
        label2_->SetFocusable(true);
        label2_->SetPosition(150, 150, 50, 50); // 150: x, 150: y, 50: width, 50: height
    }
    viewGroup_->Add(label1_);
    viewGroup_->Add(label2_);
}

void FocusManagerTest::TearDownTestCase(void)
{
    FocusManager::GetInstance()->ClearFocus();
    if (viewGroup_ != nullptr) {
        viewGroup_->Remove(label1_);
        viewGroup_->Remove(label2_);
        delete viewGroup_;
        viewGroup_ = nullptr;
    }
    if (label1_ != nullptr) {
        delete label1_;
        label1_ = nullptr;
    }
    if (label2_ != nullptr) {
        delete label2_;
        label2_  = nullptr;
    }
}

/**
 * @tc.name: GetFocusedView_001
 * @tc.desc: Verify GetFocusedView function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2R
 */
HWTEST_F(FocusManagerTest, GetFocusedView_001, TestSize.Level0)
{
    UIView* focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, nullptr);
}

/**
 * @tc.name: RequestFocus_001
 * @tc.desc: Verify RequestFocus function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2R
 */
HWTEST_F(FocusManagerTest, RequestFocus_001, TestSize.Level1)
{
    FocusManager::GetInstance()->RequestFocus(label1_);
    UIView* focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label1_);
    FocusManager::GetInstance()->RequestFocus(label2_);
    focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label2_);
}

/**
 * @tc.name: ClearFocus_001
 * @tc.desc: Verify ClearFocus function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2R
 */
HWTEST_F(FocusManagerTest, ClearFocus_001, TestSize.Level1)
{
    FocusManager::GetInstance()->RequestFocus(label1_);
    UIView* focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label1_);
    FocusManager::GetInstance()->ClearFocus();
    focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, nullptr);
}

/**
 * @tc.name: RequestFocusByDirection_001
 * @tc.desc: Verify RequestFocusByDirection function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EVI2R
 */
HWTEST_F(FocusManagerTest, RequestFocusByDirection_001, TestSize.Level0)
{
    FocusManager::GetInstance()->RequestFocus(label1_);
    UIView* focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label1_);
    FocusManager::GetInstance()->RequestFocusByDirection(FOCUS_DIRECTION_RIGHT);
    focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label2_);
    FocusManager::GetInstance()->RequestFocusByDirection(FOCUS_DIRECTION_LEFT);
    focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label1_);
    FocusManager::GetInstance()->RequestFocusByDirection(FOCUS_DIRECTION_DOWN);
    focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label2_);
    FocusManager::GetInstance()->RequestFocusByDirection(FOCUS_DIRECTION_UP);
    focusedView = FocusManager::GetInstance()->GetFocusedView();
    EXPECT_EQ(focusedView, label1_);
}
} // namespace OHOS
#endif
