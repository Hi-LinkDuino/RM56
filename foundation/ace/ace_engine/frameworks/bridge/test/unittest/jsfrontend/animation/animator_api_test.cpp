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

#include "frameworks/bridge/js_frontend/engine/quickjs/animation_bridge.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const int32_t ANIMATION_ITERATIONS_DEFAULT = 1;
const int32_t ANIMATION_ITERATIONS_INFINITY = -1;
const double ANIMATION_DURATIONS_DEFAULT = 3000;
const double ANIMATION_DELAY_DEFAULT = 1000;
const double ANIMATION_FROM_DEFAULT = 100;
const double ANIMATION_TO_DEFAULT = 300;
const std::string ANIMATION_EASING_DEFAULT = "linear";
const std::string ANIMATION_FILL_DEFAULT = "forwards";
const std::string ANIMATION_FILL_NONE = "none";
const std::string ANIMATION_EASING_ANTICIPATE = "anticipate";
const std::string ANIMATION_EASING_CUBIC = "cubic-bezier(0.0, 0.0, 0.0, 1.0)";

const std::string PARAMS_DEFAULT_TEST = ""
                                        "{                                  "
                                        "        \"duration\": 3000,        "
                                        "        \"easing\": \"linear\",    "
                                        "        \"delay\": 1000,           "
                                        "        \"fill\": \"forwards\",    "
                                        "        \"iterations\": 1,         "
                                        "        \"begin\": 100,            "
                                        "        \"end\": 300               "
                                        "}";

const std::string ANTICIPATE_CURVE_TEST = ""
                                          "{                                  "
                                          "        \"duration\": 3000,        "
                                          "        \"easing\": \"anticipate\","
                                          "        \"delay\": 1000,           "
                                          "        \"fill\": \"forwards\",    "
                                          "        \"iterations\": 1,         "
                                          "        \"begin\": 100,            "
                                          "        \"end\": 300               "
                                          "}";

const std::string CUBIC_CURVE_TEST = ""
                                     "{                                                        "
                                     "        \"duration\": 3000,                              "
                                     "        \"easing\": \"cubic-bezier(0.0, 0.0, 0.0, 1.0)\","
                                     "        \"delay\": 1000,                                 "
                                     "        \"fill\": \"forwards\",                          "
                                     "        \"iterations\": 1,                               "
                                     "        \"begin\": 100,                                  "
                                     "        \"end\": 300                                     "
                                     "}";

const std::string ITERATIONS_INFINITY_TEST = ""
                                             "{                                       "
                                             "        \"duration\": 3000,             "
                                             "        \"easing\": \"linear\",     "
                                             "        \"delay\": 1000,                "
                                             "        \"fill\": \"forwards\",         "
                                             "        \"iterations\": \"Infinity\",   "
                                             "        \"begin\": 100,                 "
                                             "        \"end\": 300                    "
                                             "}";

const std::string ITERATIONS_STRING_TEST = ""
                                           "{                                       "
                                           "        \"duration\": 3000,             "
                                           "        \"easing\": \"linear\",     "
                                           "        \"delay\": 1000,                "
                                           "        \"fill\": \"forwards\",         "
                                           "        \"iterations\": \"1\",          "
                                           "        \"begin\": 100,                 "
                                           "        \"end\": 300                    "
                                           "}";

const std::string FILLMODE_STRING_TEST = ""
                                         "{                                       "
                                         "        \"duration\": 3000,             "
                                         "        \"easing\": \"linear\",     "
                                         "        \"delay\": 1000,                "
                                         "        \"fill\": \"none\",             "
                                         "        \"iterations\": \"1\",          "
                                         "        \"begin\": 100,                 "
                                         "        \"end\": 300                    "
                                         "}";

} // namespace

