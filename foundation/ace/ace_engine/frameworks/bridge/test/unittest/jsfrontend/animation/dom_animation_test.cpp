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

#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/animation_bridge.h"
#include "frameworks/core/animation/curve.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS::Ace::Framework {

namespace {

const int32_t ANIMATION_ITERATIONS_DEFAULT = 1;
const double ANIMATION_DURATIONS_DEFAULT = 3000;
const double ANIMATION_DELAY_DEFAULT = 1000;
const std::string TRANSFORM_ORIGIN_DEFAULT_FROM = "left top";
const std::string TRANSFORM_ORIGIN_DEFAULT_TO = "right center";
const std::string TRANSFORM_SCALE_DEFAULT_FROM = "1";
const std::string TRANSFORM_TRANSLATE_DEFAULT_FROM = "50";
const std::string TRANSFORM_ROTATE_DEFAULT_FROM = "0";
const std::string TRANSFORM_SCALE_DEFAULT_TO = "3";
const std::string TRANSFORM_TRANSLATE_DEFAULT_TO = "100";
const std::string TRANSFORM_ROTATE_DEFAULT_TO = "360";
const std::string ANIMATION_HEIGHT_DEFAULT = "150";
const std::string ANIMATION_OFFSET_FROM = "0.0";
const std::string ANIMATION_OFFSET_MIDDLE = "50.0";
const std::string ANIMATION_OFFSET_TO = "100.0";
const std::string ANIMATION_WIDTH_DEFAULT = "200";
const std::string ANIMATION_OPACITY_DEFAULT = "1";
const std::string ANIMATION_COLOR_DEFAULT = "#ff0000";
const std::string ANIMATION_EASING_DEFAULT = "linear";
const std::string ANIMATION_FILL_DEFAULT = "forwards";
const std::string ANIMATION_EASING_ANTICIPATE = "anticipate";
const std::string ANIMATION_EASING_CUBIC = "cubic-bezier(0.0, 0.0, 0.0, 1.0)";

const std::string TRANSFORM_ORIGIN_TEST = ""
                                          "[                                                    "
                                          "    [                                                "
                                          "        {                                            "
                                          "            \"transformOrigin\": \"left top\"        "
                                          "        },                                           "
                                          "        {                                            "
                                          "            \"transformOrigin\": \"right center\"    "
                                          "        }                                            "
                                          "    ]                                                "
                                          "]";

const std::string TRANSFORM_TEST = ""
                                   "[                                        "
                                   "    [                                    "
                                   "        {                                "
                                   "            \"transform\": {             "
                                   "                \"translate\": \"50\",   "
                                   "                \"rotate\": \"0\"    ,   "
                                   "                \"scale\": \"1\"         "
                                   "            }                            "
                                   "        },                               "
                                   "        {                                "
                                   "            \"transform\": {             "
                                   "                \"translate\": \"100\",  "
                                   "                \"rotate\": \"360\"   ,  "
                                   "                \"scale\" : \"3\"        "
                                   "            }                            "
                                   "        }                                "
                                   "    ]                                    "
                                   "]";

const std::string HEIGHT_WIDTH_TEST = ""
                                      "[                                   "
                                      "    [                               "
                                      "        {                           "
                                      "            \"height\": \"150\",    "
                                      "            \"width\": \"200\"      "
                                      "        },                          "
                                      "        {                           "
                                      "            \"height\": \"150\",    "
                                      "            \"width\": \"200\"      "
                                      "        }                           "
                                      "    ]                               "
                                      "]";

const std::string OPACITY_COLOR_TEST = ""
                                       "[                                               "
                                       "    [                                           "
                                       "        {                                       "
                                       "            \"opacity\": \"1\",                 "
                                       "            \"backgroundColor\": \"#ff0000\"    "
                                       "        },                                      "
                                       "        {                                       "
                                       "            \"opacity\": \"1\",                 "
                                       "            \"backgroundColor\": \"#ff0000\"    "
                                       "        }                                       "
                                       "    ]                                           "
                                       "]";

const std::string OPTIONS_TEST = ""
                                 "[                                       "
                                 "    [                                   "
                                 "        {                               "
                                 "        },                              "
                                 "        {                               "
                                 "        }                               "
                                 "    ],                                  "
                                 "    {                                   "
                                 "        \"duration\": 3000,             "
                                 "        \"easing\": \"linear\",         "
                                 "        \"delay\": 1000,                "
                                 "        \"fill\": \"forwards\",         "
                                 "        \"iterations\": 1,              "
                                 "        \"direction\": \"alternate\"    "
                                 "    }                                   "
                                 "]";

const std::string ANTICIPATE_CURVE_TEST = ""
                                          "[                                  "
                                          "    [                              "
                                          "        {                          "
                                          "        },                         "
                                          "        {                          "
                                          "        }                          "
                                          "    ],                             "
                                          "    {                              "
                                          "        \"duration\": 3000,        "
                                          "        \"easing\": \"anticipate\","
                                          "        \"delay\": 1000,           "
                                          "        \"fill\": \"forwards\",    "
                                          "        \"iterations\": 1          "
                                          "    }                              "
                                          "]";

const std::string CUBIC_CURVE_TEST = ""
                                     "[                                                        "
                                     "    [                                                    "
                                     "        {                                                "
                                     "        },                                               "
                                     "        {                                                "
                                     "        }                                                "
                                     "    ],                                                   "
                                     "    {                                                    "
                                     "        \"duration\": 3000,                              "
                                     "        \"easing\": \"cubic-bezier(0.0, 0.0, 0.0, 1.0)\","
                                     "        \"delay\": 1000,                                 "
                                     "        \"fill\": \"forwards\",                          "
                                     "        \"iterations\": 1                                "
                                     "    }                                                    "
                                     "]";

const std::string NO_OFFSET_TEST = ""
                                   "[                                   "
                                   "    [                               "
                                   "        {                           "
                                   "            \"height\": \"150\"     "
                                   "        },                          "
                                   "        {                           "
                                   "            \"height\": \"150\"     "
                                   "        }                           "
                                   "    ]                               "
                                   "]";

const std::string MULTI_ANIMATION_TEST = ""
                                         "[                                   "
                                         "    [                               "
                                         "        {                           "
                                         "            \"height\": \"150\",    "
                                         "            \"opacity\": \"1\"      "
                                         "        },                          "
                                         "        {                           "
                                         "            \"height\": \"150\",    "
                                         "            \"opacity\": \"1\",     "
                                         "            \"offset\": \"0.5\"     "
                                         "        },                          "
                                         "        {                           "
                                         "            \"height\": \"150\",    "
                                         "            \"opacity\": \"1\"      "
                                         "        }                           "
                                         "    ]                               "
                                         "]";

const std::string SERIAL_ANIMATION_TEST = ""
                                          "[                                   "
                                          "    [                               "
                                          "        {                           "
                                          "            \"height\": \"150\",    "
                                          "            \"offset\": \"0.0\"     "
                                          "        },                          "
                                          "        {                           "
                                          "            \"height\": \"150\",    "
                                          "            \"offset\": \"0.5\"     "
                                          "        },                          "
                                          "        {                           "
                                          "            \"height\": \"150\",    "
                                          "            \"offset\": \"1.0\"     "
                                          "        }                           "
                                          "    ]                               "
                                          "]";

const std::string SERIAL_TRANSFORM_TEST = ""
                                          "[                                        "
                                          "    [                                    "
                                          "        {                                "
                                          "            \"transform\": {             "
                                          "                \"translate\": \"50\",   "
                                          "                \"scale\": \"1\"         "
                                          "            },                           "
                                          "            \"offset\": \"0.0\"          "
                                          "        },                               "
                                          "        {                                "
                                          "            \"transform\": {             "
                                          "                \"translate\": \"100\",  "
                                          "                \"scale\" : \"3\"        "
                                          "            },                           "
                                          "            \"offset\": \"1.0\"          "
                                          "        }                                "
                                          "    ]                                    "
                                          "]";

const std::string ERROR_OFFSET_TEST = ""
                                      "[                                   "
                                      "    [                               "
                                      "        {                           "
                                      "            \"height\": \"150\"     "
                                      "        },                          "
                                      "        {                           "
                                      "            \"height\": \"150\",    "
                                      "            \"offset\": \"0.5\"     "
                                      "        },                          "
                                      "        {                           "
                                      "            \"height\": \"150\",    "
                                      "            \"offset\": \"0.7\"     "
                                      "        }                           "
                                      "    ]                               "
                                      "]";

} // namespace

