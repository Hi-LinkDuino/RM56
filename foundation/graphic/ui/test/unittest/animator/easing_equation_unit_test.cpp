/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "animator/easing_equation.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    const int16_t START_POS = 0;
    const int16_t END_POS = 100;
    const int16_t DURATION_TIME = 4;
}

class EasingEquationTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: EasingEquationBackEaseIn_001
 * @tc.desc: Verify BackEaseIn and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationBackEaseIn_001, TestSize.Level0)
{
    const double overShoot = 2;
    EasingEquation::SetBackOvershoot(overShoot);
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::BackEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::BackEaseIn(START_POS, END_POS, i++, DURATION_TIME), -7); // -7:the value for 1th s;
    EXPECT_EQ(EasingEquation::BackEaseIn(START_POS, END_POS, i++, DURATION_TIME), -12); // -12:the value for 2th s;
    EXPECT_EQ(EasingEquation::BackEaseIn(START_POS, END_POS, i++, DURATION_TIME), 14); // 14:the value for 3th s;
    EXPECT_EQ(EasingEquation::BackEaseIn(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationBackEaseOut_001
 * @tc.desc: Verify BackEaseOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationBackEaseOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::BackEaseOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::BackEaseOut(START_POS, END_POS, i++, DURATION_TIME), 85); // 85:the value for 1th s;
    EXPECT_EQ(EasingEquation::BackEaseOut(START_POS, END_POS, i++, DURATION_TIME), 112); // 112:the value for 2th s;
    EXPECT_EQ(EasingEquation::BackEaseOut(START_POS, END_POS, i++, DURATION_TIME), 107); // 107:the value for 3th s;
    EXPECT_EQ(EasingEquation::BackEaseOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationBackEaseInOut_001
 * @tc.desc: Verify BackEaseInOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationBackEaseInOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::BackEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::BackEaseInOut(START_POS, END_POS, i++, DURATION_TIME), -6); // -6:the value for 1th s;
    EXPECT_EQ(EasingEquation::BackEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::BackEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 106); // 106:the value for 3th s;
    EXPECT_EQ(EasingEquation::BackEaseInOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationCircEaseIn_001
 * @tc.desc: Verify CircEaseIn and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationCircEaseIn_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::CircEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::CircEaseIn(START_POS, END_POS, i++, DURATION_TIME), 3); // 3:the value for 1th s;
    EXPECT_EQ(EasingEquation::CircEaseIn(START_POS, END_POS, i++, DURATION_TIME), 13); // 13:the value for 2th s;
    EXPECT_EQ(EasingEquation::CircEaseIn(START_POS, END_POS, i++, DURATION_TIME), 33); // 33:the value for 3th s;
    EXPECT_EQ(EasingEquation::CircEaseIn(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationCircEaseOut_001
 * @tc.desc: Verify CircEaseOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationCircEaseOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::CircEaseOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::CircEaseOut(START_POS, END_POS, i++, DURATION_TIME), 66); // 66:the value for 1th s;
    EXPECT_EQ(EasingEquation::CircEaseOut(START_POS, END_POS, i++, DURATION_TIME), 86); // 86:the value for 2th s;
    EXPECT_EQ(EasingEquation::CircEaseOut(START_POS, END_POS, i++, DURATION_TIME), 96); // 96:the value for 3th s;
    EXPECT_EQ(EasingEquation::CircEaseOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationCircEaseInOut_001
 * @tc.desc: Verify CircEaseInOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationCircEaseInOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::CircEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::CircEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 6); // 6:the value for 1th s;
    EXPECT_EQ(EasingEquation::CircEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::CircEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 93); // 93:the value for 3th s;
    EXPECT_EQ(EasingEquation::CircEaseInOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationCubicEaseIn_001
 * @tc.desc: Verify CubicEaseIn and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationCubicEaseIn_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::CubicEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::CubicEaseIn(START_POS, END_POS, i++, DURATION_TIME), 1); // 1:the value for 1th s;
    EXPECT_EQ(EasingEquation::CubicEaseIn(START_POS, END_POS, i++, DURATION_TIME), 12); // 12:the value for 2th s;
    EXPECT_EQ(EasingEquation::CubicEaseIn(START_POS, END_POS, i++, DURATION_TIME), 42); // 42:the value for 3th s;
    EXPECT_EQ(EasingEquation::CubicEaseIn(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationCubicEaseOut_001
 * @tc.desc: Verify CubicEaseOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationCubicEaseOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::CubicEaseOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::CubicEaseOut(START_POS, END_POS, i++, DURATION_TIME), 57); // 57:the value for 1th s;
    EXPECT_EQ(EasingEquation::CubicEaseOut(START_POS, END_POS, i++, DURATION_TIME), 87); // 87:the value for 2th s;
    EXPECT_EQ(EasingEquation::CubicEaseOut(START_POS, END_POS, i++, DURATION_TIME), 98); // 98:the value for 3th s;
    EXPECT_EQ(EasingEquation::CubicEaseOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationCubicEaseInOut_001
 * @tc.desc: Verify CubicEaseInOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationCubicEaseInOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::CubicEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::CubicEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 6); // 6:the value for 1th s;
    EXPECT_EQ(EasingEquation::CubicEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::CubicEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 93); // 93:the value for 3th s;
    EXPECT_EQ(EasingEquation::CubicEaseInOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationLinearEaseNone_001
 * @tc.desc: Verify LinearEaseNone and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationLinearEaseNone_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::LinearEaseNone(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::LinearEaseNone(START_POS, END_POS, i++, DURATION_TIME), 25); // 25:the value for 1th s;
    EXPECT_EQ(EasingEquation::LinearEaseNone(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::LinearEaseNone(START_POS, END_POS, i++, DURATION_TIME), 75); // 75:the value for 3th s;
    EXPECT_EQ(EasingEquation::LinearEaseNone(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationQuadEaseIn_001
 * @tc.desc: Verify QuadEaseIn and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationQuadEaseIn_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::QuadEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::QuadEaseIn(START_POS, END_POS, i++, DURATION_TIME), 6); // 6:the value for 1th s;
    EXPECT_EQ(EasingEquation::QuadEaseIn(START_POS, END_POS, i++, DURATION_TIME), 25); // 25:the value for 2th s;
    EXPECT_EQ(EasingEquation::QuadEaseIn(START_POS, END_POS, i++, DURATION_TIME), 56); // 56:the value for 3th s;
    EXPECT_EQ(EasingEquation::QuadEaseIn(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationQuadEaseOut_001
 * @tc.desc: Verify QuadEaseOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationQuadEaseOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::QuadEaseOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::QuadEaseOut(START_POS, END_POS, i++, DURATION_TIME), 43); // 43:the value for 1th s;
    EXPECT_EQ(EasingEquation::QuadEaseOut(START_POS, END_POS, i++, DURATION_TIME), 75); // 75:the value for 2th s;
    EXPECT_EQ(EasingEquation::QuadEaseOut(START_POS, END_POS, i++, DURATION_TIME), 93); // 93:the value for 3th s;
    EXPECT_EQ(EasingEquation::QuadEaseOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationQuadEaseInOut_001
 * @tc.desc: Verify QuadEaseInOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationQuadEaseInOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::QuadEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::QuadEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 12); // 12:the value for 1th s;
    EXPECT_EQ(EasingEquation::QuadEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::QuadEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 87); // 87:the value for 3th s;
    EXPECT_EQ(EasingEquation::QuadEaseInOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationQuintEaseIn_001
 * @tc.desc: Verify QuintEaseIn and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationQuintEaseIn_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::QuintEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::QuintEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0); // 0:the value for 1th s;
    EXPECT_EQ(EasingEquation::QuintEaseIn(START_POS, END_POS, i++, DURATION_TIME), 3); // 3:the value for 2th s;
    EXPECT_EQ(EasingEquation::QuintEaseIn(START_POS, END_POS, i++, DURATION_TIME), 23); // 23:the value for 3th s;
    EXPECT_EQ(EasingEquation::QuintEaseIn(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationQuintEaseOut_001
 * @tc.desc: Verify QuintEaseOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationQuintEaseOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::QuintEaseOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::QuintEaseOut(START_POS, END_POS, i++, DURATION_TIME), 76); // 76:the value for 1th s;
    EXPECT_EQ(EasingEquation::QuintEaseOut(START_POS, END_POS, i++, DURATION_TIME), 96); // 96:the value for 2th s;
    EXPECT_EQ(EasingEquation::QuintEaseOut(START_POS, END_POS, i++, DURATION_TIME), 99); // 99:the value for 3th s;
    EXPECT_EQ(EasingEquation::QuintEaseOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationQuintEaseInOut_001
 * @tc.desc: Verify QuintEaseInOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationQuintEaseInOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::QuintEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::QuintEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 1); // 1:the value for 1th s;
    EXPECT_EQ(EasingEquation::QuintEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::QuintEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 98); // 87:the value for 3th s;
    EXPECT_EQ(EasingEquation::QuintEaseInOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationSineEaseIn_001
 * @tc.desc: Verify SineEaseIn and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationSineEaseIn_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::SineEaseIn(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::SineEaseIn(START_POS, END_POS, i++, DURATION_TIME), 7); // 7:the value for 1th s;
    EXPECT_EQ(EasingEquation::SineEaseIn(START_POS, END_POS, i++, DURATION_TIME), 29); // 29:the value for 2th s;
    EXPECT_EQ(EasingEquation::SineEaseIn(START_POS, END_POS, i++, DURATION_TIME), 60); // 60:the value for 3th s;
    EXPECT_EQ(EasingEquation::SineEaseIn(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationSineEaseOut_001
 * @tc.desc: Verify SineEaseOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationSineEaseOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::SineEaseOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::SineEaseOut(START_POS, END_POS, i++, DURATION_TIME), 37); // 37:the value for 1th s;
    EXPECT_EQ(EasingEquation::SineEaseOut(START_POS, END_POS, i++, DURATION_TIME), 70); // 70:the value for 2th s;
    EXPECT_EQ(EasingEquation::SineEaseOut(START_POS, END_POS, i++, DURATION_TIME), 92); // 92:the value for 3th s;
    EXPECT_EQ(EasingEquation::SineEaseOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}

/**
 * @tc.name: EasingEquationSineEaseInOut_001
 * @tc.desc: Verify SineEaseInOut and GetHeight function.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(EasingEquationTest, EasingEquationSineEaseInOut_001, TestSize.Level1)
{
    uint16_t i = 0;
    EXPECT_EQ(EasingEquation::SineEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 0);
    EXPECT_EQ(EasingEquation::SineEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 14); // 14:the value for 1th s;
    EXPECT_EQ(EasingEquation::SineEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 50); // 50:the value for 2th s;
    EXPECT_EQ(EasingEquation::SineEaseInOut(START_POS, END_POS, i++, DURATION_TIME), 85); // 85:the value for 3th s;
    EXPECT_EQ(EasingEquation::SineEaseInOut(START_POS, END_POS, i++, DURATION_TIME), END_POS);
}
} // namespace OHOS
