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

#include "components/ui_analog_clock.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;

namespace OHOS {
namespace {
    const uint8_t CURRENT_HOUR = 14;
    const uint8_t CURRENT_MINUTE = 40;
    const uint8_t CURRENT_SECOND = 30;
    const uint8_t INIT_HOUR = 6;
    const uint8_t INIT_MINUTE = 0;
    const uint8_t INIT_SECOND = 0;
    const uint8_t HALF_DAY_IN_HOUR = 12;
}

class UIAnalogClockTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    static UIAnalogClock* clock_;
};

UIAnalogClock* UIAnalogClockTest::clock_ = nullptr;

void UIAnalogClockTest::SetUpTestCase()
{
    if (clock_ == nullptr) {
        clock_ = new UIAnalogClock();
    }
}

void UIAnalogClockTest::TearDownTestCase()
{
    if (clock_ != nullptr) {
        delete clock_;
        clock_ = nullptr;
    }
}

/**
 * @tc.name: UIAnalogClockGetViewType_001
 * @tc.desc: Verify GetWidth and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockGetViewType_001, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(clock_->GetViewType(), UI_ANALOG_CLOCK);
}

/**
 * @tc.name: UIAnalogClockSetPosition_001
 * @tc.desc: Verify GetWidth and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQ1
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetPosition_001, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    clock_->SetPosition(0, 0);
    EXPECT_EQ(clock_->GetX(), 0);
    EXPECT_EQ(clock_->GetY(), 0);
}

/**
 * @tc.name: UIAnalogClockSetTime24Hour_001
 * @tc.desc: Verify SetTime24Hour function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetTime24Hour_001, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    clock_->SetTime24Hour(CURRENT_HOUR, CURRENT_MINUTE, CURRENT_SECOND);
    EXPECT_EQ(clock_->GetCurrentHour(), CURRENT_HOUR);
    EXPECT_EQ(clock_->GetCurrentMinute(), CURRENT_MINUTE);
    EXPECT_EQ(clock_->GetCurrentSecond(), CURRENT_SECOND);
}

/**
 * @tc.name: UIAnalogClockSetTime12Hour_001
 * @tc.desc: Verify SetTime12Hour am function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetTime12Hour_001, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    clock_->SetTime12Hour(CURRENT_HOUR, CURRENT_MINUTE, CURRENT_SECOND, true);
    EXPECT_EQ(clock_->GetCurrentHour(), CURRENT_HOUR - HALF_DAY_IN_HOUR);
    EXPECT_EQ(clock_->GetCurrentMinute(), CURRENT_MINUTE);
    EXPECT_EQ(clock_->GetCurrentSecond(), CURRENT_SECOND);
}

/**
 * @tc.name: UIAnalogClockSetTime12Hour_002
 * @tc.desc: Verify SetTime12Hour pm function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetTime12Hour_002, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    clock_->SetTime12Hour(CURRENT_HOUR, CURRENT_MINUTE, CURRENT_SECOND, false);
    EXPECT_EQ(clock_->GetCurrentHour(), CURRENT_HOUR);
    EXPECT_EQ(clock_->GetCurrentMinute(), CURRENT_MINUTE);
    EXPECT_EQ(clock_->GetCurrentSecond(), CURRENT_SECOND);
}

/**
 * @tc.name: UIAnalogClockSetHandLine_001
 * @tc.desc: Verify SetHandLine function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetHandLine_001, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    Point position = { 214, 3 }; // { 214, 3 } : the position of the line endpoint close
    Point center = { 14, 223 }; // { 14, 223 }: the rotation center of this clock hand

    clock_->SetHandLine(UIAnalogClock::HandType::SECOND_HAND,
        position, center, Color::White(), 1, 223, OPA_OPAQUE); // 223:the height of this line
    EXPECT_EQ(clock_->GetHandPosition(UIAnalogClock::HandType::SECOND_HAND).x, position.x);
    EXPECT_EQ(clock_->GetHandPosition(UIAnalogClock::HandType::SECOND_HAND).y, position.y);
    EXPECT_EQ(clock_->GetHandRotateCenter(UIAnalogClock::HandType::SECOND_HAND).x, center.x);
    EXPECT_EQ(clock_->GetHandRotateCenter(UIAnalogClock::HandType::SECOND_HAND).y, center.y);
}

/**
 * @tc.name: UIAnalogClockSetInitTime24Hour_001
 * @tc.desc: Verify SetInitTime24Hour function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetInitTime24Hour_001, TestSize.Level0)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    const uint8_t hour = 9;

    clock_->SetInitTime24Hour(INIT_HOUR, INIT_MINUTE, INIT_SECOND);
    EXPECT_EQ(clock_->GetCurrentHour(), INIT_HOUR);
    EXPECT_EQ(clock_->GetCurrentMinute(), INIT_MINUTE);
    EXPECT_EQ(clock_->GetCurrentSecond(), INIT_SECOND);
    EXPECT_EQ(clock_->GetHandInitAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);
    EXPECT_EQ(clock_->GetHandCurrentAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);

    clock_->SetTime24Hour(hour, 0, 0);
    EXPECT_EQ(clock_->GetHandInitAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);
    EXPECT_EQ(clock_->GetHandCurrentAngle(UIAnalogClock::HandType::HOUR_HAND), THREE_QUARTER_IN_DEGREE);
}

/**
 * @tc.name: UIAnalogClockSetInitTime12Hour_001
 * @tc.desc: Verify SetInitTime12Hour am function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetInitTime12Hour_001, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    clock_->SetInitTime12Hour(INIT_HOUR, INIT_MINUTE, INIT_SECOND, true);
    EXPECT_EQ(clock_->GetCurrentHour(), INIT_HOUR);
    EXPECT_EQ(clock_->GetCurrentMinute(), INIT_MINUTE);
    EXPECT_EQ(clock_->GetCurrentSecond(), INIT_SECOND);
    EXPECT_EQ(clock_->GetHandInitAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);
    EXPECT_EQ(clock_->GetHandCurrentAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);
}

/**
 * @tc.name: UIAnalogClockSetInitTime12Hour_002
 * @tc.desc: Verify SetInitTime12Hour pm function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQA
 */
HWTEST_F(UIAnalogClockTest, UIAnalogClockSetInitTime12Hour_002, TestSize.Level1)
{
    if (clock_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    clock_->SetInitTime12Hour(INIT_HOUR, INIT_MINUTE, INIT_SECOND, false);
    EXPECT_EQ(clock_->GetCurrentHour(), INIT_HOUR + HALF_DAY_IN_HOUR);
    EXPECT_EQ(clock_->GetCurrentMinute(), INIT_MINUTE);
    EXPECT_EQ(clock_->GetCurrentSecond(), INIT_SECOND);
    EXPECT_EQ(clock_->GetHandInitAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);
    EXPECT_EQ(clock_->GetHandCurrentAngle(UIAnalogClock::HandType::HOUR_HAND), SEMICIRCLE_IN_DEGREE);
}
}
