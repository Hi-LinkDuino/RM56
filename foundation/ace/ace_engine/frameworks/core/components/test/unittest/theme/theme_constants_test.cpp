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

#include <unordered_set>

#include "gtest/gtest.h"

#define private public
#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "core/components/button/button_theme.h"
#include "core/components/checkable/checkable_theme.h"
#include "core/components/dialog/dialog_theme.h"
#include "core/components/marquee/marquee_theme.h"
#include "core/components/popup/popup_theme.h"
#include "core/components/progress/progress_theme.h"
#include "core/components/select/select_theme.h"
#include "core/components/slider/slider_theme.h"
#include "core/components/stepper/stepper_theme.h"
#include "core/components/swiper/swiper_indicator_theme.h"
#include "core/components/test/unittest/theme/theme_mock.h"
#include "core/components/text/text_theme.h"
#include "core/components/theme/app_theme.h"
#include "core/components/theme/theme_attributes.h"
#include "core/components/theme/theme_constants.h"
#include "core/components/theme/theme_constants_defines.h"
#include "core/components/theme/theme_manager.h"
#include "core/components/toast/toast_theme.h"
#include "core/components/video/video_theme.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

RefPtr<ThemeConstants> g_themeConstants = nullptr;
// Same with value defined at ThemeConstants
const Color COLOR_CONTROL_ACTIVATED_PHONE = Color(0xff0a59f7);
const Color COLOR_ERROR = Color(0xff000000);
const Dimension BUTTON_TEXT_FONTSIZE_PHONE = Dimension(16.0, DimensionUnit::FP);
const Dimension DIMENSION_ERROR = Dimension(0.0, DimensionUnit::VP);
const Color CUSTOM_COLOR_FG = Color(0xffffff00);
constexpr double CUSTOM_ALPHA = 0.5;
constexpr double CUSTOM_LINE_HEIGHT = 1.5;
constexpr int32_t CUSTOM_ANIM_DURATION = 500;
const Dimension CUSTOM_FONT_SIZE = Dimension(10.0, DimensionUnit::PX);
const Dimension CUSTOM_MARGIN = Dimension(10.0, DimensionUnit::PX);
constexpr uint32_t STYLE_ID_INVALID = 9999;
const Dimension PARSE_DIMENSION_ERROR = Dimension(0.0, DimensionUnit::PX);
// test value same with ResourceAdapterMock
constexpr uint32_t THEME_ID_VALID = 0;
constexpr uint32_t THEME_ID_INVALID = 1;
const std::string TEXT_SIZE_VALUE = "?theme:textSizeButton1";
constexpr double SYS_RES_BUTTON_WIDTH = 36.0;
const Dimension SYS_RES_BUTTON_HEIGHT = Dimension(36.0, DimensionUnit::VP);
const Color STATE_NORMAL_COLOR = Color(0x0c000000);
const Color STATE_PRESSED_COLOR = Color(0x19000000);
const Color THEME_BG_COLOR_TRANSPARENT = Color(0x33000000);
constexpr uint32_t SYS_RES_ID_COLOR = 117440512;
constexpr uint32_t SYS_RES_ID_DIMENSION = 117440513;
constexpr uint32_t SYS_RES_ID_DOUBLE = 117440514;
constexpr uint32_t SYS_RES_ID_INT = 117440515;
constexpr uint32_t SYS_RES_ID_STRING = 117440516;
const Color SYS_VALUE_COLOR = Color(0xffffffff);
const Dimension SYS_VALUE_DIMENSION = Dimension(10.0, DimensionUnit::VP);
constexpr double SYS_VALUE_DOUBLE = 10.0;
constexpr int32_t SYS_VALUE_INT = -10;
const std::string SYS_VALUE_STRING = "sans-serif";
#ifndef WEARABLE_PRODUCT
const Color COLOR_CONTROL_ACTIVATED_TV = Color(0xff266efb);
#endif

} // namespace

class ThemeConstantsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ThemeConstantsTest::SetUpTestCase()
{
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    g_themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
}