class AnimationDomTest : public testing::Test, public BaseAnimationBridgeUtils {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void AnimationDomTest::SetUpTestCase() {}
void AnimationDomTest::TearDownTestCase() {}
void AnimationDomTest::SetUp() {}
void AnimationDomTest::TearDown() {}

/**
 * @tc.name: AnimationDomTest001
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of transformOrigin.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(TRANSFORM_ORIGIN_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the transformOrigin from the map.
     */
    auto transformOriginForm = animationFrames[0].find(DOM_TRANSFORM_ORIGIN)->second;
    auto transformOriginTo = animationFrames[1].find(DOM_TRANSFORM_ORIGIN)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of transformOrigin is as expected.
     */
    EXPECT_EQ(TRANSFORM_ORIGIN_DEFAULT_FROM, transformOriginForm);
    EXPECT_EQ(TRANSFORM_ORIGIN_DEFAULT_TO, transformOriginTo);
}

/**
 * @tc.name: AnimationDomTest002
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of transform.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(TRANSFORM_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the transform from the map.
     */

    EXPECT_TRUE(animationFrames.size() == 2);
    auto transformFrom = animationFrames[0].find(DOM_TRANSFORM);
    auto transformTo = animationFrames[1].find(DOM_TRANSFORM);

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of transform is as expected.
     */
    EXPECT_TRUE(transformFrom != animationFrames[0].end());
    EXPECT_TRUE(transformTo != animationFrames[1].end());
}

