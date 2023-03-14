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

#include "base/geometry/dimension.h"
#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/radius.h"
#include "core/components/test/json/radius_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Dimension RADIUS_DEFAULT = Dimension(0.0);
const Dimension RADIUS_VALUE = Dimension(5.0);
const Dimension RADIUS_X = Dimension(1.0);
const Dimension RADIUS_Y = Dimension(2.0);

} // namespace

class RadiusCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    Radius CreateFromDsl(const std::string jsonRadiusStr) const;
};

void RadiusCreatorTest::SetUpTestCase() {}
void RadiusCreatorTest::TearDownTestCase() {}
void RadiusCreatorTest::SetUp() {}
void RadiusCreatorTest::TearDown() {}

Radius RadiusCreatorTest::CreateFromDsl(const std::string jsonRadiusStr) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonRadiusStr.c_str());
    size_t dslLen = jsonRadiusStr.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> radiusJson = JsonUtil::ParseJsonString(jsonStr);
    return RadiusCreator::CreateFromJson(*radiusJson);
}

/**
 * @tc.name: RadiusCreator001
 * @tc.desc: Verify RadiusCreator can create radius with value.
 * @tc.type: FUNC
 */
HWTEST_F(RadiusCreatorTest, RadiusCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of radius with value.
     */
    const std::string jsonRadiusStr = ""
                                      "{                               "
                                      "  \"className\": \"Radius\",    "
                                      "  \"value\": 5.0                "
                                      "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get radius.
     * @tc.expected: step2. properties are set correctly.
     */
    Radius radius = CreateFromDsl(jsonRadiusStr);
    EXPECT_EQ(radius.GetX(), RADIUS_VALUE);
    EXPECT_EQ(radius.GetY(), RADIUS_VALUE);
}

/**
 * @tc.name: RadiusCreator002
 * @tc.desc: Verify RadiusCreator can create radius with x | y.
 * @tc.type: FUNC
 */
HWTEST_F(RadiusCreatorTest, RadiusCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of radius with x | y.
     */
    const std::string jsonRadiusStr = ""
                                      "{                               "
                                      "  \"className\": \"Radius\",    "
                                      "    \"x\": 1.0,                 "
                                      "    \"y\": 2.0                  "
                                      "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get radius.
     * @tc.expected: step2. properties are set correctly.
     */
    Radius radius = CreateFromDsl(jsonRadiusStr);
    EXPECT_EQ(radius.GetX(), RADIUS_X);
    EXPECT_EQ(radius.GetY(), RADIUS_Y);
}

/**
 * @tc.name: RadiusCreator003
 * @tc.desc: Verify RadiusCreator can create radius with className only.
 * @tc.type: FUNC
 */
HWTEST_F(RadiusCreatorTest, RadiusCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of radius with className only.
     */
    const std::string jsonRadiusStr = ""
                                      "{                               "
                                      "  \"className\": \"Radius\"     "
                                      "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get radius.
     * @tc.expected: step2. properties are set as default.
     */
    Radius radius = CreateFromDsl(jsonRadiusStr);
    EXPECT_EQ(radius.GetX(), RADIUS_DEFAULT);
    EXPECT_EQ(radius.GetY(), RADIUS_DEFAULT);
}

} // namespace OHOS::Ace