void ThemeConstantsTest::TearDownTestCase() {}
void ThemeConstantsTest::SetUp() {}
void ThemeConstantsTest::TearDown() {}

/**
 * @tc.name: PlatformConstants001
 * @tc.desc: Test get default theme at phone.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, PlatformConstants001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Mock device type is phone.
     */
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    ThemeConstants::InitDeviceType();

    /**
     * @tc.steps: step2. Get color THEME_OHOS_COLOR_CONTROL_ACTIVATED, check THEME_COLOR_FOREGROUND is 0xff0a59f7.
     * @tc.expected: step2. Color is 0xff0a59f7.
     */
    auto color = g_themeConstants->GetColor(THEME_OHOS_COLOR_CONTROL_ACTIVATED);
    EXPECT_EQ(color.GetValue(), COLOR_CONTROL_ACTIVATED_PHONE.GetValue());
}

#ifndef WEARABLE_PRODUCT
/**
 * @tc.name: PlatformConstants002
 * @tc.desc: Test get default theme at tv.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, PlatformConstants002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Mock device type is tv.
     */
    SystemProperties::SetDeviceType(DeviceType::TV);
    ThemeConstants::InitDeviceType();

    /**
     * @tc.steps: step2. Get color THEME_OHOS_COLOR_CONTROL_ACTIVATED, check THEME_COLOR_FOREGROUND is 0xff266efb.
     * @tc.expected: step2. Color is 0xff266efb.
     */
    auto color = g_themeConstants->GetColor(THEME_OHOS_COLOR_CONTROL_ACTIVATED);
    EXPECT_EQ(color.GetValue(), COLOR_CONTROL_ACTIVATED_TV.GetValue());
}
#endif

/**
 * @tc.name: PlatformConstants003
 * @tc.desc: Test get default theme at other platform.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, PlatformConstants003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Mock device type is unknown.
     */
    SystemProperties::SetDeviceType(DeviceType::UNKNOWN);
    ThemeConstants::InitDeviceType();

    /**
     * @tc.steps: step2. Get color THEME_OHOS_COLOR_CONTROL_ACTIVATED, check THEME_COLOR_FOREGROUND is 0xff0a59f7.
     * @tc.expected: step2. Color is 0xff0a59f7.
     */
    auto color = g_themeConstants->GetColor(THEME_OHOS_COLOR_CONTROL_ACTIVATED);
    EXPECT_EQ(color.GetValue(), COLOR_CONTROL_ACTIVATED_PHONE.GetValue());
}

/**
 * @tc.name: ConstantsDefine001
 * @tc.desc: Test get value with key corresponding with correct value type.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ConstantsDefine001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Mock device type is phone.
     */
    SystemProperties::SetDeviceType(DeviceType::PHONE);
    ThemeConstants::InitDeviceType();

    /**
     * @tc.steps: step2. Get color, dimension with correct key.
     * @tc.expected: step2. Returned value is correct.
     */
    auto color = g_themeConstants->GetColor(THEME_OHOS_COLOR_CONTROL_ACTIVATED);
    EXPECT_EQ(color.GetValue(), COLOR_CONTROL_ACTIVATED_PHONE.GetValue());
    auto dimension = g_themeConstants->GetDimension(THEME_BUTTON_TEXT_FONTSIZE);
    EXPECT_TRUE(NearEqual(dimension.Value(), BUTTON_TEXT_FONTSIZE_PHONE.Value()));
    EXPECT_EQ(dimension.Unit(), BUTTON_TEXT_FONTSIZE_PHONE.Unit());
}

