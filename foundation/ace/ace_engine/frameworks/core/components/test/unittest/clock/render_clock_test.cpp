/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"

#define protected public
#include "base/i18n/localization.h"
#include "base/utils/system_properties.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "frameworks/core/components/clock/clock_component.h"
#include "frameworks/core/components/clock/render_clock.h"
#include "frameworks/core/components/test/json/json_frontend.h"
#include "frameworks/core/components/test/unittest/mock/mock_render_depend.h"
#include "frameworks/core/components/text/render_text.h"
#include "frameworks/core/event/key_event.h"
#include "frameworks/core/mock/mock_resource_register.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr char CLOCK_FACE_SRC[] = "clockFaceSrc";
constexpr char CLOCK_FACE_NIGHT_SRC[] = "clockFaceNightSrc";
constexpr char HOUR_HAND_SRC[] = "hourHandSrc";
constexpr char HOUR_HAND_NIGHT_SRC[] = "hourHandNightSrc";
constexpr char MINUTE_HAND_SRC[] = "minuteHandSrc";
constexpr char MINUTE_HAND_NIGHT_SRC[] = "minuteHandNightSrc";
constexpr char SECOND_HAND_SRC[] = "secondHandSrc";
constexpr char SECOND_HAND_NIGHT_SRC[] = "secondHandNightSrc";
constexpr char FONT[] = "sans-serif";
constexpr double VALID_HOUR_WEST = -7;
constexpr double VALID_DIGIT_SIZE_RATIO = 0.1;
constexpr double VALID_DIGIT_RADIUS_RATIO = 0.4;
constexpr double INVALID_DIGIT_SIZE_RATIO = 0.2;
constexpr double INVALID_DIGIT_RADIUS_RATIO = 1.6;
constexpr double DEFAULT_HOUR_WEST = DBL_MAX;
constexpr double DEFAULT_DIGIT_SIZE_RATIO = 0.08;
constexpr double DEFAULT_DIGIT_RADIUS_RATIO = 0.7;
constexpr double TEST_TEXT_SIZE = 14;
constexpr int32_t FONT_FAMILY_SIZE = 1;
const std::string DIGIT_COLOR_RED = "#ffff0000";
const std::string DIGIT_COLOR_BLUE = "#ff0000ff";

using UpdateClockCallback = std::function<void(const RefPtr<ClockComponent>&)>;

} // namespace

class MockImageCache : public ImageCache {
    void Clear() override {};
};

RefPtr<ImageCache> ImageCache::Create()
{
    return AceType::MakeRefPtr<MockImageCache>();
}

class ClockComponentTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        Localization::GetInstance()->SetLocale("zh", "CN", "", "", "");
    }

    static void TearDownTestCase() {}

    void SetUp() {}
    void TearDown() {}

protected:
    RefPtr<ClockComponent> CreateAndUpdateClockComponent(UpdateClockCallback callback) const;
};

RefPtr<ClockComponent> ClockComponentTest::CreateAndUpdateClockComponent(UpdateClockCallback updateClockComponent) const
{
    RefPtr<ClockComponent> clockComponent = AceType::MakeRefPtr<ClockComponent>();
    EXPECT_TRUE(clockComponent != nullptr);

    if (updateClockComponent) {
        updateClockComponent(clockComponent);
    }
    return clockComponent;
}

/**
 * @tc.name: UpdateClock001
 * @tc.desc: Test when clock component receives valid settings, it updates all settings correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ClockComponentTest, UpdateClock001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Clock component and update it.
     */
    auto renderClock = AceType::MakeRefPtr<RenderClock>();
    auto clockComponent = CreateAndUpdateClockComponent([](const RefPtr<ClockComponent>& clockComponent) {
        const auto& declaration = clockComponent->GetDeclaration();
        EXPECT_TRUE(declaration != nullptr);
        declaration->SetClockFaceSrc(CLOCK_FACE_SRC);
        declaration->SetClockFaceNightSrc(CLOCK_FACE_NIGHT_SRC);
        declaration->SetHourHandSrc(HOUR_HAND_SRC);
        declaration->SetHourHandNightSrc(HOUR_HAND_NIGHT_SRC);
        declaration->SetMinuteHandSrc(MINUTE_HAND_SRC);
        declaration->SetMinuteHandNightSrc(MINUTE_HAND_NIGHT_SRC);
        declaration->SetSecondHandSrc(SECOND_HAND_SRC);
        declaration->SetSecondHandNightSrc(SECOND_HAND_NIGHT_SRC);
        declaration->SetHoursWest(VALID_HOUR_WEST);
        declaration->SetDigitColor(DIGIT_COLOR_RED);
        declaration->SetDigitColorNight(DIGIT_COLOR_BLUE);
        declaration->SetDigitSizeRatio(VALID_DIGIT_SIZE_RATIO);
        declaration->SetDigitRadiusRatio(VALID_DIGIT_RADIUS_RATIO);
        declaration->SetFontFamilies(std::vector<std::string>({ FONT }));
    });

    /**
     * @tc.steps: step2. Update RenderClock with ClockComponent.
     * @tc.expected: step2. RenderClock is updated with correct settings.
     */
    renderClock->Update(clockComponent);
    EXPECT_EQ(renderClock->clockFaceNightSrc_, CLOCK_FACE_NIGHT_SRC);
    EXPECT_EQ(renderClock->hourHandNightSrc_, HOUR_HAND_NIGHT_SRC);
    EXPECT_EQ(renderClock->minuteHandNightSrc_, MINUTE_HAND_NIGHT_SRC);
    EXPECT_EQ(renderClock->secondHandNightSrc_, SECOND_HAND_NIGHT_SRC);
    EXPECT_EQ(renderClock->digitColorNight_, Declaration().ParseColor(DIGIT_COLOR_BLUE));
    ASSERT_TRUE(NearEqual(renderClock->digitSizeRatio_, VALID_DIGIT_SIZE_RATIO));
    ASSERT_TRUE(NearEqual(renderClock->digitRadiusRatio_, VALID_DIGIT_RADIUS_RATIO));
    ASSERT_TRUE(NearEqual(renderClock->renderClockHand_->hoursWest_, VALID_HOUR_WEST));

    renderClock->UpdateRenderText(TEST_TEXT_SIZE, Color::BLACK);
    for (const auto& renderText : renderClock->digitRenderNodes_) {
        auto fontFamilies = renderText->textStyle_.GetFontFamilies();
        ASSERT_TRUE(NearEqual(fontFamilies.size(), FONT_FAMILY_SIZE));
        EXPECT_EQ(fontFamilies.front(), FONT);
    }
}

