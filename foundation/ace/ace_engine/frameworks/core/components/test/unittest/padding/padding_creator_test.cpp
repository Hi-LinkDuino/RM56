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
#include "core/components/common/properties/edge.h"
#include "core/components/padding/padding_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/test/json/padding_creator.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Edge EDGE_DEFAULT_VALUE = Edge(0.0);
const Edge EDGE_COMMON_VALUE = Edge(5.0);
const Dimension EDGE_LEFT_VALUE = Dimension(1.0);
const Dimension EDGE_TOP_VALUE = Dimension(2.0);
const Dimension EDGE_RIGHT_VALUE = Dimension(3.0);
const Dimension EDGE_BOTTOM_VALUE = Dimension(4.0);

const std::string TEXT_VALUE = "Hi Ace";

} // namespace

class PaddingCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PaddingCreatorTest::SetUpTestCase() {}
void PaddingCreatorTest::TearDownTestCase() {}
void PaddingCreatorTest::SetUp() {}
void PaddingCreatorTest::TearDown() {}

/**
 * @tc.name: PaddingCreator001
 * @tc.desc: Verify PaddingCreator can create padding component with padding and child.
 * @tc.type: FUNC
 */
HWTEST_F(PaddingCreatorTest, PaddingCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of padding component with padding and child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Padding\",    "
                                "  \"padding\": {                 "
                                "    \"className\": \"Edge\",     "
                                "    \"value\": 5.0               "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get padding component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(component);
    ASSERT_TRUE(padding != nullptr);
    ASSERT_TRUE(padding->GetPadding() == EDGE_COMMON_VALUE);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(padding->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
}

/**
 * @tc.name: PaddingCreator002
 * @tc.desc: Verify PaddingCreator can create padding component with className only.
 * @tc.type: FUNC
 */
HWTEST_F(PaddingCreatorTest, PaddingCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of padding component with className only.
     */
    const std::string content = ""
                                "{                               "
                                "  \"className\": \"Padding\"    "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get padding component.
     * @tc.expected: step2. properties are set as default, child is nullptr.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(component);
    ASSERT_TRUE(padding != nullptr);
    ASSERT_TRUE(padding->GetPadding() == EDGE_DEFAULT_VALUE);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(padding->GetChild());
    ASSERT_TRUE(text == nullptr);
}

/**
 * @tc.name: PaddingCreator003
 * @tc.desc: Verify PaddingCreator can create padding component with directions params only
 * @tc.type: FUNC
 */
HWTEST_F(PaddingCreatorTest, PaddingCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of padding component with padding and child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Padding\",    "
                                "  \"padding\": {                 "
                                "    \"className\": \"Edge\",     "
                                "    \"left\" : 1.0,              "
                                "    \"right\" : 3.0,             "
                                "    \"top\" : 2.0,               "
                                "    \"bottom\" : 4.0            "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get padding component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(component);
    ASSERT_TRUE(padding != nullptr);
    ASSERT_TRUE(padding->GetPadding().Left() == EDGE_LEFT_VALUE);
    ASSERT_TRUE(padding->GetPadding().Right() == EDGE_RIGHT_VALUE);
    ASSERT_TRUE(padding->GetPadding().Top() == EDGE_TOP_VALUE);
    ASSERT_TRUE(padding->GetPadding().Bottom() == EDGE_BOTTOM_VALUE);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(padding->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
}

/**
 * @tc.name: PaddingCreator004
 * @tc.desc: Verify PaddingCreator can create padding component with value param and other directions params
 * @tc.type: FUNC
 */
HWTEST_F(PaddingCreatorTest, PaddingCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of padding component with padding and child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Padding\",    "
                                "  \"padding\": {                 "
                                "    \"className\": \"Edge\",     "
                                "    \"value\": 5.0,              "
                                "    \"left\" : 1.0,              "
                                "    \"right\" : 3.0,             "
                                "    \"top\" : 2.0,               "
                                "    \"bottom\" : 4.0            "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get padding component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(component);
    ASSERT_TRUE(padding != nullptr);
    ASSERT_TRUE(padding->GetPadding() == EDGE_COMMON_VALUE);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(padding->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
}

/**
 * @tc.name: PaddingCreator005
 * @tc.desc: Verify PaddingCreator can create padding component with directions param negative
 * @tc.type: FUNC
 */
HWTEST_F(PaddingCreatorTest, PaddingCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of padding component with padding and child.
     */
    const std::string content = ""
                                "{                                "
                                "  \"className\": \"Padding\",    "
                                "  \"padding\": {                 "
                                "    \"className\": \"Edge\",     "
                                "    \"left\" : 1.0,              "
                                "    \"right\" : 3.0,             "
                                "    \"bottom\" : -4.0            "
                                "  },                             "
                                "  \"child\": {                   "
                                "    \"className\": \"Text\",     "
                                "    \"value\": \"Hi Ace\"        "
                                "  }                              "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get padding component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PaddingComponent> padding = AceType::DynamicCast<PaddingComponent>(component);
    ASSERT_TRUE(padding != nullptr);
    ASSERT_TRUE(NearZero(padding->GetPadding().Top().Value()));
    ASSERT_TRUE(NearZero(padding->GetPadding().Bottom().Value()));
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(padding->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
}

} // namespace OHOS::Ace