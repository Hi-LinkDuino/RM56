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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/common/properties/shadow.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/test/json/decoration_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Color COLOR_DEFAULT = Color(0x00000000);
const Offset OFFSET_DEFAULT = Offset(0.0, 0.0);
const Color DECORATION_COLOR_VALUE = Color(0xffff0000);
const double BLUR_RADIUS_VALUE = 10.0;
const Offset OFFSET_VALUE = Offset(10.0, 10.0);
const Border DECORATION_BORDER_VALUE =
    Border(BorderEdge(Color(0xffff0000), Dimension(10.0, DimensionUnit::PX), BorderStyle::SOLID));
const Border DECORATION_BORDER_DEFAULT = Border(BorderEdge(Color(0xff000000), Dimension(), BorderStyle::NONE));
const std::string DECORATION_IMAGE_SRC = "/data/000.jpg";
const double TEST_GRADIENT_ANGLE = 350.0;
const bool TEST_GRADIENT_REPEAT = false;

} // namespace

class DecorationCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    RefPtr<Decoration> CreateFromDsl(const std::string content) const;
};

void DecorationCreatorTest::SetUpTestCase() {}
void DecorationCreatorTest::TearDownTestCase() {}
void DecorationCreatorTest::SetUp() {}
void DecorationCreatorTest::TearDown() {}

RefPtr<Decoration> DecorationCreatorTest::CreateFromDsl(const std::string content) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> decorationJson = JsonUtil::ParseJsonString(jsonStr);
    return DecorationCreator::CreateFromJson(*decorationJson);
}

/**
 * @tc.name: DecorationCreator001
 * @tc.desc: Verify DecorationCreator can create a decoration with shadows.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with shadows properties.
     */
    const std::string content = ""
                                "{                                                                               "
                                "    \"className\": \"Decoration\",                                              "
                                "    \"shadowList\":[                                                            "
                                "        {                                                                       "
                                "            \"color\": {\"className\": \"Color\",\"alpha\": 255,\"red\": 255},  "
                                "            \"blurRadius\": 10.0,                                               "
                                "            \"offset\":{\"dx\":10.0,\"dy\":10.0}                                "
                                "        }                                                                       "
                                "    ]                                                                           "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    auto decoration = CreateFromDsl(content);
    if (!decoration->GetShadows().empty()) {
        Shadow shadow = decoration->GetShadows().front();
        ASSERT_TRUE(shadow.GetColor() == DECORATION_COLOR_VALUE);
        ASSERT_TRUE(shadow.GetOffset() == OFFSET_VALUE);
        ASSERT_TRUE(shadow.GetBlurRadius() == BLUR_RADIUS_VALUE);
    }
}

/**
 * @tc.name: DecorationCreator002
 * @tc.desc: Verify DecorationCreator can create a decoration with only empty shadowList.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only empty shadowList.
     */
    const std::string content = ""
                                "{                                   "
                                "    \"className\": \"Decoration\",  "
                                "    \"shadowList\":[                "
                                "    ]                               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetShadows().empty());
}

/**
 * @tc.name: DecorationCreator003
 * @tc.desc: Verify DecorationCreator can create a decoration with only color property.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only color property.
     */
    const std::string content = ""
                                "{                                   "
                                "    \"className\": \"Decoration\",  "
                                "    \"color\": {                    "
                                "        \"className\": \"Color\",   "
                                "        \"alpha\":255,              "
                                "        \"red\":255                 "
                                "    }                               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetBackgroundColor() == DECORATION_COLOR_VALUE);
}

/**
 * @tc.name: DecorationCreator004
 * @tc.desc: Verify DecorationCreator can create a decoration with only gradient property.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only gradient property.
     */
    const std::string content = ""
                                "{                                             "
                                "    \"className\": \"Decoration\",            "
                                "    \"gradient\": {                           "
                                "        \"className\": \"Gradient\",          "
                                "        \"angle\": 350,                       "
                                "        \"repeat\":false                      "
                                "    }                                         "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetGradient().GetLinearGradient().angle.value().Value() == TEST_GRADIENT_ANGLE);
    ASSERT_TRUE(decoration->GetGradient().GetRepeat() == TEST_GRADIENT_REPEAT);
}

/**
 * @tc.name: DecorationCreator005
 * @tc.desc: Verify DecorationCreator can create a decoration with only image property.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only image property.
     */
    const std::string content = ""
                                "{                                             "
                                "    \"className\": \"Decoration\",            "
                                "    \"backgroundImage\": {                    "
                                "        \"className\": \"BackgroundImage\",   "
                                "        \"src\":\"/data/000.jpg\",            "
                                "        \"imageRepeat\":0                     "
                                "    }                                         "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetImage()->GetImageRepeat() == ImageRepeat::REPEAT);
    ASSERT_TRUE(decoration->GetImage()->GetSrc() == DECORATION_IMAGE_SRC);
}

/**
 * @tc.name: DecorationCreator006
 * @tc.desc: Verify DecorationCreator can create a decoration with only border property.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only border property.
     */
    const std::string content = ""
                                "{                                       "
                                "    \"className\": \"Decoration\",      "
                                "    \"border\": {                       "
                                "      \"className\": \"Border\",        "
                                "      \"value\": {                      "
                                "        \"className\": \"BorderEdge\",  "
                                "        \"width\":10.0,                 "
                                "        \"color\": {                    "
                                "          \"className\": \"Color\",     "
                                "          \"alpha\": 255,               "
                                "          \"red\": 255                  "
                                "        },                              "
                                "        \"style\": \"solid\"            "
                                "      }                                 "
                                "    }                                   "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetBorder() == DECORATION_BORDER_VALUE);
}

/**
 * @tc.name: DecorationCreator007
 * @tc.desc: Verify DecorationCreator can create a decoration with only className.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only className.
     */
    const std::string content = ""
                                "{                                   "
                                "    \"className\": \"Decoration\"   "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set as default.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetShadows().empty());
    ASSERT_TRUE(decoration->GetBackgroundColor() == COLOR_DEFAULT);
    ASSERT_TRUE(decoration->GetBorder() == DECORATION_BORDER_DEFAULT);
}

/**
 * @tc.name: DecorationCreator008
 * @tc.desc: Verify DecorationCreator can create a decoration with only border property.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only border className.
     */
    const std::string content = ""
                                "{                                       "
                                "    \"className\": \"Decoration\",      "
                                "    \"border\": {                       "
                                "      \"className\": \"Border\"         "
                                "      }                                 "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetBorder() == DECORATION_BORDER_DEFAULT);
}

/**
 * @tc.name: DecorationCreator009
 * @tc.desc: Verify DecorationCreator can create a decoration with only color property.
 * @tc.type: FUNC
 */
HWTEST_F(DecorationCreatorTest, DecorationCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of decoration with only color className.
     */
    const std::string content = ""
                                "{                                   "
                                "    \"className\": \"Decoration\",  "
                                "    \"color\": {                    "
                                "        \"className\": \"Color\"    "
                                "    }                               "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get decoration.
     * @tc.expected: step2. decoration properties are set correctly.
     */
    RefPtr<Decoration> decoration = CreateFromDsl(content);
    ASSERT_TRUE(decoration->GetBackgroundColor() == Color::BLACK);
}

} // namespace OHOS::Ace
