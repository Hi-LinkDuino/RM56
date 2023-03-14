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

#include "components/ui_digital_clock.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const uint8_t CURRENT_HOUR = 14;
    const uint8_t CURRENT_MINUTE = 40;
    const uint8_t CURRENT_SECOND = 30;
    const uint8_t HALF_DAY_IN_HOUR = 12;
}

class UIDigitalClockTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIDigitalClock* digitalClock_;
};

UIDigitalClock* UIDigitalClockTest::digitalClock_ = nullptr;

void UIDigitalClockTest::SetUpTestCase(void)
{
    if (digitalClock_ == nullptr) {
        digitalClock_ = new UIDigitalClock();
    }
}

void UIDigitalClockTest::TearDownTestCase(void)
{
    if (digitalClock_ != nullptr) {
        delete digitalClock_;
        digitalClock_ = nullptr;
    }
}

/**
 * @tc.name: UIDigitalClockGetViewType_001
 * @tc.desc: Verify GetViewType function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockGetViewType_001, TestSize.Level1)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(digitalClock_->GetViewType(), UI_DIGITAL_CLOCK);
}

/**
 * @tc.name: UIDigitalClockSetFontId_001
 * @tc.desc: Verify SetFontId function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockSetFontId_001, TestSize.Level1)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint8_t fontId = 10;
    digitalClock_->SetFontId(fontId);
    EXPECT_EQ(digitalClock_->GetStyle(STYLE_TEXT_FONT), fontId);
}

/**
 * @tc.name: UIDigitalClockSetDisplayMode_001
 * @tc.desc: Verify SetDisplayMode function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockSetDisplayMode_001, TestSize.Level0)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    digitalClock_->SetDisplayMode(UIDigitalClock::DisplayMode::DISPLAY_12_HOUR_NO_SECONDS);
    EXPECT_EQ(digitalClock_->GetDisplayMode(), UIDigitalClock::DisplayMode::DISPLAY_12_HOUR_NO_SECONDS);
    digitalClock_->SetDisplayMode(UIDigitalClock::DisplayMode::DISPLAY_24_HOUR_NO_SECONDS);
    EXPECT_EQ(digitalClock_->GetDisplayMode(), UIDigitalClock::DisplayMode::DISPLAY_24_HOUR_NO_SECONDS);
    digitalClock_->SetDisplayMode(UIDigitalClock::DisplayMode::DISPLAY_12_HOUR);
    EXPECT_EQ(digitalClock_->GetDisplayMode(), UIDigitalClock::DisplayMode::DISPLAY_12_HOUR);
    digitalClock_->SetDisplayMode(UIDigitalClock::DisplayMode::DISPLAY_24_HOUR);
    EXPECT_EQ(digitalClock_->GetDisplayMode(), UIDigitalClock::DisplayMode::DISPLAY_24_HOUR);
}

/**
 * @tc.name: UIDigitalClockSetOpacity_001
 * @tc.desc: Verify SetOpacity function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockSetOpacity_001, TestSize.Level1)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    digitalClock_->SetOpacity(OPA_OPAQUE);
    EXPECT_EQ(digitalClock_->GetOpacity(), OPA_OPAQUE);
}

/**
 * @tc.name: UIDigitalClockSetTime24Hour_001
 * @tc.desc: Verify SetTime24Hour function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockSetTime24Hour_001, TestSize.Level1)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    digitalClock_->SetTime24Hour(CURRENT_HOUR, CURRENT_MINUTE, CURRENT_SECOND);
    EXPECT_EQ(digitalClock_->GetCurrentHour(), CURRENT_HOUR);
    EXPECT_EQ(digitalClock_->GetCurrentMinute(), CURRENT_MINUTE);
    EXPECT_EQ(digitalClock_->GetCurrentSecond(), CURRENT_SECOND);
}

/**
 * @tc.name: UIDigitalClockSetTime12Hour_001
 * @tc.desc: Verify SetTime12Hour function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockSetTime12Hour_001, TestSize.Level1)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    digitalClock_->SetTime12Hour(CURRENT_HOUR, CURRENT_MINUTE, CURRENT_SECOND, true);
    EXPECT_EQ(digitalClock_->GetCurrentHour(), CURRENT_HOUR - HALF_DAY_IN_HOUR);
    EXPECT_EQ(digitalClock_->GetCurrentMinute(), CURRENT_MINUTE);
    EXPECT_EQ(digitalClock_->GetCurrentSecond(), CURRENT_SECOND);
}

/**
 * @tc.name: UIDigitalClockSetTime12Hour_002
 * @tc.desc: Verify SetTime12Hour function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ4
 */
HWTEST_F(UIDigitalClockTest, UIDigitalClockSetTime12Hour_002, TestSize.Level1)
{
    if (digitalClock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    digitalClock_->SetTime12Hour(CURRENT_HOUR, CURRENT_MINUTE, CURRENT_SECOND, false);
    EXPECT_EQ(digitalClock_->GetCurrentHour(), CURRENT_HOUR);
    EXPECT_EQ(digitalClock_->GetCurrentMinute(), CURRENT_MINUTE);
    EXPECT_EQ(digitalClock_->GetCurrentSecond(), CURRENT_SECOND);
}
}
