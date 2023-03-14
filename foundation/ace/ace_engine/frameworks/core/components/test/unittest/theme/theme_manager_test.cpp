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

#include <string>
#include <unordered_map>

#include "gtest/gtest.h"

#include "base/log/log.h"
#include "core/components/button/button_theme.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components/focus_animation/focus_animation_theme.h"
#include "core/components/marquee/marquee_theme.h"
#include "core/components/popup/popup_theme.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/rating/rating_theme.h"
#include "core/components/select/select_theme.h"
#include "core/components/slider/slider_theme.h"
#include "core/components/stepper/stepper_theme.h"
#include "core/components/swiper/swiper_indicator_theme.h"
#include "core/components/tab_bar/tab_theme.h"
#include "core/components/text/text_theme.h"
#include "core/components/text_field/textfield_theme.h"
#include "core/components/theme/app_theme.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/toast/toast_theme.h"
#include "core/components/video/video_theme.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::unordered_map<std::string, ThemeType> COMPONENTS_THEMES = {
    { "ButtonTheme", ButtonTheme::TypeId() },
    { "CheckboxTheme", CheckboxTheme::TypeId() },
    { "SwitchTheme", SwitchTheme::TypeId() },
    { "RadioTheme", RadioTheme::TypeId() },
    { "DialogTheme", DialogTheme::TypeId() },
    { "MarqueeTheme", MarqueeTheme::TypeId() },
    { "PopupTheme", PopupTheme::TypeId() },
    { "ProgressTheme", ProgressTheme::TypeId() },
    { "SelectTheme", SelectTheme::TypeId() },
    { "SliderTheme", SliderTheme::TypeId() },
    { "StepperTheme", StepperTheme::TypeId() },
    { "SwiperIndicatorTheme", SwiperIndicatorTheme::TypeId() },
    { "TabTheme", TabTheme::TypeId() },
    { "TextTheme", TextTheme::TypeId() },
    { "TextFieldTheme", TextFieldTheme::TypeId() },
    { "ToastTheme", ToastTheme::TypeId() },
    { "VideoTheme", VideoTheme::TypeId() },
    { "RatingTheme", RatingTheme::TypeId() },
    { "FocusAnimationTheme", FocusAnimationTheme::TypeId() },
};

} // namespace

class ThemeManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ThemeManagerTest::SetUpTestCase() {}
void ThemeManagerTest::TearDownTestCase() {}
void ThemeManagerTest::SetUp() {}
void ThemeManagerTest::TearDown() {}

/**
 * @tc.name: GlobalTheme001
 * @tc.desc: Test global theme exists.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeManagerTest, GlobalTheme001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get global AppTheme from ThemeManager, check whether is nullptr.
     * @tc.expected: step1. Global AppTheme is not nullptr.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<AppTheme> theme = themeManager->GetTheme<AppTheme>();
    EXPECT_TRUE(theme);
}

/**
 * @tc.name: GlobalTheme002
 * @tc.desc: Test global theme contains all components' theme.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeManagerTest, GlobalTheme002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get all components' theme from ThemeManager, check each whether is nullptr.
     * @tc.expected: step1. Each components' theme is not nullptr, and typeId is correct.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    for (const auto& [name, themeType] : COMPONENTS_THEMES) {
        auto theme = themeManager->GetTheme(themeType);
        EXPECT_TRUE(theme) << "Theme not found: " << name;
        EXPECT_EQ(themeType, AceType::TypeId(theme));
    }
}

} // namespace OHOS::Ace