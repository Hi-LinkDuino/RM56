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

#include "animator/interpolation.h"

#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
namespace {
    constexpr float U1 = 0.5f;
    constexpr float U2 = 0.25f;
    constexpr float U3 = 0.125f;
    constexpr float U4 = 0.5f;
    constexpr float PRECISION = 0.001f;
}
class InterpolationTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
};

/**
 * @tc.name: InterpolationGetBezierInterpolation_001
 * @tc.desc: Verify GetBezierInterpolation function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(InterpolationTest, InterpolationGetBezierInterpolation_001, TestSize.Level0)
{
    float ret = Interpolation::GetBezierInterpolation(0.2f, U1, U2, U3, U4); // 0.2f:current change rate;
    EXPECT_NEAR(0.368, ret, PRECISION); // 0.368:current change rate;
    ret = Interpolation::GetBezierInterpolation(0.5f, U1, U2, U3, U4); // 0.5f:current change rate;
    EXPECT_NEAR(0.266, ret, PRECISION); // 0.266:current change rate;
    ret = Interpolation::GetBezierInterpolation(0.7f, U1, U2, U3, U4); // 0.7f:current change rate;
    EXPECT_NEAR(0.287, ret, PRECISION); // 0.287:current change rate;
    ret = Interpolation::GetBezierInterpolation(0.9f, U1, U2, U3, U4); // 0.9f:current change rate;
    EXPECT_NEAR(0.402, ret, PRECISION); // 0.402:current change rate;
}

/**
 * @tc.name: GetBezierY_001
 * @tc.desc: Verify GetBezierY function, equal.
 * @tc.type: FUNC
 * @tc.require: AR000DSMQM
 */
HWTEST_F(InterpolationTest, GetBezierY_001, TestSize.Level0)
{
    float ret = Interpolation::GetBezierY(0.2f, U1, U2, U3, U4); // 0.2f:current change rate;
    EXPECT_NEAR(0.152, ret, PRECISION); // 0.152:current change rate;
    ret = Interpolation::GetBezierY(0.5f, U1, U2, U3, U4); // 0.5f:current change rate;
    EXPECT_NEAR(0.635, ret, PRECISION); // 0.635:current change rate;
    ret = Interpolation::GetBezierY(0.7f, U1, U2, U3, U4); // 0.7f:current change rate;
    EXPECT_NEAR(0.811, ret, PRECISION); // 0.811:current change rate;
    ret = Interpolation::GetBezierY(0.9f, U1, U2, U3, U4); // 0.9f:current change rate;
    EXPECT_NEAR(0.949, ret, PRECISION); // 0.949:current change rate;
}
} // namespace OHOS