/**
 * @tc.name: AnimationDomTest003
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of height and width.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(HEIGHT_WIDTH_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto heightForm = animationFrames[0].find(DOM_ANIMATION_HEIGHT)->second;
    auto widthForm = animationFrames[0].find(DOM_ANIMATION_WIDTH)->second;
    auto heightTo = animationFrames[1].find(DOM_ANIMATION_HEIGHT)->second;
    auto widthTo = animationFrames[1].find(DOM_ANIMATION_WIDTH)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightForm);
    EXPECT_EQ(ANIMATION_WIDTH_DEFAULT, widthForm);
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightTo);
    EXPECT_EQ(ANIMATION_WIDTH_DEFAULT, widthTo);
}

/**
 * @tc.name: AnimationDomTest004
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of opacity and color.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(OPACITY_COLOR_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto opacityForm = animationFrames[0].find(DOM_ANIMATION_OPACITY)->second;
    auto colorForm = animationFrames[0].find(DOM_ANIMATION_COLOR)->second;
    auto opacityTo = animationFrames[1].find(DOM_ANIMATION_OPACITY)->second;
    auto colorTo = animationFrames[1].find(DOM_ANIMATION_COLOR)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_OPACITY_DEFAULT, opacityForm);
    EXPECT_EQ(ANIMATION_COLOR_DEFAULT, colorForm);
    EXPECT_EQ(ANIMATION_OPACITY_DEFAULT, opacityTo);
    EXPECT_EQ(ANIMATION_COLOR_DEFAULT, colorTo);
}

/**
 * @tc.name: AnimationDomTest005
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of options.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleOptions;
    std::unordered_map<std::string, std::string> animationStringOptions;
    JsParseAnimationOptions(OPTIONS_TEST, iterations, animationDoubleOptions, animationStringOptions);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringOptions.find(DOM_ANIMATION_EASING)->second;
    auto fill = animationStringOptions.find(DOM_ANIMATION_FILL)->second;
    auto duration = animationDoubleOptions.find(DOM_ANIMATION_DURATION_API)->second;
    auto delay = animationDoubleOptions.find(DOM_ANIMATION_DELAY_API)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_ITERATIONS_DEFAULT, iterations);
    EXPECT_EQ(ANIMATION_DURATIONS_DEFAULT, duration);
    EXPECT_EQ(ANIMATION_DELAY_DEFAULT, delay);
    EXPECT_EQ(ANIMATION_EASING_DEFAULT, easing);
    EXPECT_EQ(ANIMATION_FILL_DEFAULT, fill);
}

/**
 * @tc.name: AnimationDomTest006
 * @tc.desc: Serial animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(NO_OFFSET_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto heightForm = animationFrames[0].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetFrom = animationFrames[0].find(DOM_ANIMATION_OFFSET)->second;
    auto heightTo = animationFrames[1].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetTo = animationFrames[1].find(DOM_ANIMATION_OFFSET)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightForm);

    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_FROM), StringToDouble(offsetFrom)));
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightTo);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_TO), StringToDouble(offsetTo)));
}

/**
 * @tc.name: AnimationDomTest007
 * @tc.desc: Serial animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(MULTI_ANIMATION_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto heightForm = animationFrames[0].find(DOM_ANIMATION_HEIGHT)->second;
    auto opacityFrom = animationFrames[0].find(DOM_ANIMATION_OPACITY)->second;
    auto offsetFrom = animationFrames[0].find(DOM_ANIMATION_OFFSET)->second;
    auto heightMiddle = animationFrames[1].find(DOM_ANIMATION_HEIGHT)->second;
    auto opacityMiddle = animationFrames[1].find(DOM_ANIMATION_OPACITY)->second;
    auto offsetMiddle = animationFrames[1].find(DOM_ANIMATION_OFFSET)->second;
    auto heightTo = animationFrames[2].find(DOM_ANIMATION_HEIGHT)->second;
    auto opacityTo = animationFrames[2].find(DOM_ANIMATION_OPACITY)->second;
    auto offsetTo = animationFrames[2].find(DOM_ANIMATION_OFFSET)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightForm);
    EXPECT_EQ(ANIMATION_OPACITY_DEFAULT, opacityFrom);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_FROM), StringToDouble(offsetFrom)));
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightMiddle);
    EXPECT_EQ(ANIMATION_OPACITY_DEFAULT, opacityMiddle);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_MIDDLE), StringToDouble(offsetMiddle)));
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightTo);
    EXPECT_EQ(ANIMATION_OPACITY_DEFAULT, opacityTo);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_TO), StringToDouble(offsetTo)));
}

/**
 * @tc.name: AnimationDomTest008
 * @tc.desc: Serial animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(SERIAL_ANIMATION_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto heightForm = animationFrames[0].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetFrom = animationFrames[0].find(DOM_ANIMATION_OFFSET)->second;
    auto heightMiddle = animationFrames[1].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetMiddle = animationFrames[1].find(DOM_ANIMATION_OFFSET)->second;
    auto heightTo = animationFrames[2].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetTo = animationFrames[2].find(DOM_ANIMATION_OFFSET)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightForm);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_FROM), StringToDouble(offsetFrom)));
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightMiddle);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_MIDDLE), StringToDouble(offsetMiddle)));
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightTo);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_TO), StringToDouble(offsetTo)));
}

/**
 * @tc.name: AnimationDomTest009
 * @tc.desc: Serial animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(HEIGHT_WIDTH_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto heightForm = animationFrames[0].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetFrom = animationFrames[0].find(DOM_ANIMATION_OFFSET)->second;
    auto heightTo = animationFrames[1].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetTo = animationFrames[1].find(DOM_ANIMATION_OFFSET)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightForm);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_FROM), StringToDouble(offsetFrom)));
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightTo);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_TO), StringToDouble(offsetTo)));
}

/**
 * @tc.name: AnimationDomTest010
 * @tc.desc: Serial animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(SERIAL_TRANSFORM_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    EXPECT_TRUE(animationFrames.size() == 2);
    auto offsetFrom = animationFrames[0].find(DOM_ANIMATION_OFFSET);
    EXPECT_TRUE(offsetFrom != animationFrames[0].end());
    auto offsetTo = animationFrames[1].find(DOM_ANIMATION_OFFSET);
    EXPECT_TRUE(offsetTo != animationFrames[1].end());

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(StringToDouble(offsetFrom->second), StringToDouble(ANIMATION_OFFSET_FROM));
    EXPECT_EQ(StringToDouble(offsetTo->second), StringToDouble(ANIMATION_OFFSET_TO));
}

/**
 * @tc.name: AnimationDomTest011
 * @tc.desc: Serial animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     */
    std::vector<std::unordered_map<std::string, std::string>> animationFrames;
    JsParseAnimationFrames(ERROR_OFFSET_TEST, animationFrames);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto heightForm = animationFrames[0].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetFrom = animationFrames[0].find(DOM_ANIMATION_OFFSET)->second;

    auto heightMiddle = animationFrames[1].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetMiddle = animationFrames[1].find(DOM_ANIMATION_OFFSET)->second;

    auto heightTo = animationFrames[2].find(DOM_ANIMATION_HEIGHT)->second;
    auto offsetTo = animationFrames[2].find(DOM_ANIMATION_OFFSET)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightForm);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_FROM), StringToDouble(offsetFrom)));

    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightMiddle);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_MIDDLE), StringToDouble(offsetMiddle)));

    EXPECT_EQ(ANIMATION_HEIGHT_DEFAULT, heightTo);
    EXPECT_TRUE(NearEqual(StringToDouble(ANIMATION_OFFSET_TO), StringToDouble(offsetTo)));
}

