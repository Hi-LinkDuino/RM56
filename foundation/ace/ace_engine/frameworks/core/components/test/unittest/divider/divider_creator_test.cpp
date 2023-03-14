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
#include "core/components/divider/divider_component.h"
#include "core/components/test/json/component_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Dimension DEFAULT_STROKE_WIDTH = Dimension(1.0);
constexpr bool DEFAULT_VERTICAL = false;
const Color DEFAULT_COLOR = Color(0xFF000000);
const Dimension STROKE_WIDTH = Dimension(1.0);
constexpr bool DIVIDER_DIRECTION = true;
const Color DEFAULT_DIVIDER_COLOR = Color(0xffff0000);

} // namespace

class DividerCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DividerCreatorTest::SetUpTestCase() {}
void DividerCreatorTest::TearDownTestCase() {}
void DividerCreatorTest::SetUp() {}
void DividerCreatorTest::TearDown() {}

/**
 * @tc.name: DividerCreator001
 * @tc.desc: Verify DividerCreator can create divider component with content.
 * @tc.type: FUNC
 */
HWTEST_F(DividerCreatorTest, DividerCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with content.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Divider\"   "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get divider component.
     * @tc.expected: step2. all properties are set as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    auto divider = AceType::DynamicCast<DividerComponent>(component);
    ASSERT_TRUE(divider);
    EXPECT_EQ(divider->GetStrokeWidth(), DEFAULT_STROKE_WIDTH);
    EXPECT_EQ(divider->IsVertical(), DEFAULT_VERTICAL);
    EXPECT_EQ(divider->GetDividerColor(), DEFAULT_COLOR);
}

/**
 * @tc.name: DividerCreator002
 * @tc.desc: Verify DividerCreator can create divider component with content.
 * @tc.type: FUNC
 */
HWTEST_F(DividerCreatorTest, DividerCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with content.
     */
    const std::string json = ""
                             "{"
                             " \"className\": \"Divider\", "
                             " \"strokeWidth\": 1,        "
                             " \"vertical\": true,         "
                             " \"color\": {                "
                             " \"className\": \"Color\",   "
                             " \"red\": 255,               "
                             " \"green\": 0,               "
                             " \"blue\": 0                 "
                             "   }                         "
                             " } ";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get divider component.
     * @tc.expected: step2. properties are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    auto component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    auto divider = AceType::DynamicCast<DividerComponent>(component);
    ASSERT_TRUE(divider);
    EXPECT_EQ(divider->GetStrokeWidth(), STROKE_WIDTH);
    EXPECT_EQ(divider->IsVertical(), DIVIDER_DIRECTION);
    EXPECT_EQ(divider->GetDividerColor(), DEFAULT_DIVIDER_COLOR);
}

} // namespace OHOS::Ace