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

#include "core/components/rating/rating_theme.h"
#include "core/pipeline/base/constants.h"
#include "frameworks/bridge/common/dom/dom_rating.h"
#include "frameworks/bridge/test/unittest/jsfrontend/dom_node_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {
namespace {

const std::string FOREGROUND_SRC_VALUE = "/example/foreground";
const std::string SECONDARY_SRC_VALUE = "/example/secondary";
const std::string BACKGROUND_SRC_VALUE = "/example/background";
const double RATING_WIDTH_VALUE = 600.0;
const double RATING_HEIGHT_VALUE = 200.0;
const double RATING_SCORE_VALUE = 2.5;
const double RATING_STEPSIZE_VALUE = 0.5;
const int32_t RATING_STAR_NUM_VALUE = 5;

} // namespace

class DomRatingTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomRatingTest::SetUpTestCase() {}
void DomRatingTest::TearDownTestCase() {}
void DomRatingTest::SetUp() {}
void DomRatingTest::TearDown() {}

/**
 * @tc.name: CreateDOMNodeFromDsl001
 * @tc.desc: Verify that DomRating can create ratingComponent correctly with all attributes set.
 * @tc.type: FUNC
 */
HWTEST_F(DomRatingTest, CreateDOMNodeFromDsl001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomRating with all attributes set.
     */
    const std::string jsonRatingStr = ""
                                      "{                                                                "
                                      "  \"tag\": \"rating\",                                           "
                                      "  \"attr\": [{                                                   "
                                      "                \"numstars\" : \"5\"                             "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"rating\" : \"2.5\"                             "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"stepsize\" : \"0.5\"                           "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"indicator\" : \"true\"                         "
                                      "              }],                                                "
                                      "  \"style\": [{                                                  "
                                      "                \"starBackground\":\"/example/background\"       "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"starSecondary\" : \"/example/secondary\"       "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"starForeground\" : \"/example/foreground\"     "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"height\" : \"200px\"                           "
                                      "              },                                                 "
                                      "              {                                                  "
                                      "                \"width\" : \"600px\"                            "
                                      "          }],                                                    "
                                      "  \"event\": [ \"change\" ]                                      "
                                      "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomRating.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonRatingStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<RatingComponent> ratingComponent = AceType::DynamicCast<RatingComponent>(boxChild->GetChild());

    /**
     * @tc.steps: step3. Verify whether all attributes of ratingComponent are as excepted.
     * @tc.expected: step3. All attributes of ratingComponent are as excepted.
     */
    ASSERT_TRUE(ratingComponent != nullptr);
    ASSERT_TRUE(ratingComponent->GetStarNum() == RATING_STAR_NUM_VALUE);
    ASSERT_TRUE(ratingComponent->GetRatingScore() == RATING_SCORE_VALUE);
    ASSERT_TRUE(ratingComponent->GetStepSize() == RATING_STEPSIZE_VALUE);
    ASSERT_TRUE(ratingComponent->GetIndicator());
    ASSERT_TRUE(ratingComponent->GetForegroundSrc() == FOREGROUND_SRC_VALUE);
    ASSERT_TRUE(ratingComponent->GetSecondarySrc() == SECONDARY_SRC_VALUE);
    ASSERT_TRUE(ratingComponent->GetBackgroundSrc() == BACKGROUND_SRC_VALUE);
    ASSERT_TRUE(boxChild->GetWidthDimension().Value() == RATING_WIDTH_VALUE);
    ASSERT_TRUE(boxChild->GetHeightDimension().Value() == RATING_HEIGHT_VALUE);
    ASSERT_TRUE(ratingComponent->GetChangeEventId() == std::to_string(domNodeRoot->GetNodeId()));
}

/**
 * @tc.name: CreateDOMNodeFromDsl002
 * @tc.desc: Verify that DomRating can create ratingComponent correctly with no attribute set.
 * @tc.type: FUNC
 */
HWTEST_F(DomRatingTest, CreateDOMNodeFromDsl002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. the json string of DomRating with no attribute set.
     */
    const std::string jsonRatingStr = ""
                                      "{                     "
                                      "  \"tag\": \"rating\" "
                                      "}";

    /**
     * @tc.steps: step2. call JsonUtil interface and create DomRating.
     */
    auto domNodeRoot = DOMNodeFactory::GetInstance().CreateDOMNodeFromDsl(jsonRatingStr);
    auto boxChild = DOMNodeFactory::GetInstance().GetBoxChildComponent(domNodeRoot);
    RefPtr<RatingComponent> ratingComponent = AceType::DynamicCast<RatingComponent>(boxChild->GetChild());

    /**
     * @tc.steps: step3. Verify whether all attributes of ratingComponent are as excepted.
     * @tc.expected: step3. All attributes of ratingComponent are as excepted.
     */
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    RefPtr<RatingTheme> theme = themeManager->GetTheme<RatingTheme>();
    ASSERT_TRUE(theme);
    ASSERT_TRUE(ratingComponent != nullptr);
    ASSERT_TRUE(ratingComponent->GetStarNum() == theme->GetStarNum());
    ASSERT_TRUE(ratingComponent->GetRatingScore() == theme->GetRatingScore());
    ASSERT_TRUE(ratingComponent->GetStepSize() == theme->GetStepSize());
    ASSERT_TRUE(!ratingComponent->GetIndicator());
    ASSERT_TRUE(ratingComponent->GetForegroundSrc().empty());
    ASSERT_TRUE(ratingComponent->GetSecondarySrc().empty());
    ASSERT_TRUE(ratingComponent->GetBackgroundSrc().empty());
    ASSERT_TRUE(boxChild->GetWidthDimension().Value() == theme->GetRatingWidth().Value());
    ASSERT_TRUE(boxChild->GetHeightDimension().Value() == theme->GetRatingHeight().Value());
}

} // namespace OHOS::Ace::Framework
