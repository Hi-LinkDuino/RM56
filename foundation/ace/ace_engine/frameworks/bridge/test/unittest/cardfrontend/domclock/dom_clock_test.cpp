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

#include "file_ex.h"
#include "gtest/gtest.h"

#include "base/json/json_util.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string CORRECT_PATH = "/data/test/resource/carddomclock/correct.json";
const std::string ERROR_PATH = "/data/test/resource/carddomclock/error.json";
constexpr int32_t DOM_CLOCK_INDEX = 2;

} // namespace

class DomCardClockTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DomCardClockTest::SetUpTestCase() {}
void DomCardClockTest::TearDownTestCase() {}
void DomCardClockTest::SetUp() {}
void DomCardClockTest::TearDown() {}

/**
 * @tc.name: DomClockDataParse001
 * @tc.desc: Verify that dom clock can be created successfully and the attr is correct.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardClockTest, DomClockDataParse001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(CORRECT_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom clock.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domClock = AceType::DynamicCast<DOMClock>(document->GetDOMNodeById(DOM_CLOCK_INDEX));
    ASSERT_TRUE(domClock != nullptr);
    ASSERT_TRUE(domClock->GetTag() == "clock");

    /**
     * @tc.steps: step3. read clock component attr.
     * @tc.expected: step3. clock component attr is correct.
     */
    auto clockComponent = AceType::DynamicCast<ClockComponent>(domClock->GetSpecializedComponent());
    ASSERT_TRUE(clockComponent != nullptr);
    auto hourswest = clockComponent->GetHoursWest();
    ASSERT_EQ(hourswest, 8);
    auto digitRadiusRatio = clockComponent->GetDigitRadiusRatio();
    ASSERT_DOUBLE_EQ(digitRadiusRatio, 0.7);
    auto digitSizeRatio = clockComponent->GetDigitSizeRatio();
    ASSERT_DOUBLE_EQ(digitSizeRatio, 0.08);
    auto face = clockComponent->GetClockFaceSrc();
    ASSERT_EQ(face, "common/clock_widget.png");
    auto hourHand = clockComponent->GetHourHandSrc();
    ASSERT_EQ(hourHand, "common/clock_widget_hour.png");
    auto minuteHand = clockComponent->GetMinuteHandSrc();
    ASSERT_EQ(minuteHand, "common/clock_widget_minute.png");
    auto secondHand = clockComponent->GetSecondHandSrc();
    ASSERT_EQ(secondHand, "common/clock_widget_second.png");
    auto faceNight = clockComponent->GetClockFaceNightSrc();
    ASSERT_EQ(faceNight, "common/black_clock_widget.png");
    auto hourHandNight = clockComponent->GetHourHandNightSrc();
    ASSERT_EQ(hourHandNight, "common/black_clock_widget_hour.png");
    auto minuteHandNight = clockComponent->GetMinuteHandNightSrc();
    ASSERT_EQ(minuteHandNight, "common/black_clock_widget_minute.png");
    auto digitColor = clockComponent->GetDigitColor();
    ASSERT_EQ(digitColor, Color::FromString("#000000"));
    auto digitColorNight = clockComponent->GetDigitColorNight();
    ASSERT_EQ(digitColorNight, Color::FromString("#FFFFFF"));
}

/**
 * @tc.name: DomClockDataParse002
 * @tc.desc: Verify that dom clock can be created successfully and the style is correct.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardClockTest, DomClockDataParse002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(CORRECT_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom clock.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domClock = AceType::DynamicCast<DOMClock>(document->GetDOMNodeById(DOM_CLOCK_INDEX));
    ASSERT_TRUE(domClock != nullptr);
    ASSERT_TRUE(domClock->GetTag() == "clock");

    /**
     * @tc.steps: step3. read clock component style.
     * @tc.expected: step3. clock component style is correct.
     */
    auto clockComponent = AceType::DynamicCast<ClockComponent>(domClock->GetSpecializedComponent());
    ASSERT_TRUE(clockComponent != nullptr);
    auto fontFamilies = clockComponent->GetFontFamilies();
    ASSERT_EQ(fontFamilies.size(), 1UL);
    ASSERT_EQ(fontFamilies[0], "Courier");
}

/**
 * @tc.name: DomClockDataParse003
 * @tc.desc: Verify that dom clock can be created successfully and the style is incorrect.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardClockTest, DomClockDataParse003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the Error card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(ERROR_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom clock.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domClock = AceType::DynamicCast<DOMClock>(document->GetDOMNodeById(DOM_CLOCK_INDEX));
    ASSERT_TRUE(domClock != nullptr);
    ASSERT_TRUE(domClock->GetTag() == "clock");

    /**
     * @tc.steps: step3. read clock component style.
     * @tc.expected: step3. clock component style is incorrect.
     */
    auto clockComponent = AceType::DynamicCast<ClockComponent>(domClock->GetSpecializedComponent());
    ASSERT_TRUE(clockComponent != nullptr);
    auto hoursWest = clockComponent->GetHoursWest();
    ASSERT_EQ(hoursWest, DBL_MAX);
}