/**
 * @tc.name: AnimationDomTest012
 * @tc.desc: backgroundPosition parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     * @tc.expected: step2. The value of the params are as expected.
     */
    BackgroundImagePosition backgroundImagePosition;
    EXPECT_TRUE(ParseBackgroundImagePosition("10% 20%", backgroundImagePosition));
    EXPECT_EQ(10, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(20, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());
}

/**
 * @tc.name: AnimationDomTest013
 * @tc.desc: backgroundPosition parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param.
     * @tc.expected: step2. The value of the params are as expected.
     */
    BackgroundImagePosition backgroundImagePosition;
    EXPECT_TRUE(ParseBackgroundImagePosition("10%", backgroundImagePosition));
    EXPECT_EQ(10, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(50, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step3. parse the param.
     * @tc.expected: step4. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("10% 100px", backgroundImagePosition));
    EXPECT_EQ(10, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PX, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step5. parse the param.
     * @tc.expected: step6. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition(" 30px   45% ", backgroundImagePosition));
    EXPECT_EQ(30, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PX, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(45, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());
}

/**
 * @tc.name: AnimationDomTest014
 * @tc.desc: backgroundPosition parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest014, TestSize.Level1)
{
    BackgroundImagePosition backgroundImagePosition;
    /**
     * @tc.steps: step1. parse the param.
     * @tc.expected: step2. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("left", backgroundImagePosition));
    EXPECT_EQ(0, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(50, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step3. parse the param.
     * @tc.expected: step4. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("bottom", backgroundImagePosition));
    EXPECT_EQ(50, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step5. parse the param.
     * @tc.expected: step6. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("right bottom", backgroundImagePosition));
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step7. parse the param.
     * @tc.expected: step8. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("bottom right", backgroundImagePosition));
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step9. parse the param.
     * @tc.expected: step10. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("center 50px", backgroundImagePosition));
    EXPECT_EQ(50, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(50, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PX, backgroundImagePosition.GetSizeTypeY());

    /**
     * @tc.steps: step11. parse the param.
     * @tc.expected: step12. The value of the params are as expected.
     */
    EXPECT_TRUE(ParseBackgroundImagePosition("70px bottom", backgroundImagePosition));
    EXPECT_EQ(70, backgroundImagePosition.GetSizeValueX());
    EXPECT_EQ(BackgroundImagePositionType::PX, backgroundImagePosition.GetSizeTypeX());
    EXPECT_EQ(100, backgroundImagePosition.GetSizeValueY());
    EXPECT_EQ(BackgroundImagePositionType::PERCENT, backgroundImagePosition.GetSizeTypeY());
}