/**
 * @tc.name: UpdateClock002
 * @tc.desc: Test when clock component receives incomplete settings, it updates all settings as expected.
 * @tc.type: FUNC
 */
HWTEST_F(ClockComponentTest, UpdateClock002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Clock component and update it.
     */
    auto renderClock = AceType::MakeRefPtr<RenderClock>();
    auto clockComponent = CreateAndUpdateClockComponent([](const RefPtr<ClockComponent>& clockComponent) {
        const auto& declaration = clockComponent->GetDeclaration();
        EXPECT_TRUE(declaration != nullptr);
        declaration->SetClockFaceSrc(CLOCK_FACE_SRC);
        declaration->SetHourHandSrc(HOUR_HAND_SRC);
        declaration->SetMinuteHandSrc(MINUTE_HAND_SRC);
        declaration->SetSecondHandSrc(SECOND_HAND_SRC);
    });

    /**
     * @tc.steps: step2. Update RenderClock with ClockComponent.
     * @tc.expected: step2. RenderClock is updated with correct settings.
     */
    renderClock->Update(clockComponent);
    EXPECT_EQ(renderClock->clockFaceNightSrc_, CLOCK_FACE_SRC);
    EXPECT_EQ(renderClock->hourHandNightSrc_, HOUR_HAND_SRC);
    EXPECT_EQ(renderClock->minuteHandNightSrc_, MINUTE_HAND_SRC);
    EXPECT_EQ(renderClock->secondHandNightSrc_, SECOND_HAND_SRC);
    EXPECT_EQ(renderClock->digitColorNight_, Color::BLACK);
    ASSERT_TRUE(NearEqual(renderClock->digitSizeRatio_, DEFAULT_DIGIT_SIZE_RATIO));
    ASSERT_TRUE(NearEqual(renderClock->digitRadiusRatio_, DEFAULT_DIGIT_RADIUS_RATIO));
    ASSERT_TRUE(NearEqual(renderClock->renderClockHand_->hoursWest_, DEFAULT_HOUR_WEST));

    renderClock->UpdateRenderText(TEST_TEXT_SIZE, Color::BLACK);
    for (const auto& renderText : renderClock->digitRenderNodes_) {
        EXPECT_TRUE(renderText->textStyle_.GetFontFamilies().empty());
    }
}

/**
 * @tc.name: UpdateClock003
 * @tc.desc: Test when clock component receives invalid settings, it updates all settings correctly.
 * @tc.type: FUNC
 */
HWTEST_F(ClockComponentTest, UpdateClock003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Build a Clock component and update it.
     */
    auto renderClock = AceType::MakeRefPtr<RenderClock>();
    auto clockComponent = CreateAndUpdateClockComponent([](const RefPtr<ClockComponent>& clockComponent) {
        const auto& declaration = clockComponent->GetDeclaration();
        EXPECT_TRUE(declaration != nullptr);
        declaration->SetDigitSizeRatio(INVALID_DIGIT_SIZE_RATIO);
        declaration->SetDigitRadiusRatio(INVALID_DIGIT_RADIUS_RATIO);
    });

    /**
     * @tc.steps: step2. Update RenderClock with ClockComponent.
     * @tc.expected: step2. RenderClock is updated with correct settings.
     */
    renderClock->Update(clockComponent);
    ASSERT_TRUE(NearEqual(renderClock->digitSizeRatio_, DEFAULT_DIGIT_SIZE_RATIO));
    ASSERT_TRUE(NearEqual(renderClock->digitRadiusRatio_, DEFAULT_DIGIT_RADIUS_RATIO));
}

} // namespace OHOS::Ace