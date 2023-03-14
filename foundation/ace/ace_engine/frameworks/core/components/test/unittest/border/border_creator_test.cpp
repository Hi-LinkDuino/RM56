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
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/radius.h"
#include "core/components/test/json/border_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const BorderEdge BORDER_EDGE_DEFAULT = BorderEdge(Color(0xff000000), Dimension(), BorderStyle::NONE);
const BorderEdge BORDER_EDGE_VALUE =
    BorderEdge(Color(0xffff0000), Dimension(5.0, DimensionUnit::PX), BorderStyle::SOLID);
const BorderEdge BORDER_EDGE_LEFT = BorderEdge(Color(100), Dimension(5.0, DimensionUnit::PX), BorderStyle::SOLID);
const BorderEdge BORDER_EDGE_TOP = BorderEdge(Color(200), Dimension(10.0, DimensionUnit::PX), BorderStyle::SOLID);
const BorderEdge BORDER_EDGE_RIGHT = BorderEdge(Color(300), Dimension(15.0, DimensionUnit::PX), BorderStyle::DOTTED);
const BorderEdge BORDER_EDGE_BOTTOM = BorderEdge(Color(400), Dimension(20.0, DimensionUnit::PX), BorderStyle::DASHED);
const Radius BORDER_RADIUS_DEFAULT = Radius(0.0);
const Radius BORDER_RADIUS_NORMAL = Radius(50.0);
const Radius BORDER_RADIUS_TOPLEFT = Radius(10.0);
const Radius BORDER_RADIUS_TOPRIGHT = Radius(20.0);
const Radius BORDER_RADIUS_BOTTOMLEFT = Radius(30.0);
const Radius BORDER_RADIUS_BOTTOMRIGHT = Radius(40.0);

} // namespace

class BorderCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<Border> CreateFromDsl(const std::string jsonBorderStr) const;
};

void BorderCreatorTest::SetUpTestCase() {}
void BorderCreatorTest::TearDownTestCase() {}
void BorderCreatorTest::SetUp() {}
void BorderCreatorTest::TearDown() {}

std::unique_ptr<Border> BorderCreatorTest::CreateFromDsl(const std::string jsonBorderStr) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonBorderStr.c_str());
    size_t dslLen = jsonBorderStr.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> borderJson = JsonUtil::ParseJsonString(jsonStr);
    return BorderCreator::CreateFromJson(*borderJson);
}

/**
 * @tc.name: BoxCreator001
 * @tc.desc: Verify BorderCreator can create a border with four identical edges.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BoxCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct a border with four identical edges.
     */
    const std::string content = ""
                                "{                                       "
                                "  \"className\": \"Border\",            "
                                "  \"value\": {                          "
                                "    \"className\": \"BorderEdge\",      "
                                "    \"width\":5.0,                      "
                                "    \"color\": {                        "
                                "      \"className\": \"Color\",         "
                                "      \"alpha\": 255,                   "
                                "      \"red\": 255,                     "
                                "      \"green\": 0,                     "
                                "      \"blue\": 0                       "
                                "    },                                  "
                                "    \"style\": \"solid\"                "
                                "  }                                     "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. border properties are set correctly, radius are set as default..
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_DEFAULT);
}

/**
 * @tc.name: BorderCreator002
 * @tc.desc: Verify that borderCreator can independently create four borders: up, down, left, and right.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BorderCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border with four borders: up, down, left, and right.
     */
    const std::string content = ""
                                "{                                                            "
                                "  \"className\": \"Border\",                                 "
                                "  \"left\": {                                                "
                                "    \"className\": \"BorderEdge\", \"width\": 5.0,           "
                                "    \"color\": { \"className\": \"Color\", \"value\": 100 }, "
                                "    \"style\": \"solid\" },                                  "
                                "  \"top\": {                                                 "
                                "    \"className\": \"BorderEdge\", \"width\": 10.0,          "
                                "    \"color\": { \"className\": \"Color\", \"value\": 200 }, "
                                "    \"style\": \"solid\" },                                  "
                                "  \"right\": {                                               "
                                "    \"className\": \"BorderEdge\", \"width\": 15.0,          "
                                "    \"color\": { \"className\": \"Color\", \"value\": 300 }, "
                                "    \"style\": \"dotted\" },                                 "
                                "  \"bottom\": {                                              "
                                "    \"className\": \"BorderEdge\", \"width\": 20.0,          "
                                "    \"color\": { \"className\": \"Color\", \"value\": 400 }, "
                                "    \"style\": \"dashed\" }                                  "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. border are set correctly, radius are set as default.
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_LEFT);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_TOP);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_RIGHT);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_BOTTOM);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_DEFAULT);
}

