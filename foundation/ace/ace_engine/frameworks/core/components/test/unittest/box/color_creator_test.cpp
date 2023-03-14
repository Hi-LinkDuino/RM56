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
#include "core/components/common/properties/color.h"
#include "core/components/test/json/color_creator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const uint32_t COLOR_DEFAULT = 0xff000000;
const uint32_t COLOR_VALUE = 255;
const uint32_t COLOR_RED = 0xffff0000;
const uint32_t COLOR_GREEN = 0xff00ff00;
const uint32_t COLOR_BLUE = 0xff0000ff;

} // namespace

class ColorCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    Color CreateFromDsl(const std::string jsonColorStr) const;
};

void ColorCreatorTest::SetUpTestCase() {}
void ColorCreatorTest::TearDownTestCase() {}
void ColorCreatorTest::SetUp() {}
void ColorCreatorTest::TearDown() {}

Color ColorCreatorTest::CreateFromDsl(const std::string jsonColorStr) const
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(jsonColorStr.c_str());
    size_t dslLen = jsonColorStr.length();
    std::string jsonStr(reinterpret_cast<const char*>(dslBuf), dslLen);
    std::unique_ptr<JsonValue> colorJson = JsonUtil::ParseJsonString(jsonStr);
    return ColorCreator::CreateFromJson(*colorJson);
}

/**
 * @tc.name: ColorCreator001
 * @tc.desc: Verify ColorCreator can create color with value.
 * @tc.type: FUNC
 */
HWTEST_F(ColorCreatorTest, ColorCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of color with value.
     */
    const std::string jsonColorStr = ""
                                     "{                              "
                                     "  \"className\": \"Color\",    "
                                     "  \"value\": 255               "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get color.
     * @tc.expected: step2. properties are set correctly.
     */
    Color color = CreateFromDsl(jsonColorStr);
    EXPECT_EQ(color.GetValue(), COLOR_VALUE);
}

/**
 * @tc.name: ColorCreator002
 * @tc.desc: Verify ColorCreator can create color with ARGB.
 * @tc.type: FUNC
 */
HWTEST_F(ColorCreatorTest, ColorCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of color with ARGB.
     */
    const std::string jsonColorStr = ""
                                     "{                              "
                                     "  \"className\": \"Color\",    "
                                     "  \"alpha\": 255,              "
                                     "  \"red\": 255,                "
                                     "  \"green\": 0,                "
                                     "  \"blue\": 0                  "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get color.
     * @tc.expected: step2. properties are set correctly.
     */
    Color color = CreateFromDsl(jsonColorStr);
    EXPECT_EQ(color.GetValue(), COLOR_RED);
}

/**
 * @tc.name: ColorCreator003
 * @tc.desc: Verify ColorCreator can create color with RGBO.
 * @tc.type: FUNC
 */
HWTEST_F(ColorCreatorTest, ColorCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of color with RGBO.
     */
    const std::string jsonColorStr = ""
                                     "{                              "
                                     "  \"className\": \"Color\",    "
                                     "  \"red\": 0,                  "
                                     "  \"green\": 0,                "
                                     "  \"blue\": 255,               "
                                     "  \"opacity\": 1.0             "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get color.
     * @tc.expected: step2. properties are set correctly.
     */
    Color color = CreateFromDsl(jsonColorStr);
    EXPECT_EQ(color.GetValue(), COLOR_BLUE);
}

/**
 * @tc.name: ColorCreator004
 * @tc.desc: Verify ColorCreator can create color with one of RGB.
 * @tc.type: FUNC
 */
HWTEST_F(ColorCreatorTest, ColorCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of color with RGB.
     */
    const std::string jsonColorStr = ""
                                     "{                              "
                                     "  \"className\": \"Color\",    "
                                     "  \"red\": 0,                  "
                                     "  \"green\": 255,              "
                                     "  \"blue\": 0                  "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get color.
     * @tc.expected: step2. properties are set correctly.
     */
    Color color = CreateFromDsl(jsonColorStr);
    EXPECT_EQ(color.GetValue(), COLOR_GREEN);
}

/**
 * @tc.name: ColorCreator005
 * @tc.desc: Verify ColorCreator can create color with className only.
 * @tc.type: FUNC
 */
HWTEST_F(ColorCreatorTest, ColorCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of color with className only.
     */
    const std::string jsonColorStr = ""
                                     "{                             "
                                     "  \"className\": \"Color\"    "
                                     "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get color.
     * @tc.expected: step2. properties are set as default.
     */
    Color color = CreateFromDsl(jsonColorStr);
    EXPECT_EQ(color.GetValue(), COLOR_DEFAULT);
}

} // namespace OHOS::Ace