class AnimatorApiTest : public testing::Test, public BaseAnimationBridgeUtils {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AnimatorApiTest::SetUpTestCase() {}
void AnimatorApiTest::TearDownTestCase() {}
void AnimatorApiTest::SetUp() {}
void AnimatorApiTest::TearDown() {}

/**
 * @tc.name: AnimatorApiTest001
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimatorApiTest, AnimatorApiTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the params of default.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    JsParseAnimatorParams(PARAMS_DEFAULT_TEST, iterations, animationDoubleParams, animationStringParams);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringParams.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringParams.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API)->second;
    auto from = animationDoubleParams.find(DOM_ANIMATION_BEGIN)->second;
    auto to = animationDoubleParams.find(DOM_ANIMATION_END)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_DEFAULT, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_FROM_DEFAULT, from);
    EXPECT_EQ(ANIMATION_TO_DEFAULT, to);
    EXPECT_EQ(ANIMATION_EASING_DEFAULT, easing);
    EXPECT_EQ(ANIMATION_FILL_DEFAULT, fill);
}

/**
 * @tc.name: AnimatorApiTest002
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimatorApiTest, AnimatorApiTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the params of curve.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    JsParseAnimatorParams(ANTICIPATE_CURVE_TEST, iterations, animationDoubleParams, animationStringParams);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringParams.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringParams.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API)->second;
    auto from = animationDoubleParams.find(DOM_ANIMATION_BEGIN)->second;
    auto to = animationDoubleParams.find(DOM_ANIMATION_END)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_DEFAULT, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_FROM_DEFAULT, from);
    EXPECT_EQ(ANIMATION_TO_DEFAULT, to);
    EXPECT_EQ(ANIMATION_EASING_ANTICIPATE, easing);
    EXPECT_EQ(ANIMATION_FILL_DEFAULT, fill);
}

/**
 * @tc.name: AnimatorApiTest003
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimatorApiTest, AnimatorApiTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the params of curve.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    JsParseAnimatorParams(CUBIC_CURVE_TEST, iterations, animationDoubleParams, animationStringParams);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringParams.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringParams.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API)->second;
    auto from = animationDoubleParams.find(DOM_ANIMATION_BEGIN)->second;
    auto to = animationDoubleParams.find(DOM_ANIMATION_END)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_DEFAULT, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_FROM_DEFAULT, from);
    EXPECT_EQ(ANIMATION_TO_DEFAULT, to);
    EXPECT_EQ(ANIMATION_EASING_CUBIC, easing);
    EXPECT_EQ(ANIMATION_FILL_DEFAULT, fill);
}

/**
 * @tc.name: AnimatorApiTest004
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimatorApiTest, AnimatorApiTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the params of iterations.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    JsParseAnimatorParams(ITERATIONS_INFINITY_TEST, iterations, animationDoubleParams, animationStringParams);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringParams.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringParams.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API)->second;
    auto from = animationDoubleParams.find(DOM_ANIMATION_BEGIN)->second;
    auto to = animationDoubleParams.find(DOM_ANIMATION_END)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_INFINITY, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_FROM_DEFAULT, from);
    EXPECT_EQ(ANIMATION_TO_DEFAULT, to);
    EXPECT_EQ(ANIMATION_EASING_DEFAULT, easing);
    EXPECT_EQ(ANIMATION_FILL_DEFAULT, fill);
}

/**
 * @tc.name: AnimatorApiTest005
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimatorApiTest, AnimatorApiTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the params of iterations.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    JsParseAnimatorParams(ITERATIONS_STRING_TEST, iterations, animationDoubleParams, animationStringParams);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringParams.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringParams.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API)->second;
    auto from = animationDoubleParams.find(DOM_ANIMATION_BEGIN)->second;
    auto to = animationDoubleParams.find(DOM_ANIMATION_END)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_DEFAULT, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_FROM_DEFAULT, from);
    EXPECT_EQ(ANIMATION_TO_DEFAULT, to);
    EXPECT_EQ(ANIMATION_EASING_DEFAULT, easing);
    EXPECT_EQ(ANIMATION_FILL_DEFAULT, fill);
}

/**
 * @tc.name: AnimatorApiTest006
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimatorApiTest, AnimatorApiTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the params of fillmode.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleParams;
    std::unordered_map<std::string, std::string> animationStringParams;
    JsParseAnimatorParams(FILLMODE_STRING_TEST, iterations, animationDoubleParams, animationStringParams);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringParams.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringParams.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleParams.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleParams.find(DOM_ANIMATION_DELAY_API)->second;
    auto from = animationDoubleParams.find(DOM_ANIMATION_BEGIN)->second;
    auto to = animationDoubleParams.find(DOM_ANIMATION_END)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_DEFAULT, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_FROM_DEFAULT, from);
    EXPECT_EQ(ANIMATION_TO_DEFAULT, to);
    EXPECT_EQ(ANIMATION_EASING_DEFAULT, easing);
    EXPECT_EQ(ANIMATION_FILL_NONE, fill);
}

} // namespace OHOS::Ace::Framework
