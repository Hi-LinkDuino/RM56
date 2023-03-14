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

#include "components/ui_arc_label.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
class UIArcLabelTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIArcLabel* arcLabel_;
};
UIArcLabel* UIArcLabelTest::arcLabel_ = nullptr;

void UIArcLabelTest::SetUpTestCase(void)
{
    if (arcLabel_ == nullptr) {
        arcLabel_ = new UIArcLabel();
    }
}

void UIArcLabelTest::TearDownTestCase(void)
{
    if (arcLabel_ != nullptr) {
        delete arcLabel_;
        arcLabel_ = nullptr;
    }
}
/**
 * @tc.name: UIArcLabelGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelGetViewType_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(arcLabel_->GetViewType(), UI_ARC_LABEL);
}

/**
 * @tc.name: UIArcLabelGetWidth_001
 * @tc.desc: Verify GetWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelGetWidth_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t width = 100;
    arcLabel_->SetWidth(width);
    EXPECT_EQ(arcLabel_->GetWidth(), width);
}

/**
 * @tc.name: UIArcLabelGetHeight_001
 * @tc.desc: Verify GetHeight function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelGetHeight_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t height = 100;
    arcLabel_->SetHeight(height);
    EXPECT_EQ(arcLabel_->GetHeight(), height);
}

/**
 * @tc.name: UIArcLabelSetText_001
 * @tc.desc: Verify SetText function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelSetText_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const char* text = "abc";
    arcLabel_->SetText(text);
    EXPECT_EQ(strcmp(arcLabel_->GetText(), text), 0);
}

/**
 * @tc.name: UIArcLabelSetAlign_001
 * @tc.desc: Verify SetAlign function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelSetAlign_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UITextLanguageAlignment horizontalAlign = TEXT_ALIGNMENT_LEFT;
    arcLabel_->SetAlign(horizontalAlign);
    EXPECT_EQ(arcLabel_->GetHorAlign(), horizontalAlign);
}

/**
 * @tc.name: UIArcLabelSetArcTextCenter_001
 * @tc.desc: Verify SetArcTextCenter function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelSetArcTextCenter_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t posX = 10;
    const int16_t posY = 20;
    arcLabel_->SetArcTextCenter(posX, posY);
    EXPECT_EQ(arcLabel_->GetArcTextCenter().x, posX);
    EXPECT_EQ(arcLabel_->GetArcTextCenter().y, posY);
}

/**
 * @tc.name: UIArcLabelSetArcTextRadius_001
 * @tc.desc: Verify SetArcTextRadius function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelSetArcTextRadius_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t radius = 1;
    arcLabel_->SetArcTextRadius(radius);
    EXPECT_EQ(arcLabel_->GetArcTextRadius(), radius);
}

/**
 * @tc.name: UIArcLabelSetArcTextAngle_001
 * @tc.desc: Verify SetArcTextAngle function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelSetArcTextAngle_001, TestSize.Level0)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t endAngle = 10;
    arcLabel_->SetArcTextAngle(0, endAngle);
    EXPECT_EQ(arcLabel_->GetArcTextStartAngle(), 0);
    EXPECT_EQ(arcLabel_->GetArcTextEndAngle(), endAngle);
}

/**
 * @tc.name: UIArcLabelSetArcTextOrientation_001
 * @tc.desc: Verify SetArcTextOrientation function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelSetArcTextOrientation_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    arcLabel_->SetArcTextOrientation(UIArcLabel::TextOrientation::INSIDE);
    EXPECT_EQ(arcLabel_->GetArcTextOrientation(),
        UIArcLabel::TextOrientation::INSIDE);
    arcLabel_->SetArcTextOrientation(UIArcLabel::TextOrientation::OUTSIDE);
    EXPECT_EQ(arcLabel_->GetArcTextOrientation(),
        UIArcLabel::TextOrientation::OUTSIDE);
}

/**
 * @tc.name: UIArcLabelGetDirect_001
 * @tc.desc: Verify GetDirect function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMPM
 */
HWTEST_F(UIArcLabelTest, UIArcLabelGetDirect_001, TestSize.Level1)
{
    if (arcLabel_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(arcLabel_->GetDirect(), UITextLanguageDirect(0));
}
}
