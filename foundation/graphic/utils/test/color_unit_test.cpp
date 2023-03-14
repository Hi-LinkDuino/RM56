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

#include "gfx_utils/color.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class ColorTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: ColorGetColorFromRGB_001
 * @tc.desc: Verify GetColorFromRGB function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorGetColorFromRGB_001, TestSize.Level0)
{
    ColorType color = Color::GetColorFromRGB(0, 0, 0);

    EXPECT_EQ(color.red, 0);
    EXPECT_EQ(color.green, 0);
    EXPECT_EQ(color.blue, 0);
    EXPECT_EQ(color.alpha, OPA_OPAQUE);
}

/**
 * @tc.name: ColorGetColorFromRGBA_002
 * @tc.desc: Verify GetColorFromRGBA function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorGetColorFromRGBA_002, TestSize.Level0)
{
    ColorType color = Color::GetColorFromRGBA(0, 0, 0, OPA_TRANSPARENT);

    EXPECT_EQ(color.red, 0);
    EXPECT_EQ(color.green, 0);
    EXPECT_EQ(color.blue, 0);
    EXPECT_EQ(color.alpha, OPA_TRANSPARENT);
}

/**
 * @tc.name: ColorColorTo32_001
 * @tc.desc: Verify ColorTo32 function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorColorTo32_001, TestSize.Level0)
{
    ColorType color;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = OPA_TRANSPARENT;
    uint32_t color32 = 0;

    EXPECT_EQ(Color::ColorTo32(color), color32);
}

/**
 * @tc.name: ColorColorTo32_002
 * @tc.desc: Verify ColorTo32 function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorColorTo32_002, TestSize.Level0)
{
    Color16 color16;
    color16.red = 0;
    color16.green = 0;
    color16.blue = 0;

    EXPECT_EQ(Color::ColorTo32(color16, 0), 0);
}

/**
 * @tc.name: ColorColorTo16_001
 * @tc.desc: Verify ColorTo16 function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorColorTo16_001, TestSize.Level0)
{
    Color32 color32;
    color32.red = 0;
    color32.green = 0;
    color32.blue = 0;

    EXPECT_EQ(Color::ColorTo16(color32), 0);
}

/**
 * @tc.name: ColorWhite_001
 * @tc.desc: Verify White function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorWhite_001, TestSize.Level0)
{
    ColorType white = Color::White();

    EXPECT_EQ(white.red, 0xFF);
    EXPECT_EQ(white.green, 0xFF);
    EXPECT_EQ(white.blue, 0xFF);
}

/**
 * @tc.name: ColorSilver_001
 * @tc.desc: Verify Silver function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorSilver_001, TestSize.Level0)
{
    ColorType silver = Color::Silver();

    EXPECT_EQ(silver.red, 0xC0);
    EXPECT_EQ(silver.green, 0xC0);
    EXPECT_EQ(silver.blue, 0xC0);
}

/**
 * @tc.name: ColorGray_001
 * @tc.desc: Verify Gray function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorGray_001, TestSize.Level0)
{
    ColorType gray = Color::Gray();

    EXPECT_EQ(gray.red, 0x80);
    EXPECT_EQ(gray.green, 0x80);
    EXPECT_EQ(gray.blue, 0x80);
}

/**
 * @tc.name: ColorBlack_001
 * @tc.desc: Verify Black function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorBlack_001, TestSize.Level0)
{
    ColorType black = Color::Black();

    EXPECT_EQ(black.red, 0x00);
    EXPECT_EQ(black.green, 0x00);
    EXPECT_EQ(black.blue, 0x00);
}

/**
 * @tc.name: ColorRed_001
 * @tc.desc: Verify Red function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorRed_001, TestSize.Level0)
{
    ColorType red = Color::Red();

    EXPECT_EQ(red.red, 0xFF);
    EXPECT_EQ(red.green, 0x00);
    EXPECT_EQ(red.blue, 0x00);
}

/**
 * @tc.name: ColorMaroon_001
 * @tc.desc: Verify Maroon function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorMaroon_001, TestSize.Level0)
{
    ColorType maroon = Color::Maroon();

    EXPECT_EQ(maroon.red, 0x80);
    EXPECT_EQ(maroon.green, 0x00);
    EXPECT_EQ(maroon.blue, 0x00);
}

/**
 * @tc.name: ColorYellow_001
 * @tc.desc: Verify Yellow function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorYellow_001, TestSize.Level0)
{
    ColorType yellow = Color::Yellow();

    EXPECT_EQ(yellow.red, 0xFF);
    EXPECT_EQ(yellow.green, 0xFF);
    EXPECT_EQ(yellow.blue, 0x00);
}

/**
 * @tc.name: ColorOlive_001
 * @tc.desc: Verify Olive function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorOlive_001, TestSize.Level0)
{
    ColorType olive = Color::Olive();

    EXPECT_EQ(olive.red, 0x80);
    EXPECT_EQ(olive.green, 0x80);
    EXPECT_EQ(olive.blue, 0x00);
}

/**
 * @tc.name: ColorLime_001
 * @tc.desc: Verify Lime function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorLime_001, TestSize.Level0)
{
    ColorType lime = Color::Lime();

    EXPECT_EQ(lime.red, 0x00);
    EXPECT_EQ(lime.green, 0xFF);
    EXPECT_EQ(lime.blue, 0x00);
}

/**
 * @tc.name: ColorGreen_001
 * @tc.desc: Verify Green function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorGreen_001, TestSize.Level0)
{
    ColorType green = Color::Green();

    EXPECT_EQ(green.red, 0x00);
    EXPECT_EQ(green.green, 0xFF);
    EXPECT_EQ(green.blue, 0x00);
}

/**
 * @tc.name: ColorCyan_001
 * @tc.desc: Verify Cyan function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorCyan_001, TestSize.Level0)
{
    ColorType cyan = Color::Cyan();

    EXPECT_EQ(cyan.red, 0x00);
    EXPECT_EQ(cyan.green, 0xFF);
    EXPECT_EQ(cyan.blue, 0xFF);
}

/**
 * @tc.name: ColorAqua_001
 * @tc.desc: Verify Aqua function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorAqua_001, TestSize.Level0)
{
    ColorType aqua = Color::Aqua();

    EXPECT_EQ(aqua.red, 0x00);
    EXPECT_EQ(aqua.green, 0xFF);
    EXPECT_EQ(aqua.blue, 0xFF);
}

/**
 * @tc.name: ColorTeal_001
 * @tc.desc: Verify Teal function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorTeal_001, TestSize.Level0)
{
    ColorType teal = Color::Teal();

    EXPECT_EQ(teal.red, 0x00);
    EXPECT_EQ(teal.green, 0x80);
    EXPECT_EQ(teal.blue, 0x80);
}

/**
 * @tc.name: ColorBlue_001
 * @tc.desc: Verify Blue function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorBlue_001, TestSize.Level0)
{
    ColorType blue = Color::Blue();

    EXPECT_EQ(blue.red, 0x00);
    EXPECT_EQ(blue.green, 0x00);
    EXPECT_EQ(blue.blue, 0xFF);
}

/**
 * @tc.name: ColorNavy_001
 * @tc.desc: Verify Navy function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorNavy_001, TestSize.Level0)
{
    ColorType navy = Color::Navy();

    EXPECT_EQ(navy.red, 0x00);
    EXPECT_EQ(navy.green, 0x00);
    EXPECT_EQ(navy.blue, 0x80);
}

/**
 * @tc.name: ColorMagenta_001
 * @tc.desc: Verify Magenta function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorMagenta_001, TestSize.Level0)
{
    ColorType magenta = Color::Magenta();

    EXPECT_EQ(magenta.red, 0xFF);
    EXPECT_EQ(magenta.green, 0x00);
    EXPECT_EQ(magenta.blue, 0xFF);
}

/**
 * @tc.name: ColorPurple_001
 * @tc.desc: Verify Purple function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorPurple_001, TestSize.Level0)
{
    ColorType purple = Color::Purple();

    EXPECT_EQ(purple.red, 0x80);
    EXPECT_EQ(purple.green, 0x00);
    EXPECT_EQ(purple.blue, 0x80);
}

/**
 * @tc.name: ColorOrange_001
 * @tc.desc: Verify Orange function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQ9
 */
HWTEST_F(ColorTest, ColorOrange_001, TestSize.Level0)
{
    ColorType orange = Color::Orange();

    EXPECT_EQ(orange.red, 0xFF);
    EXPECT_EQ(orange.green, 0xA5);
    EXPECT_EQ(orange.blue, 0x00);
}
} // namespace OHOS