/**
 * @tc.name: ConstantsDefine002
 * @tc.desc: Test get value with key corresponding with incorrect value type.
 * @tc.type: FUNC
 * @tc.require: AR000DAR29 AR000DAR2C
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ConstantsDefine002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Get color, dimension with incorrect key.
     * @tc.expected: step1. Returned value is correct.
     */
    auto color = g_themeConstants->GetColor(THEME_BUTTON_TEXT_FONTSIZE);
    EXPECT_EQ(color.GetValue(), COLOR_ERROR.GetValue());
    auto dimension = g_themeConstants->GetDimension(THEME_OHOS_COLOR_CONTROL_ACTIVATED);
    EXPECT_TRUE(NearEqual(dimension.Value(), DIMENSION_ERROR.Value()));
    EXPECT_EQ(dimension.Unit(), DIMENSION_ERROR.Unit());
}

/**
 * @tc.name: ParseStyle001
 * @tc.desc: Parse user input custom style config.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PR
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ParseStyle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct custom style json input.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    const std::string jsonStr = "{                                   "
                                "  \"style\": {                      "
                                "    \"000\": \"#ffff00\",           "
                                "    \"400\": \"0.5\",               "
                                "    \"500\": \"10px\",              "
                                "    \"550\": \"HwChinese-bold\",    "
                                "    \"700\": \"10px\",              "
                                "    \"780\": \"1.5\",               "
                                "    \"2020\": \"500\"               "
                                "  }"
                                "}";

    /**
     * @tc.steps: step2. Parse input json string, check style map.
     * @tc.expected: step2. Value in ThemeConstants is same with input custom style.
     */
    themeConstants->ParseCustomStyle(jsonStr);
    EXPECT_EQ(themeConstants->GetColor(THEME_OHOS_COLOR_FG).GetValue(), CUSTOM_COLOR_FG.GetValue());
    EXPECT_TRUE(NearEqual(themeConstants->GetDouble(THEME_OHOS_PRIMARY_CONTENT_ALPHA), CUSTOM_ALPHA));
    EXPECT_EQ(themeConstants->GetDimension(THEME_OHOS_TEXT_SIZE_HEADLINE1), CUSTOM_FONT_SIZE);
    EXPECT_EQ(themeConstants->GetDimension(THEME_OHOS_DIMENS_DEFAULT_START), CUSTOM_MARGIN);
    EXPECT_TRUE(NearEqual(themeConstants->GetDouble(THEME_OHOS_TEXT_SIZE_SPACE_SHORT), CUSTOM_LINE_HEIGHT));
    EXPECT_EQ(themeConstants->GetInt(THEME_DIALOG_ANIMATION_DURATION_IN), CUSTOM_ANIM_DURATION);
}

/**
 * @tc.name: ParseStyle002
 * @tc.desc: Parse user input custom style config, value format error.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PR
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ParseStyle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct custom style json input.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    const std::string jsonStr = "{                                      "
                                "  \"style\": {                         "
                                "    \"000\": \"0.5\",                  "
                                "    \"400\": \"#ff0000\",              "
                                "    \"500\": \"#ff0000\",              "
                                "    \"550\": \"400\",                  "
                                "    \"700\": \"#ff0000\",              "
                                "    \"780\": \"#ff0000\",              "
                                "    \"2020\": \"#ff0000\"              "
                                "  }"
                                "}";

    /**
     * @tc.steps: step2. Parse input json string, check style map.
     * @tc.expected: step2. Value in ThemeConstants is not changed, except font family.
     */
    themeConstants->ParseCustomStyle(jsonStr);
    EXPECT_EQ(themeConstants->GetColor(THEME_OHOS_COLOR_FG).GetValue(), COLOR_ERROR.GetValue());
    EXPECT_TRUE(NearEqual(themeConstants->GetDouble(THEME_OHOS_PRIMARY_CONTENT_ALPHA), 0.0));
    EXPECT_EQ(themeConstants->GetDimension(THEME_OHOS_TEXT_SIZE_HEADLINE1), PARSE_DIMENSION_ERROR);
    EXPECT_EQ(themeConstants->GetDimension(THEME_OHOS_DIMENS_DEFAULT_START), PARSE_DIMENSION_ERROR);
    EXPECT_TRUE(NearEqual(themeConstants->GetDouble(THEME_OHOS_TEXT_SIZE_SPACE_SHORT), 0.0));
    EXPECT_EQ(themeConstants->GetInt(THEME_DIALOG_ANIMATION_DURATION_IN), 0);
}