/**
 * @tc.name: BorderCreator003
 * @tc.desc: Verify BorderCreator can create border with left.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BorderCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border with left.
     */
    const std::string content = ""
                                "{                                                            "
                                "  \"className\": \"Border\",                                 "
                                "  \"left\": {                                                "
                                "    \"className\": \"BorderEdge\", \"width\": 5.0,           "
                                "    \"color\": { \"className\": \"Color\", \"value\": 100 }, "
                                "    \"style\": \"solid\" }                                   "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. left border edge are set correctly, other three edges and radius are set as default.
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_LEFT);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_DEFAULT);
}

/**
 * @tc.name: BorderCreator004
 * @tc.desc: Verify BorderCreator can create border with className only.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BorderCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border with className only.
     */
    const std::string content = ""
                                "{                              "
                                "  \"className\": \"Border\"    "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. border edges and radius are set as default.
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_DEFAULT);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_DEFAULT);
}

/**
 * @tc.name: BorderCreator005
 * @tc.desc: Verify BorderCreator can create border with value and radius.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BorderCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border with value and four radius.
     */
    const std::string content = ""
                                "{                                                            "
                                "  \"className\": \"Border\",                                 "
                                "  \"value\": {                                               "
                                "    \"className\": \"BorderEdge\", \"width\": 5.0,           "
                                "    \"color\": {                                             "
                                "      \"className\": \"Color\",                              "
                                "      \"alpha\": 255,                                        "
                                "      \"red\": 255,                                          "
                                "      \"green\": 0,                                          "
                                "      \"blue\": 0                                            "
                                "    },                                                       "
                                "    \"style\": \"solid\" },                                  "
                                "  \"radius\": { \"className\": \"Radius\", \"value\": 50.0 } "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. border edges are set as default, radius are set correctly.
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_NORMAL);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_NORMAL);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_NORMAL);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_NORMAL);
}

/**
 * @tc.name: BorderCreator006
 * @tc.desc: Verify BorderCreator can create border with value and four radius:topLeft and topRight and bottomLeft and
 * bottomRight.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BorderCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border with value and four radius:topLeft and topRight and
     * bottomLeft and bottomRight.
     */
    const std::string content = ""
                                "{                                                                        "
                                "  \"className\": \"Border\",                                             "
                                "  \"value\": {                                                           "
                                "    \"className\": \"BorderEdge\", \"width\": 5.0,                       "
                                "    \"color\": {                                                         "
                                "      \"className\": \"Color\",                                          "
                                "      \"alpha\": 255,                                                    "
                                "      \"red\": 255,                                                      "
                                "      \"green\": 0,                                                      "
                                "      \"blue\": 0                                                        "
                                "    },                                                                   "
                                "    \"style\": \"solid\" },                                              "
                                "  \"topLeftRadius\": { \"className\": \"Radius\", \"value\": 10.0 },     "
                                "  \"topRightRadius\": { \"className\": \"Radius\", \"value\": 20.0 },    "
                                "  \"bottomLeftRadius\": { \"className\": \"Radius\", \"value\": 30.0 },  "
                                "  \"bottomRightRadius\": { \"className\": \"Radius\", \"value\": 40.0 }  "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. border edges are set as default, radius are set correctly.
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_TOPLEFT);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_TOPRIGHT);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_BOTTOMLEFT);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_BOTTOMRIGHT);
}

/**
 * @tc.name: BorderCreator007
 * @tc.desc: Verify BorderCreator can create border with value and topLeft radius.
 * @tc.type: FUNC
 */
HWTEST_F(BorderCreatorTest, BorderCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border with value and topLeft radius.
     */
    const std::string content = ""
                                "{                                                                       "
                                "  \"className\": \"Border\",                                            "
                                "  \"value\": {                                                          "
                                "    \"className\": \"BorderEdge\", \"width\": 5.0,                      "
                                "    \"color\": {                                                        "
                                "      \"className\": \"Color\",                                         "
                                "      \"alpha\": 255,                                                   "
                                "      \"red\": 255,                                                     "
                                "      \"green\": 0,                                                     "
                                "      \"blue\": 0                                                       "
                                "    },                                                                  "
                                "    \"style\": \"solid\" },                                             "
                                "  \"topLeftRadius\": { \"className\": \"Radius\", \"value\": 10.0 }     "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border.
     * @tc.expected: step2. topLeft radius are set correct border edges and other three radius are set as default
     */
    std::unique_ptr<Border> border = CreateFromDsl(content);
    ASSERT_TRUE(border->Left() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Top() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Right() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->Bottom() == BORDER_EDGE_VALUE);
    ASSERT_TRUE(border->TopLeftRadius() == BORDER_RADIUS_TOPLEFT);
    ASSERT_TRUE(border->TopRightRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomLeftRadius() == BORDER_RADIUS_DEFAULT);
    ASSERT_TRUE(border->BottomRightRadius() == BORDER_RADIUS_DEFAULT);
}

} // namespace OHOS::Ace
