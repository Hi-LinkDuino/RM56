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

#include "components/ui_abstract_progress.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t MAX_VALUE = 80;
    const int16_t MEDIAN_VALUE = 50;
    const int16_t MIN_VALUE = 20;
}
class UIAbsatrctProgressTest : public testing::Test {
public:
    UIAbsatrctProgressTest() : abstractProgress_(nullptr) {}
    virtual ~UIAbsatrctProgressTest() {}
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    UIAbstractProgress* abstractProgress_;
};

void UIAbsatrctProgressTest::SetUpTestCase()
{
}

void UIAbsatrctProgressTest::TearDownTestCase()
{
}

void UIAbsatrctProgressTest::SetUp()
{
    if (abstractProgress_ == nullptr) {
        abstractProgress_ = new UIAbstractProgress();
    }
}

void UIAbsatrctProgressTest::TearDown()
{
    if (abstractProgress_ != nullptr) {
        delete abstractProgress_;
        abstractProgress_ = nullptr;
    }
}
/**
 * @tc.name: UIAbsatrctProgressGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressGetViewType_001, TestSize.Level1)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(abstractProgress_->GetViewType(), UI_ABSTRACT_PROGRESS);
}

/**
 * @tc.name: UIAbsatrctProgressSetValue_001
 * @tc.desc: Verify SetValue function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressSetValue_001, TestSize.Level0)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    abstractProgress_->SetRange(MAX_VALUE, MIN_VALUE);
    EXPECT_EQ(abstractProgress_->GetRangeMin(), MIN_VALUE);
    EXPECT_EQ(abstractProgress_->GetRangeMax(), MAX_VALUE);

    abstractProgress_->SetValue(MEDIAN_VALUE);
    EXPECT_EQ(abstractProgress_->GetValue(), MEDIAN_VALUE);

    abstractProgress_->SetValue(MAX_VALUE + 1);
    EXPECT_EQ(abstractProgress_->GetValue(), MAX_VALUE);

    abstractProgress_->SetValue(MIN_VALUE - 1);
    EXPECT_EQ(abstractProgress_->GetValue(), MIN_VALUE);
}

/**
 * @tc.name: UIAbsatrctProgressSetValue_002
 * @tc.desc: Verify SetValue function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressSetValue_002, TestSize.Level1)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    abstractProgress_->SetRange(MAX_VALUE, MIN_VALUE);
    EXPECT_EQ(abstractProgress_->GetRangeMin(), MIN_VALUE);
    EXPECT_EQ(abstractProgress_->GetRangeMax(), MAX_VALUE);

    abstractProgress_->SetRange(MIN_VALUE, MAX_VALUE);
    EXPECT_EQ(abstractProgress_->GetRangeMin(), MIN_VALUE);
    EXPECT_EQ(abstractProgress_->GetRangeMax(), MAX_VALUE);
}

/**
 * @tc.name: UIAbsatrctProgressSetStep_001
 * @tc.desc: Verify SetStep function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressSetStep_001, TestSize.Level1)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    abstractProgress_->SetStep(MEDIAN_VALUE);
    EXPECT_EQ(abstractProgress_->GetStep(), MEDIAN_VALUE);
}

/**
 * @tc.name: UIAbsatrctProgressSetBackgroundStyle_001
 * @tc.desc: Verify SetBackgroundStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressSetBackgroundStyle_001, TestSize.Level1)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    Style style;
    style.imageOpa_ = OPA_TRANSPARENT;
    style.lineOpa_ = OPA_TRANSPARENT;
    style.borderRadius_ = 1;

    abstractProgress_->SetBackgroundStyle(style);
    EXPECT_EQ(abstractProgress_->GetBackgroundStyle().imageOpa_, OPA_TRANSPARENT);
    EXPECT_EQ(abstractProgress_->GetBackgroundStyle().lineOpa_, OPA_TRANSPARENT);
    EXPECT_EQ(abstractProgress_->GetBackgroundStyle().borderRadius_, 1);

    abstractProgress_->SetBackgroundStyle(STYLE_BACKGROUND_COLOR, Color::Silver().full);
    EXPECT_EQ(abstractProgress_->GetBackgroundStyle(STYLE_BACKGROUND_COLOR), Color::Silver().full);
}

/**
 * @tc.name: UIAbsatrctProgressSetForegroundStyle_001
 * @tc.desc: Verify SetForegroundStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressSetForegroundStyle_001, TestSize.Level1)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    Style style;
    style.imageOpa_ = OPA_TRANSPARENT;
    style.lineOpa_ = OPA_TRANSPARENT;
    style.borderRadius_ = 1;

    abstractProgress_->SetForegroundStyle(style);
    EXPECT_EQ(abstractProgress_->GetForegroundStyle().imageOpa_, OPA_TRANSPARENT);
    EXPECT_EQ(abstractProgress_->GetForegroundStyle().lineOpa_, OPA_TRANSPARENT);
    EXPECT_EQ(abstractProgress_->GetForegroundStyle().borderRadius_, 1);

    abstractProgress_->SetForegroundStyle(STYLE_BACKGROUND_COLOR, Color::White().full);
    EXPECT_EQ(abstractProgress_->GetForegroundStyle(STYLE_BACKGROUND_COLOR), Color::White().full);
}

/**
 * @tc.name: UIAbsatrctProgressSetCapType_001
 * @tc.desc: Verify SetCapType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQG
 */
HWTEST_F(UIAbsatrctProgressTest, UIAbsatrctProgressSetCapType_001, TestSize.Level1)
{
    if (abstractProgress_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(abstractProgress_->GetForegroundStyle(STYLE_LINE_CAP), CapType::CAP_NONE);
    EXPECT_EQ(abstractProgress_->GetBackgroundStyle(STYLE_LINE_CAP), CapType::CAP_NONE);
    abstractProgress_->SetCapType(CapType::CAP_ROUND);
    EXPECT_EQ(abstractProgress_->GetForegroundStyle(STYLE_LINE_CAP), CapType::CAP_ROUND);
    EXPECT_EQ(abstractProgress_->GetBackgroundStyle(STYLE_LINE_CAP), CapType::CAP_ROUND);
}
} // namespace OHOS