/**
 * @tc.name: ParseStyle003
 * @tc.desc: Parse user input custom style config, id not found.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PR
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ParseStyle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct custom style json input.
     */
    const std::string jsonStr = "{                                      "
                                "  \"style\": {                         "
                                "    \"9999\": \"0.5\",                 "
                                "  }"
                                "}";

    /**
     * @tc.steps: step2. Parse input json string, check style map.
     * @tc.expected: step2. Key 9999 in ThemeConstants is not exist.
     */
    g_themeConstants->ParseCustomStyle(jsonStr);
    EXPECT_EQ(g_themeConstants->GetValue(STYLE_ID_INVALID).type, ThemeConstantsType::ERROR);
}

/**
 * @tc.name: ParseStyle004
 * @tc.desc: Parse user input custom style config, id not public.
 * @tc.type: FUNC
 * @tc.require: AR000DQ1PR
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ParseStyle004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct custom style json input. Cache current style value.
     */
    const std::string jsonStr = "{                                      "
                                "  \"style\": {                         "
                                "    \"136\": \"#ff0000\",              "
                                "  }"
                                "}";
    const auto cacheColor = g_themeConstants->GetColor(THEME_OHOS_COLOR_TIPS_BG);

    /**
     * @tc.steps: step2. Parse input json string, check style map.
     * @tc.expected: step2. Style in ThemeConstants is not changed.
     */
    g_themeConstants->ParseCustomStyle(jsonStr);
    EXPECT_EQ(g_themeConstants->GetColor(THEME_OHOS_COLOR_TIPS_BG).GetValue(), cacheColor.GetValue());
}

/**
 * @tc.name: ThemeStyleRead001
 * @tc.desc: Initialize theme style correctly.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1L
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ThemeStyleRead001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct theme constants with mock adapter, load theme resource.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    themeConstants->LoadTheme(THEME_ID_VALID);

    /**
     * @tc.steps: step2. Check currentThemeStyle_ of themeConstants.
     * @tc.expected: step2. Style content in currentThemeStyle_ is same with mock data.
     */
    auto themeStyle = themeConstants->GetThemeStyle();
    ASSERT_NE(themeStyle, nullptr);
    // app background color
    EXPECT_EQ(themeStyle->GetAttr<Color>(THEME_ATTR_BG_COLOR, Color()).GetValue(), CUSTOM_COLOR_FG.GetValue());
    // button style
    auto buttonStyle = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_BUTTON, RefPtr<ThemeStyle>());
    ASSERT_NE(buttonStyle, nullptr);
    // button background color
    auto buttonBgStates = buttonStyle->GetAttr<RefPtr<StateResource>>(PATTERN_BG_COLOR, RefPtr<StateResource>());
    ASSERT_NE(buttonBgStates, nullptr);
    EXPECT_EQ(
        buttonBgStates->GetState<Color>(STATE_NORMAL, Color()).GetValue(), STATE_NORMAL_COLOR.GetValue());
    EXPECT_EQ(
        buttonBgStates->GetState<Color>(STATE_PRESSED, Color()).GetValue(), STATE_PRESSED_COLOR.GetValue());
    // button text size
    EXPECT_EQ(buttonStyle->GetAttr<std::string>(PATTERN_TEXT_SIZE, ""), TEXT_SIZE_VALUE);
    // button width
    EXPECT_TRUE(NearEqual(buttonStyle->GetAttr<double>(PATTERN_WIDTH, 0.0), SYS_RES_BUTTON_WIDTH));
    // button height
    EXPECT_EQ(buttonStyle->GetAttr<Dimension>(PATTERN_HEIGHT, 0.0_vp), SYS_RES_BUTTON_HEIGHT);
}