/**
 * @tc.name: DomClockDataUpdate001
 * @tc.desc: Verify that dom clock can be created and updated successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardClockTest, DomClockDataUpdate001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(CORRECT_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom clock.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    // update config value, the data key in json file is clockconfig.
    // digitSizeRatio: 0.08 -> 1.2.
    auto params = "{\n"
                  "    \"timeRegion\": \"8\",\n"
                  "    \"fontFamily\": \"Courier\",\n"
                  "    \"clockconfig\": {\n"
                  "      \"digitRadiusRatio\": 1.2,\n"
                  "      \"digitSizeRatio\": 1.2,\n"
                  "      \"face\": \"common/clock_widget.png\",\n"
                  "      \"hourHand\": \"common/clock_widget_hour.png\",\n"
                  "      \"minuteHand\": \"common/clock_widget_minute.png\",\n"
                  "      \"secondHand\": \"common/clock_widget_second.png\",\n"
                  "      \"faceNight\": \"common/black_clock_widget.png\",\n"
                  "      \"hourHandNight\": \"common/black_clock_widget_hour.png\",\n"
                  "      \"minuteHandNight\": \"common/black_clock_widget_minute.png\",\n"
                  "      \"digitColor\": \"#000000\",\n"
                  "      \"digitColorNight\": \"#FFFFFF\"\n"
                  "    }\n"
                  "  }";
    CardTestFactory::GetInstance().ParseCardDslWithParams(page, index, params);
    auto domClock = AceType::DynamicCast<DOMClock>(document->GetDOMNodeById(DOM_CLOCK_INDEX));
    ASSERT_TRUE(domClock != nullptr);
    ASSERT_TRUE(domClock->GetTag() == "clock");

    /**
     * @tc.steps: step3. read clock component style.
     * @tc.expected: step3. clock component config is updated.
     */
    auto clockComponent = AceType::DynamicCast<ClockComponent>(domClock->GetSpecializedComponent());
    ASSERT_TRUE(clockComponent != nullptr);
    auto digitSizeRatio = clockComponent->GetDigitSizeRatio();
    ASSERT_EQ(digitSizeRatio, 1.2);
}

/**
 * @tc.name: DomClockDataUpdate002
 * @tc.desc: Verify that dom clock can be created and updated successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardClockTest, DomClockDataUpdate002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. read the correct card template file.
     * @tc.expected: step1. read file successfully.
     */
    std::string index;
    bool isSuccess = LoadStringFromFile(CORRECT_PATH, index);
    ASSERT_TRUE(isSuccess);

    /**
     * @tc.steps: step2. use factory to parse template file.
     * @tc.expected: step2. parse successfully and can get the dom clock.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    // update config value, the data key in json file is clockconfig.
    auto params = "{\n"
                  "    \"timeRegion\": \"8\",\n"
                  "    \"fontFamily\": \"Couriery\",\n"
                  "    \"clockconfig\": {\n"
                  "      \"digitRadiusRatio\": 1,\n"
                  "      \"digitSizeRatio\": 2,\n"
                  "      \"face\": \"common/clockwidget.png\",\n"
                  "      \"hourHand\": \"common/clockwidget_hour.png\",\n"
                  "      \"minuteHand\": \"common/clockwidget_minute.png\",\n"
                  "      \"secondHand\": \"common/clockwidget_second.png\",\n"
                  "      \"faceNight\": \"common/blackclock_widget.png\",\n"
                  "      \"hourHandNight\": \"common/blackclock_widget_hour.png\",\n"
                  "      \"minuteHandNight\": \"common/blackclock_widget_minute.png\",\n"
                  "      \"digitColor\": \"#000001\",\n"
                  "      \"digitColorNight\": \"#FFFFFE\"\n"
                  "    }\n"
                  "  }";
    CardTestFactory::GetInstance().ParseCardDslWithParams(page, index, params);
    auto domClock = AceType::DynamicCast<DOMClock>(document->GetDOMNodeById(DOM_CLOCK_INDEX));
    ASSERT_TRUE(domClock != nullptr);
    ASSERT_TRUE(domClock->GetTag() == "clock");

    /**
     * @tc.steps: step3. read clock component style.
     * @tc.expected: step3. clock component config is updated.
     */
    auto clockComponent = AceType::DynamicCast<ClockComponent>(domClock->GetSpecializedComponent());
    ASSERT_TRUE(clockComponent != nullptr);
    auto hourswest = clockComponent->GetHoursWest();
    ASSERT_EQ(hourswest, 8);
    auto fontFamilies = clockComponent->GetFontFamilies();
    ASSERT_EQ(fontFamilies[0], "Couriery");
    auto digitRadiusRatio = clockComponent->GetDigitRadiusRatio();
    ASSERT_DOUBLE_EQ(digitRadiusRatio, 1);
    auto digitSizeRatio = clockComponent->GetDigitSizeRatio();
    ASSERT_DOUBLE_EQ(digitSizeRatio, 2);
    auto face = clockComponent->GetClockFaceSrc();
    ASSERT_EQ(face, "common/clockwidget.png");
    auto hourHand = clockComponent->GetHourHandSrc();
    ASSERT_EQ(hourHand, "common/clockwidget_hour.png");
    auto minuteHand = clockComponent->GetMinuteHandSrc();
    ASSERT_EQ(minuteHand, "common/clockwidget_minute.png");
    auto secondHand = clockComponent->GetSecondHandSrc();
    ASSERT_EQ(secondHand, "common/clockwidget_second.png");
    auto faceNight = clockComponent->GetClockFaceNightSrc();
    ASSERT_EQ(faceNight, "common/blackclock_widget.png");
    auto hourHandNight = clockComponent->GetHourHandNightSrc();
    ASSERT_EQ(hourHandNight, "common/blackclock_widget_hour.png");
    auto minuteHandNight = clockComponent->GetMinuteHandNightSrc();
    ASSERT_EQ(minuteHandNight, "common/blackclock_widget_minute.png");
    auto digitColor = clockComponent->GetDigitColor();
    ASSERT_EQ(digitColor, Color::FromString("#000001"));
    auto digitColorNight = clockComponent->GetDigitColorNight();
    ASSERT_EQ(digitColorNight, Color::FromString("#FFFFFE"));
}

} // namespace OHOS::Ace::Framework
