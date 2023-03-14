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
#include "core/components/common/properties/border_edge.h"
#include "core/components/test/json/border_edge_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double BORDER_EDGE_WIDTH_VALUE = 1.0;
const double BORDER_EDGE_WIDTH_DEFAULT = 0.0;
const uint32_t BORDER_EDGE_COLOR_VALUE = 0x000000ff;
const uint32_t BORDER_EDGE_COLOR_DEFAULT = 0xff000000;

}

class BorderEdgeCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    BorderEdge CreateFromDsl(const std::string jsonBorderEdgeStr) const;
};

void BorderEdgeCreatorTest::SetUpTestCase() {}
void BorderEdgeCreatorTest::TearDownTestCase() {}
void BorderEdgeCreatorTest::SetUp() {}
void BorderEdgeCreatorTest::TearDown() {}

BorderEdge BorderEdgeCreatorTest::CreateFromDsl(const std::string jsonBorderEdgeStr) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonBorderEdgeStr.c_str());
    size_t dslLen = jsonBorderEdgeStr.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> borderEdgeJson = JsonUtil::ParseJsonString(jsonStr);
    return BorderEdgeCreator::CreateFromJson(*borderEdgeJson);
}

/**
 * @tc.name: BorderEdgeCreator001
 * @tc.desc: Verify BorderEdgeCreator can create border edge with width | color | style.
 * @tc.type: FUNC
 */
HWTEST_F(BorderEdgeCreatorTest, BorderEdgeCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border edge with width | color | style.
     */
    const std::string jsonBorderEdgeStr = ""
                                          "{                                   "
                                          "  \"className\": \"BorderEdge\",    "
                                          "  \"width\": 1.0,                   "
                                          "  \"color\": {                      "
                                          "     \"className\": \"Color\",      "
                                          "     \"value\": 255                 "
                                          "  },                                "
                                          "  \"style\": \"solid\"              "
                                          "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border edge.
     * @tc.expected: step2. properties are set correctly.
     */
    BorderEdge borderEdge = CreateFromDsl(jsonBorderEdgeStr);
    EXPECT_EQ(borderEdge.GetWidth().Value(), BORDER_EDGE_WIDTH_VALUE);
    EXPECT_EQ(borderEdge.GetColor().GetValue(), BORDER_EDGE_COLOR_VALUE);
    EXPECT_EQ(borderEdge.GetBorderStyle(), BorderStyle::SOLID);
}

/**
 * @tc.name: BorderEdgeCreator002
 * @tc.desc: Verify BorderEdgeCreator can create border edge with className only.
 * @tc.type: FUNC
 */
HWTEST_F(BorderEdgeCreatorTest, BorderEdgeCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of border edge with className only.
     */
    const std::string jsonBorderEdgeStr = ""
                                          "{                                   "
                                          "  \"className\": \"BorderEdge\"     "
                                          "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get border edge.
     * @tc.expected: step2. properties are set as default.
     */
    BorderEdge borderEdge = CreateFromDsl(jsonBorderEdgeStr);
    EXPECT_EQ(borderEdge.GetWidth().Value(), BORDER_EDGE_WIDTH_DEFAULT);
    EXPECT_EQ(borderEdge.GetColor().GetValue(), BORDER_EDGE_COLOR_DEFAULT);
    EXPECT_EQ(borderEdge.GetBorderStyle(), BorderStyle::NONE);
}

} // namespace OHOS::Ace