/**
 * @tc.name: ThemeStyleRead002
 * @tc.desc: Initialize theme style, get attributes with wrong type.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1L
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ThemeStyleRead002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct theme constants with mock adapter, load theme resource.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    themeConstants->LoadTheme(THEME_ID_VALID);

    /**
     * @tc.steps: step2. Check currentThemeStyle_ of themeConstants.
     * @tc.expected: step2. Style content in currentThemeStyle_ is same with mock data.
     */
    auto themeStyle = themeConstants->GetThemeStyle();
    ASSERT_NE(themeStyle, nullptr);
    // app background color
    auto bgColor = themeStyle->GetAttr<Dimension>(THEME_ATTR_BG_COLOR, Dimension());
    EXPECT_EQ(bgColor.Value(), 0.0);
    EXPECT_EQ(bgColor.Unit(), DimensionUnit::PX);
    // button style
    auto buttonStyle = themeStyle->GetAttr<RefPtr<ThemeStyle>>(THEME_PATTERN_BUTTON, RefPtr<ThemeStyle>());
    ASSERT_NE(buttonStyle, nullptr);
    // button background color
    auto buttonBgStyle = buttonStyle->GetAttr<double>(PATTERN_BG_COLOR, 0.0);
    EXPECT_TRUE(NearEqual(buttonBgStyle, 0.0));
    // button text size
    EXPECT_EQ(buttonStyle->GetAttr<RefPtr<ThemeStyle>>(PATTERN_TEXT_SIZE, RefPtr<ThemeStyle>()), nullptr);
    // button width
    EXPECT_EQ(buttonStyle->GetAttr<Color>(PATTERN_WIDTH, Color()).GetValue(), Color().GetValue());
    // button height
    EXPECT_EQ(buttonStyle->GetAttr<std::string>(PATTERN_HEIGHT, ""), "");
}

/**
 * @tc.name: ThemeStyleRead003
 * @tc.desc: Initialize theme style with incorrect theme id.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1L
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ThemeStyleRead003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct theme constants with mock adapter, load theme resource with invalid theme id.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    themeConstants->LoadTheme(THEME_ID_INVALID);

    /**
     * @tc.steps: step2. Check currentThemeStyle_ of themeConstants.
     * @tc.expected: step2. currentThemeStyle_ is nullptr.
     */
    EXPECT_EQ(themeConstants->currentThemeStyle_, nullptr);
}

/**
 * @tc.name: ThemeStyleRead004
 * @tc.desc: Initialize theme style correctly, override app background color.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1L
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ThemeStyleRead004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct theme constants with mock adapter, load theme resource, set ColorScheme::TRANSPARENT.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    themeConstants->LoadTheme(THEME_ID_VALID);
    themeConstants->SetColorScheme(ColorScheme::SCHEME_TRANSPARENT);

    /**
     * @tc.steps: step2. Check app background color of currentThemeStyle_.
     * @tc.expected: step2. App background color is .
     */
    auto themeStyle = themeConstants->GetThemeStyle();
    EXPECT_EQ(
        themeStyle->GetAttr<Color>(THEME_ATTR_BG_COLOR, Color()).GetValue(), THEME_BG_COLOR_TRANSPARENT.GetValue());
}

/**
 * @tc.name: ThemeResourceRead001
 * @tc.desc: Read theme resource correctly.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1N
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ThemeResourceRead001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct theme constants with mock adapter, load theme resource.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);

    /**
     * @tc.steps: step2. Get different type of resources from themeConstants.
     * @tc.expected: step2. Resource value is same with ResourceAdapterMock.
     */
    auto color = themeConstants->GetColor(SYS_RES_ID_COLOR);
    EXPECT_EQ(color.GetValue(), SYS_VALUE_COLOR.GetValue());
    auto dimension = themeConstants->GetDimension(SYS_RES_ID_DIMENSION);
    EXPECT_TRUE(NearEqual(dimension.Value(), SYS_VALUE_DIMENSION.Value()));
    EXPECT_EQ(dimension.Unit(), SYS_VALUE_DIMENSION.Unit());
    auto doubleValue = themeConstants->GetDouble(SYS_RES_ID_DOUBLE);
    EXPECT_TRUE(NearEqual(doubleValue, SYS_VALUE_DOUBLE));
    auto intValue = themeConstants->GetInt(SYS_RES_ID_INT);
    EXPECT_EQ(intValue, SYS_VALUE_INT);
    auto str = themeConstants->GetString(SYS_RES_ID_STRING);
    EXPECT_EQ(str, SYS_VALUE_STRING);
}