/**
 * @tc.name: AnimationDomTest015
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomTest015, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of options.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleOptions;
    std::unordered_map<std::string, std::string> animationStringOptions;
    JsParseAnimationOptions(OPTIONS_TEST, iterations, animationDoubleOptions, animationStringOptions);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto direction = animationStringOptions.find(DOM_ANIMATION_DIRECTION_API)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(DOM_ANIMATION_DIRECTION_ALTERNATE, direction);
}

/**
 * @tc.name: AnimationDomCurveTest002
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomCurveTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of anticipate curve.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleOptions;
    std::unordered_map<std::string, std::string> animationStringOptions;
    JsParseAnimationOptions(ANTICIPATE_CURVE_TEST, iterations, animationDoubleOptions, animationStringOptions);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringOptions.find(DOM_ANIMATION_EASING)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_EASING_ANTICIPATE, easing);
}

/**
 * @tc.name: AnimationDomCurveTest003
 * @tc.desc: Animation JS api parse
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomCurveTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of cubic curve.
     */
    int32_t iterations = 0;
    std::unordered_map<std::string, double> animationDoubleOptions;
    std::unordered_map<std::string, std::string> animationStringOptions;
    JsParseAnimationOptions(CUBIC_CURVE_TEST, iterations, animationDoubleOptions, animationStringOptions);

    /**
     * @tc.steps: step2. get the params from the map.
     */
    auto easing = animationStringOptions.find(DOM_ANIMATION_EASING)->second;

    /**
     * @tc.steps: step3. Judge whether the parameter resolution is successful.
     * @tc.expected: step3. The value of the params are as expected.
     */
    EXPECT_EQ(ANIMATION_EASING_CUBIC, easing);
}

/**
 * @tc.name: AnimationDomCurveTest004
 * @tc.desc: Animation steps parse check
 * @tc.type: FUNC
 */
HWTEST_F(AnimationDomTest, AnimationDomCurveTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. parse the param of steps curve.
     * @tc.expected: step1. steps curve parsed success.
     */
    auto curve = AceType::DynamicCast<StepsCurve>(CreateCurve("steps(2, start)"));
    EXPECT_EQ(curve->MoveInternal(0.0), 0.5);
}

} // namespace OHOS::Ace::Framework
