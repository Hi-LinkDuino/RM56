/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "core/components/rating/rating_component.h"
#include "core/components/test/json/rating_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

RefPtr<RatingComponent> CreateRatingComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<RatingComponent>(component);
}

void CheckRatingDefaultValue(const RefPtr<RatingComponent>& rating)
{
    EXPECT_TRUE(rating);
    EXPECT_EQ(rating->GetStarNum(), DEFAULT_RATING_STAR_NUM);
    EXPECT_EQ(rating->GetRatingScore(), DEFAULT_RATING_SCORE);
    EXPECT_EQ(rating->GetStepSize(), DEFAULT_RATING_STEP_SIZE);
    EXPECT_EQ(rating->GetWidth().Value(), DEFAULT_RATING_WIDTH);
    EXPECT_EQ(rating->GetHeight().Value(), DEFAULT_RATING_HEIGHT);
    EXPECT_EQ(rating->GetPaddingHorizontal().Value(), DEFAULT_RATING_HORIZONTAL_PADDING);
    EXPECT_EQ(rating->GetPaddingVertical().Value(), DEFAULT_RATING_VERTICAL_PADDING);
    EXPECT_EQ(rating->GetForegroundSrc(), "");
    EXPECT_EQ(rating->GetSecondarySrc(), "");
    EXPECT_EQ(rating->GetBackgroundSrc(), "");
    EXPECT_TRUE(!rating->GetIndicator());
    EXPECT_TRUE(rating->GetChangeEventId().IsEmpty());
}

} // namespace

class RatingCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void RatingCreatorTest::SetUpTestCase() {}

void RatingCreatorTest::TearDownTestCase() {}

void RatingCreatorTest::SetUp() {}

void RatingCreatorTest::TearDown() {}

/**
 * @tc.name: RatingCreator001
 * @tc.desc: Test parse rating's field successfully from the right fields dsl json of rating component.
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB AR000DBSLC AR000DBSLD AR000DBSLE
 * @tc.author: kouxinxin
 */
HWTEST_F(RatingCreatorTest, RatingCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with right fields, then create rating component with the json.
     * @tc.expected: step1. Rating component successfully created, and the fields value same with the json fields.
     */
    std::string dslJson = "{                                                   "
                          "  \"className\": \"Rating\",                        "
                          "  \"onChange\": \"changeId001\",                    "
                          "  \"starNum\": 1,                                   "
                          "  \"ratingScore\": 1,                               "
                          "  \"stepsize\": 0.1,                                "
                          "  \"width\": 500,                                   "
                          "  \"height\": 80,                                   "
                          "  \"paddingHorizontal\": 20,                        "
                          "  \"paddingVertical\": 20,                          "
                          "  \"isIndicator\": 1,                               "
                          "  \"foregroundSrc\": \"/rating/foreground.img\",    "
                          "  \"secondarySrc\": \"/rating/secondary.img\",      "
                          "  \"backgroundSrc\": \"/rating/background.img\"     "
                          "}                                            ";

    RefPtr<RatingComponent> rating = CreateRatingComponent(dslJson);
    EXPECT_TRUE(rating);
    EXPECT_EQ(rating->GetChangeEventId(), "changeId001");
    EXPECT_EQ(rating->GetStarNum(), 1);
    EXPECT_EQ(rating->GetRatingScore(), 1);
    EXPECT_EQ(rating->GetStepSize(), 0.1);
    EXPECT_EQ(rating->GetWidth().Value(), 500);
    EXPECT_EQ(rating->GetHeight().Value(), 80);
    EXPECT_EQ(rating->GetPaddingHorizontal().Value(), 20);
    EXPECT_EQ(rating->GetPaddingVertical().Value(), 20);
    EXPECT_EQ(rating->GetForegroundSrc(), "/rating/foreground.img");
    EXPECT_EQ(rating->GetSecondarySrc(), "/rating/secondary.img");
    EXPECT_EQ(rating->GetBackgroundSrc(), "/rating/background.img");
    EXPECT_TRUE(rating->GetIndicator());
}

/**
 * @tc.name: RatingCreator002
 * @tc.desc: Test parse rating's field successfully from no field dsl json of rating component.
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB AR000DBSLC AR000DBSLD AR000DBSLE
 * @tc.author: kouxinxin
 */
HWTEST_F(RatingCreatorTest, RatingCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with no fields, then create rating component with the json.
     * @tc.expected: step1. Rating component successfully created, and the fields has default values.
     */
    std::string dslJson = "{ \"className\": \"Rating\" }";
    RefPtr<RatingComponent> rating = CreateRatingComponent(dslJson);
    CheckRatingDefaultValue(rating);
}

/**
 * @tc.name: RatingCreator003
 * @tc.desc: Test parse rating's field successfully from the type error fields dsl json of rating component.
 * @tc.type: FUNC
 * @tc.require: AR000DBSLB AR000DBSLC AR000DBSLD AR000DBSLE
 * @tc.author: kouxinxin
 */
HWTEST_F(RatingCreatorTest, RatingCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with type error fields, then create rating component with the json.
     * @tc.expected: step1. Rating component successfully created, and the fields has default values.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Rating\",                 "
                          "  \"onChange\": 1,                           "
                          "  \"numstars\": \"1\",                       "
                          "  \"ratingScore\": \"1\",                    "
                          "  \"rating\": \"3\",                         "
                          "  \"stepsize\": \"0.1\",                     "
                          "  \"width\": \"500\",                        "
                          "  \"height\": \"80\",                        "
                          "  \"paddingHorizontal\": \"20\",             "
                          "  \"paddingVertical\": \"20\",               "
                          "  \"isIndicator\": \"true\",                 "
                          "  \"foregroundSrc\": 1,                      "
                          "  \"secondarySrc\": 1,                       "
                          "  \"backgroundSrc\": 1                       "
                          "}                                            ";
    RefPtr<RatingComponent> rating = CreateRatingComponent(dslJson);
    CheckRatingDefaultValue(rating);
}

} // namespace OHOS::Ace