/**
 * @tc.name: ThemeResourceRead002
 * @tc.desc: Read theme resource with wrong type.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1N
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, ThemeResourceRead002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct theme constants with mock adapter, load theme resource.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);

    /**
     * @tc.steps: step2. Get value from themeConstants use wrong interface.
     * @tc.expected: step2. Resource value is same with error value.
     */
    auto dimension = themeConstants->GetDimension(SYS_RES_ID_COLOR);
    EXPECT_TRUE(NearEqual(dimension.Value(), 0.0));
    EXPECT_EQ(dimension.Unit(), DimensionUnit::VP);
    auto doubleValue = themeConstants->GetDouble(SYS_RES_ID_DIMENSION);
    EXPECT_TRUE(NearEqual(doubleValue, 0.0));
    auto intValue = themeConstants->GetInt(SYS_RES_ID_DOUBLE);
    EXPECT_EQ(intValue, 0);
    auto str = themeConstants->GetString(SYS_RES_ID_INT);
    EXPECT_EQ(str, "");
    auto color = themeConstants->GetColor(SYS_RES_ID_STRING);
    EXPECT_EQ(color.GetValue(), Color().GetValue());
}

/**
 * @tc.name: MultiInstance001
 * @tc.desc: Parse custom style with multi theme constants.
 * @tc.type: FUNC
 * @tc.require: AR000F3L1M
 * @tc.author: liruokun
 */
HWTEST_F(ThemeConstantsTest, MultiInstance001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct custom style json input.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstantsA = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    auto themeConstantsB = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    const std::string jsonStrA = "{                                      "
                                 "  \"style\": {                         "
                                 "    \"000\": \"#ff0000\"               "
                                 "  }"
                                 "}";
    const std::string jsonStrB = "{                                      "
                                 "  \"style\": {                         "
                                 "    \"000\": \"#0000ff\"               "
                                 "  }"
                                 "}";

    /**
     * @tc.steps: step2. Parse input json string, check style map.
     * @tc.expected: step2. Value in ThemeConstants is not changed, except font family.
     */
    themeConstantsA->ParseCustomStyle(jsonStrA);
    themeConstantsB->ParseCustomStyle(jsonStrB);
    EXPECT_EQ(themeConstantsA->GetColor(THEME_OHOS_COLOR_FG).GetValue(), Color::RED.GetValue());
    EXPECT_EQ(themeConstantsB->GetColor(THEME_OHOS_COLOR_FG).GetValue(), Color::BLUE.GetValue());
}

/**
 * @tc.name: ParseIdStyle001
 * @tc.desc: Parse user input id style config.
 * @tc.type: FUNC
 * @tc.require: AR000FDGO4
 * @tc.author: yangfan
 */
HWTEST_F(ThemeConstantsTest, ParseIdStyle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct custom style json input.
     */
    auto resAdapter = AceType::MakeRefPtr<ResourceAdapterMock>();
    auto themeConstants = AceType::MakeRefPtr<ThemeConstants>(resAdapter);
    const std::string jsonStr = "@ohos_id_500";
    auto parseResult = ThemeUtils::ParseThemeIdReference(jsonStr);
    auto parseColor = themeConstants->GetColor(parseResult.id);

    /**
     * @tc.steps: step2. Check style map.
     * @tc.expected: step2. Style in ThemeConstants is not changed.
     */
    Color correctColor = Color(0xff5434ff);
    EXPECT_EQ(correctColor.GetValue(), parseColor.GetValue());
}

} // namespace OHOS::Ace
