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

#include "base/geometry/dimension.h"
#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/edge.h"
#include "core/components/test/json/edge_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Dimension EDGE_DEFAULT = Dimension(0.0);
const Dimension EDGE_COMMON_VALUE = Dimension(5.0);
const Dimension EDGE_LEFT_VALUE = Dimension(1.0);
const Dimension EDGE_TOP_VALUE = Dimension(2.0);
const Dimension EDGE_RIGHT_VALUE = Dimension(3.0);
const Dimension EDGE_BOTTOM_VALUE = Dimension(4.0);

} // namespace

class EdgeCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    Edge CreateFromDsl(const std::string jsonEdgeStr) const;
};

void EdgeCreatorTest::SetUpTestCase() {}
void EdgeCreatorTest::TearDownTestCase() {}
void EdgeCreatorTest::SetUp() {}
void EdgeCreatorTest::TearDown() {}

Edge EdgeCreatorTest::CreateFromDsl(const std::string jsonEdgeStr) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonEdgeStr.c_str());
    size_t dslLen = jsonEdgeStr.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> edgeJson = JsonUtil::ParseJsonString(jsonStr);
    return EdgeCreator::CreateFromJson(*edgeJson);
}

/**
 * @tc.name: EdgeCreator001
 * @tc.desc: Verify EdgeCreator can create edge with value.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUF
 */
HWTEST_F(EdgeCreatorTest, EdgeCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of edge with value.
     */
    const std::string jsonEdgeStr = ""
                                    "{                                 "
                                    "  \"className\": \"Edge\",        "
                                    "  \"value\": 5.0                  "
                                    "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get edge.
     * @tc.expected: step2. properties are set correctly.
     */
    Edge edge = CreateFromDsl(jsonEdgeStr);
    EXPECT_EQ(edge.Left(), EDGE_COMMON_VALUE);
    EXPECT_EQ(edge.Top(), EDGE_COMMON_VALUE);
    EXPECT_EQ(edge.Right(), EDGE_COMMON_VALUE);
    EXPECT_EQ(edge.Bottom(), EDGE_COMMON_VALUE);
}

/**
 * @tc.name: EdgeCreator002
 * @tc.desc: Verify EdgeCreator can create edge with left | top | right | bottom.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUF
 */
HWTEST_F(EdgeCreatorTest, EdgeCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of edge with left | top | right | bottom.
     */
    const std::string jsonEdgeStr = ""
                                    "{                               "
                                    "  \"className\": \"Edge\",      "
                                    "  \"left\": 1.0,                "
                                    "  \"top\": 2.0,                 "
                                    "  \"right\": 3.0,               "
                                    "  \"bottom\": 4.0               "
                                    "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get edge.
     * @tc.expected: step2. properties are set correctly.
     */
    Edge edge = CreateFromDsl(jsonEdgeStr);
    EXPECT_EQ(edge.Left(), EDGE_LEFT_VALUE);
    EXPECT_EQ(edge.Top(), EDGE_TOP_VALUE);
    EXPECT_EQ(edge.Right(), EDGE_RIGHT_VALUE);
    EXPECT_EQ(edge.Bottom(), EDGE_BOTTOM_VALUE);
}

/**
 * @tc.name: EdgeCreator003
 * @tc.desc: Verify EdgeCreator can create Edge with className only.
 * @tc.type: FUNC
 * @tc.require: AR000DAQUF
 */
HWTEST_F(EdgeCreatorTest, EdgeCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of edge with className only.
     */
    const std::string jsonEdgeStr = ""
                                    "{                              "
                                    "  \"className\": \"Edge\"      "
                                    "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get edge.
     * @tc.expected: step2. properties are set as default.
     */
    Edge edge = CreateFromDsl(jsonEdgeStr);
    EXPECT_EQ(edge.Left(), EDGE_DEFAULT);
    EXPECT_EQ(edge.Top(), EDGE_DEFAULT);
    EXPECT_EQ(edge.Right(), EDGE_DEFAULT);
    EXPECT_EQ(edge.Bottom(), EDGE_DEFAULT);
}

} // namespace OHOS::Ace