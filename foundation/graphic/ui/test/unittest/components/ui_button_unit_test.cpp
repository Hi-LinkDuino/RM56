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

#include "components/ui_button.h"

#include <climits>
#include <gtest/gtest.h>
#include "test_resource_config.h"

using namespace testing::ext;

namespace OHOS {
namespace {
    const Point INIT_POS = { 10, 12 };
}
class UIButtonTest : public testing::Test {
public:
    UIButtonTest() : button_(nullptr) {}
    virtual ~UIButtonTest() {}
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void TearDown();
    void SetUp();
    UIButton* button_;
};

void UIButtonTest::SetUp()
{
    if (button_ == nullptr) {
        button_ = new UIButton();
    }
}

void UIButtonTest::TearDown()
{
    if (button_ != nullptr) {
        delete button_;
        button_ = nullptr;
    }
}

/**
 * @tc.name: UIButtonGetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonGetViewType_001, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(button_->GetViewType(), UI_BUTTON);
}

/**
 * @tc.name: UIButtonSetImageSrc_001
 * @tc.desc: Verify SetImageSrc function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetImageSrc_001, TestSize.Level0)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetImageSrc(BLUE_RGB888_IMAGE_PATH, BLUE_RGB565_IMAGE_PATH);
    ASSERT_TRUE(button_->GetCurImageSrc());

    if (button_->GetCurImageSrc()->GetPath() == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(strcmp(button_->GetCurImageSrc()->GetPath(), BLUE_RGB888_IMAGE_PATH), 0);
    PressEvent event(INIT_POS);
    button_->OnPressEvent(event);
    if (button_->GetCurImageSrc()->GetPath() == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(strcmp(button_->GetCurImageSrc()->GetPath(), BLUE_RGB565_IMAGE_PATH), 0);
}

/**
 * @tc.name:UIButtonSetImagePosition_001
 * @tc.desc: Verify SetImagePosition function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetImagePosition_001, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetImagePosition(INIT_POS.x, INIT_POS.y);
    EXPECT_EQ(button_->GetImageX(), INIT_POS.x);
    EXPECT_EQ(button_->GetImageY(), INIT_POS.y);
}

/**
 * @tc.name: UIButtonSetStyle_001
 * @tc.desc: Verify SetStyle function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetStyle_001, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetStateForStyle(UIButton::ButtonState::PRESSED);
    button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    EXPECT_EQ(button_->GetStyle(STYLE_BACKGROUND_COLOR), Color::Blue().full);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::PRESSED), Color::Blue().full);
}

/**
 * @tc.name: UIButtonSetStyle_002
 * @tc.desc: Verify SetStyle function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetStyle_002, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetStateForStyle(UIButton::ButtonState::INACTIVE);
    button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    EXPECT_EQ(button_->GetStyle(STYLE_BACKGROUND_COLOR), Color::Red().full);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::INACTIVE), Color::Red().full);
}

/**
 * @tc.name: UIButtonSetStyle_003
 * @tc.desc: Verify SetStyle function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetStyle_003, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetStateForStyle(UIButton::ButtonState::RELEASED);
    button_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Green().full);
    EXPECT_EQ(button_->GetStyle(STYLE_BACKGROUND_COLOR), Color::Green().full);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::RELEASED), Color::Green().full);
}

/**
 * @tc.name: UIButtonIsTouchable_001
 * @tc.desc: Verify IsTouchable function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonIsTouchable_001, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->Enable();
    EXPECT_EQ(button_->IsTouchable(), true);
    button_->Disable();
    EXPECT_EQ(button_->IsTouchable(), false);
}

/**
 * @tc.name: UIButtonSetSize_001
 * @tc.desc: Verify SetSize function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetSize_001, TestSize.Level0)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t releasedWidth = 5;
    const int16_t releasedHeight = 25;
    const int16_t paddingdLeft = 10;
    const int16_t paddingdTop = 20;
    const int16_t borderWidth = 2;
    const int16_t posX = 50;
    const int16_t posY = 100;
    button_->SetStyleForState(STYLE_PADDING_LEFT, paddingdLeft, UIButton::ButtonState::RELEASED);
    button_->SetStyleForState(STYLE_BORDER_WIDTH, borderWidth, UIButton::ButtonState::RELEASED);
    button_->SetStyleForState(STYLE_PADDING_TOP, paddingdTop, UIButton::ButtonState::RELEASED);

    ReleaseEvent releaseEvent(INIT_POS);
    button_->OnReleaseEvent(releaseEvent);
button_->SetPosition(posX, posY);
    button_->SetWidth(releasedWidth);
    button_->SetHeight(releasedHeight);
    EXPECT_EQ(button_->GetWidth(), releasedWidth);
    EXPECT_EQ(button_->GetHeight(), releasedHeight);
    EXPECT_EQ(button_->GetContentRect().GetWidth(), releasedWidth);
    EXPECT_EQ(button_->GetContentRect().GetHeight(), releasedHeight);
    EXPECT_EQ(button_->GetContentRect().GetX(), posX + paddingdLeft + borderWidth);
    EXPECT_EQ(button_->GetContentRect().GetY(), posY + paddingdTop + borderWidth);
}

/**
 * @tc.name: UIButtonSetSize_002
 * @tc.desc: Verify SetSize function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetSize_002, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t pressWidth = 5;
    const int16_t pressHeight = 25;
    const int16_t paddingdLeft = 10;
    const int16_t paddingdTop = 20;
    const int16_t borderWidth = 2;
    const int16_t posX = 50;
    const int16_t posY = 100;
    button_->SetStyleForState(STYLE_PADDING_LEFT, paddingdLeft, UIButton::ButtonState::PRESSED);
    button_->SetStyleForState(STYLE_BORDER_WIDTH, borderWidth, UIButton::ButtonState::PRESSED);
    button_->SetStyleForState(STYLE_PADDING_TOP, paddingdTop, UIButton::ButtonState::PRESSED);

    PressEvent pressEvent(INIT_POS);
    button_->OnPressEvent(pressEvent);
    button_->SetPosition(posX, posY);
    button_->SetWidth(pressWidth);
    button_->SetHeight(pressHeight);
    EXPECT_EQ(button_->GetWidth(), pressWidth);
    EXPECT_EQ(button_->GetHeight(), pressHeight);
    EXPECT_EQ(button_->GetContentRect().GetWidth(), pressWidth);
    EXPECT_EQ(button_->GetContentRect().GetHeight(), pressHeight);
    EXPECT_EQ(button_->GetContentRect().GetX(), posX + paddingdLeft + borderWidth);
    EXPECT_EQ(button_->GetContentRect().GetY(), posY + paddingdTop + borderWidth);
}

/**
 * @tc.name: UIButtonSetSize_003
 * @tc.desc: Verify SetSize function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetSize_003, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    const int16_t inactiveWidth = 5;
    const int16_t inactiveHeight = 25;
    const int16_t paddingdLeft = 10;
    const int16_t paddingdTop = 20;
    const int16_t borderWidth = 2;
    const int16_t posX = 50;
    const int16_t posY = 100;
    button_->SetStyleForState(STYLE_PADDING_LEFT, paddingdLeft, UIButton::ButtonState::INACTIVE);
    button_->SetStyleForState(STYLE_BORDER_WIDTH, borderWidth, UIButton::ButtonState::INACTIVE);
    button_->SetStyleForState(STYLE_PADDING_TOP, paddingdTop, UIButton::ButtonState::INACTIVE);

    button_->Disable();
    button_->SetPosition(posX, posY);
    button_->SetWidth(inactiveWidth);
    button_->SetHeight(inactiveHeight);
    EXPECT_EQ(button_->GetWidth(), inactiveWidth);
    EXPECT_EQ(button_->GetHeight(), inactiveHeight);
    EXPECT_EQ(button_->GetContentRect().GetWidth(), inactiveWidth);
    EXPECT_EQ(button_->GetContentRect().GetHeight(), inactiveHeight);
    EXPECT_EQ(button_->GetContentRect().GetX(), posX + paddingdLeft + borderWidth);
    EXPECT_EQ(button_->GetContentRect().GetY(), posY + paddingdTop + borderWidth);
}

/**
 * @tc.name: UIButtonSetStyleForState_001
 * @tc.desc: Verify SetStyleForState function, equal.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetStyleForState_001, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Red().full, UIButton::ButtonState::RELEASED);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::RELEASED),
        Color::Red().full);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Green().full, UIButton::ButtonState::PRESSED);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::PRESSED),
        Color::Green().full);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Yellow().full, UIButton::ButtonState::INACTIVE);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::INACTIVE),
        Color::Yellow().full);
}

/**
 * @tc.name: UIButtonSetStateForStyle_001
 * @tc.desc: Verify SetStyle function.
 * @tc.type: FUNC
 * @tc.require: SR000DRSH1
 */
HWTEST_F(UIButtonTest, UIButtonSetStateForStyle_001, TestSize.Level1)
{
    if (button_ == nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Red().full, UIButton::ButtonState::RELEASED);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::RELEASED),
        Color::Red().full);
    button_->SetStyleForState(STYLE_BACKGROUND_COLOR, Color::Green().full, UIButton::ButtonState::PRESSED);
    EXPECT_EQ(button_->GetStyleForState(STYLE_BACKGROUND_COLOR, UIButton::ButtonState::PRESSED),
        Color::Green().full);
    button_->SetStateForStyle(UIButton::ButtonState::RELEASED);
    EXPECT_EQ(button_->GetStyle(STYLE_BACKGROUND_COLOR), Color::Red().full);
    button_->SetStateForStyle(UIButton::ButtonState::PRESSED);
    EXPECT_EQ(button_->GetStyle(STYLE_BACKGROUND_COLOR), Color::Green().full);
}
} // namespace OHOS
