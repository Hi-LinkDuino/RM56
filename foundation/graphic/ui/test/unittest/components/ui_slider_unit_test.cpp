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

#include "components/ui_slider.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UISliderTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UISlider* slider_;
};

UISlider* UISliderTest::slider_ = nullptr;

void UISliderTest::SetUpTestCase(void)
{
    if (slider_ == nullptr) {
        slider_ = new UISlider();
    }
}

void UISliderTest::TearDownTestCase(void)
{
    if (slider_ != nullptr) {
        delete slider_;
        slider_ = nullptr;
    }
}

/**
 * @tc.name:UISliderGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(UISliderTest, UISliderGetViewType_001, TestSize.Level1)
{
    if (slider_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(slider_->GetViewType(), UI_SLIDER);
}

/**
 * @tc.name:UISliderSetKnobWidth_001
 * @tc.desc: Verify SetKnobWidth function, equal.
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(UISliderTest, UISliderSetKnobWidth_001, TestSize.Level0)
{
    if (slider_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const int16_t width = 10;

    slider_->SetKnobWidth(width);
    EXPECT_EQ(slider_->GetKnobWidth(), width);
}

/**
 * @tc.name:UISliderSetKnobStyle_001
 * @tc.desc: Verify SetKnobStyle function, equal.
 * @tc.type: FUNC
 * @tc.require: NA
 */
HWTEST_F(UISliderTest, UISliderSetKnobStyle_001, TestSize.Level0)
{
    if (slider_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    slider_->SetKnobStyle(STYLE_BACKGROUND_COLOR, Color::Gray().full);
    EXPECT_EQ(slider_->GetKnobStyle().bgColor_.full, Color::Gray().full);
}
} // namespace OHOS
