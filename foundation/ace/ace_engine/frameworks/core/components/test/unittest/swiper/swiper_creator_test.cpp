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

#include "core/components/swiper/swiper_component.h"
#include "core/components/test/json/swiper_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

RefPtr<SwiperComponent> CreateSwiperComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<SwiperComponent>(component);
}

void CheckSwiperDefaultValue(const RefPtr<SwiperComponent>& swiper)
{
    EXPECT_TRUE(swiper);
    EXPECT_TRUE(swiper->GetChangeEventId().IsEmpty());
    EXPECT_EQ(swiper->GetIndex(), DEFAULT_SWIPER_CURRENT_INDEX);
    EXPECT_FALSE(swiper->IsAutoPlay());
    EXPECT_EQ(swiper->GetAutoPlayInterval(), DEFAULT_SWIPER_AUTOPLAY_INTERVAL);
    EXPECT_TRUE(swiper->IsLoop());
    EXPECT_EQ(swiper->GetDuration(), DEFAULT_SWIPER_ANIMATION_DURATION);
    auto indicator = swiper->GetIndicator();
    EXPECT_TRUE(indicator);
    EXPECT_EQ(swiper->GetAxis(), Axis::HORIZONTAL);
}

} // namespace

class SwiperCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SwiperCreatorTest::SetUpTestCase() {}

void SwiperCreatorTest::TearDownTestCase() {}

void SwiperCreatorTest::SetUp() {}

void SwiperCreatorTest::TearDown() {}

/**
 * @tc.name: SwiperCreator001
 * @tc.desc: Test parse swiper's field successfully from the right fields dsl json of swiper component.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVD AR000DAQVE AR000DAQVF AR000DAQVH AR000DAQVG AR000DAQVI AR000DAQVJ
 * @tc.author: kouxinxin
 */
HWTEST_F(SwiperCreatorTest, SwiperCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with right fields, then create swiper component with the json.
     * @tc.expected: step1. Swiper component successfully created, and the fields value same with the json fields.
     */
    std::string dslJson = "{                                            "
                          "  \"className\": \"Swiper\",                 "
                          "  \"onChanged\": \"changeId001\",            "
                          "  \"index\": 1,                              "
                          "  \"duration\": 500,                         "
                          "  \"loop\": false,                           "
                          "  \"autoPlay\": true,                        "
                          "  \"autoPlayInterval\": 2000,                "
                          "  \"isVertical\": true,                      "
                          "  \"showIndicator\": true,                   "
                          "  \"indicatorColor\": { \"className\": \"Color\", \"value\": 255 },             "
                          "  \"indicatorSelectedColor\": { \"className\": \"Color\", \"value\": 128 },     "
                          "  \"indicatorSize\": 10,                     "
                          "  \"indicatorSelectedSize\": 15,             "
                          "  \"indicatorTop\": 20,                      "
                          "  \"indicatorLeft\": 20,                     "
                          "  \"indicatorBottom\": 20,                   "
                          "  \"indicatorRight\": 20,                    "
                          "  \"child\": [{                              "
                          "      \"className\":\"Text\",                "
                          "      \"value\":\"Hi Ace\"                   "
                          "   }, {                                      "
                          "      \"className\":\"Text\",                "
                          "      \"value\":\"Hi Ace\"                   "
                          "   }]                                        "
                          "}                                            ";

    RefPtr<SwiperComponent> swiper = CreateSwiperComponent(dslJson);
    EXPECT_TRUE(swiper);
    EXPECT_EQ(swiper->GetChangeEventId(), "changeId001");
    EXPECT_EQ(swiper->GetIndex(), 1U);
    EXPECT_TRUE(swiper->IsAutoPlay());
    EXPECT_EQ(swiper->GetAutoPlayInterval(), 2000.0);
    EXPECT_FALSE(swiper->IsLoop());
    EXPECT_EQ(swiper->GetDuration(), 500.0);
    auto indicator = swiper->GetIndicator();
    EXPECT_TRUE(indicator);
    if (indicator) {
        EXPECT_EQ(indicator->GetSize().Value(), 10.0);
        EXPECT_EQ(indicator->GetSelectedSize().Value(), 15.0);
        EXPECT_EQ(indicator->GetTop().Value(), 20.0);
        EXPECT_EQ(indicator->GetLeft().Value(), 20.0);
        EXPECT_EQ(indicator->GetBottom().Value(), 20.0);
        EXPECT_EQ(indicator->GetRight().Value(), 20.0);
        Color indicatorColor(255U);
        EXPECT_EQ(indicator->GetColor(), indicatorColor);
        Color indicatorSelectedColor(128U);
        EXPECT_EQ(indicator->GetSelectedColor(), indicatorSelectedColor);
    }
    EXPECT_EQ(swiper->GetAxis(), Axis::VERTICAL);
}

/**
 * @tc.name: SwiperCreator002
 * @tc.desc: Test parse swiper's field successfully from no field dsl json of swiper component.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVD AR000DAQVE AR000DAQVF AR000DAQVH AR000DAQVG AR000DAQVI AR000DAQVJ
 * @tc.author: kouxinxin
 */
HWTEST_F(SwiperCreatorTest, SwiperCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with no fields, then create swiper component with the json.
     * @tc.expected: step1. Swiper component successfully created, and the fields has default values.
     */
    std::string dslJson = "{ \"className\": \"Swiper\" }";
    RefPtr<SwiperComponent> swiper = CreateSwiperComponent(dslJson);
    CheckSwiperDefaultValue(swiper);
}

/**
 * @tc.name: SwiperCreator003
 * @tc.desc: Test parse swiper's field successfully from the type error fields dsl json of swiper component.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVD AR000DAQVE AR000DAQVF AR000DAQVH AR000DAQVG AR000DAQVI AR000DAQVJ
 * @tc.author: kouxinxin
 */
HWTEST_F(SwiperCreatorTest, SwiperCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with type error fields, then create swiper component with the json.
     * @tc.expected: step1. Swiper component successfully created, and the fields has default values.
     */
    std::string dslJson = "{                                                "
                          "  \"className\": \"Swiper\",                     "
                          "  \"onChanged\": 1,                              "
                          "  \"index\": \"1\",                              "
                          "  \"duration\": \"500\",                         "
                          "  \"loop\": \"false\",                           "
                          "  \"autoPlay\": \"true\",                        "
                          "  \"autoPlayInterval\": \"2000\",                "
                          "  \"isVertical\": \"true\",                      "
                          "  \"showIndicator\": \"true\",                   "
                          "  \"indicatorColor\": [],                        "
                          "  \"indicatorSelectedColor\": [],                "
                          "  \"indicatorSize\": \"10\",                     "
                          "  \"indicatorSelectedSize\": \"15\",             "
                          "  \"indicatorTop\": \"20\",                      "
                          "  \"indicatorLeft\": \"20\",                     "
                          "  \"indicatorBottom\": \"20\",                   "
                          "  \"indicatorRight\": \"20\",                    "
                          "  \"child\": {}                                  "
                          "}                                                ";
    RefPtr<SwiperComponent> swiper = CreateSwiperComponent(dslJson);
    CheckSwiperDefaultValue(swiper);
}

} // namespace OHOS::Ace