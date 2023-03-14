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

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/flex/flex_component.h"
#include "core/components/root/root_component.h"
#include "core/components/stack/stack_component.h"
#include "core/components/test/json/flex_creator.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t SINGLE_CHILD_COUNT = 1;
constexpr int32_t MULTI_CHILD_COUNT = 2;
const std::string TEST_TEXT_VALUE = "Hi Ace";

} // namespace

class StackCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    RefPtr<Component> CreateComponent(const std::string& testJson);
};

void StackCreatorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "StackCreatorTest SetUpTestCase";
}

void StackCreatorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "StackCreatorTest TearDownTestCase";
}

void StackCreatorTest::SetUp() {}

void StackCreatorTest::TearDown() {}

RefPtr<Component> StackCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    return component;
}

/**
 * @tc.name: StackProperties001
 * @tc.desc: Check stack component with no properties in the json returns default properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(StackCreatorTest, StackProperties001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string where stack has no properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Stack\",       "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get column component
     * @tc.expected: step2. properties are set as default
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<StackComponent> stack = AceType::DynamicCast<StackComponent>(component);
    ASSERT_TRUE(stack != nullptr);
    Alignment align = stack->GetAlignment();
    EXPECT_TRUE(NearEqual(align.GetVertical(), -1.0));
    EXPECT_TRUE(NearEqual(align.GetHorizontal(), -1.0));
    int32_t stackFit = static_cast<int32_t>(stack->GetStackFit());
    int32_t overflowFlag = static_cast<int32_t>(stack->GetOverflow());
    std::list<RefPtr<Component>> children = stack->GetChildren();
    EXPECT_TRUE(stackFit == 0);
    EXPECT_TRUE(overflowFlag == 0);
    EXPECT_TRUE(children.size() == SINGLE_CHILD_COUNT);
}

/**
 * @tc.name: StackProperties002
 * @tc.desc: Check stack component with invalid properties in the json returns default properties
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(StackCreatorTest, StackProperties002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string where stack has invalid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Stack\",       "
                           "  \"stackFit\": -1.5,             "
                           "  \"overflow\": -1,               "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get row component
     * @tc.expected: step2. properties are set as default
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<StackComponent> stack = AceType::DynamicCast<StackComponent>(component);
    ASSERT_TRUE(stack != nullptr);
    int32_t overflowFlag = static_cast<int32_t>(stack->GetOverflow());
    int32_t stackFit = static_cast<int32_t>(stack->GetStackFit());
    std::list<RefPtr<Component>> children = stack->GetChildren();
    EXPECT_TRUE(overflowFlag == 0);
    EXPECT_TRUE(stackFit == 0);
    EXPECT_TRUE(children.size() == SINGLE_CHILD_COUNT);
}

/**
 * @tc.name: StackProperties003
 * @tc.desc: Check stack component with valid properties in the json returns correct properties
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(StackCreatorTest, StackProperties003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string where stack has valid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Stack\",       "
                           "  \"overflow\": 1,                "
                           "  \"stackFit\": 1,                "
                           "  \"child\": [{                   "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          },"
                           "          {"
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }]                      "
                           "}";
    /**
     * @tc.steps: step2. get row component
     * @tc.expected: step2. properties are set as default
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<StackComponent> stack = AceType::DynamicCast<StackComponent>(component);
    ASSERT_TRUE(stack != nullptr);
    int32_t overflow = static_cast<int32_t>(stack->GetOverflow());
    int32_t stackFit = static_cast<int32_t>(stack->GetStackFit());
    std::list<RefPtr<Component>> children = stack->GetChildren();
    EXPECT_TRUE(overflow == 1);
    EXPECT_TRUE(stackFit == 1);
    EXPECT_TRUE(children.size() == MULTI_CHILD_COUNT);
}

/**
 * @tc.name: StackChild001
 * @tc.desc: Verify stack component with single flex child is correct when load the json
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(StackCreatorTest, StackChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json with one child
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Stack\",       "
                           "  \"stackFit\": 1,           "
                           "  \"overflow\": 0,          "
                           "  \"child\": {                    "
                           "    \"className\": \"Row\",       "
                           "    \"child\": {"
                           "    \"className\": \"Text\",      "
                           "    \"value\": \"Hi Ace\"         "
                           "               }                  "
                           "   }                              "
                           "}";
    /**
     * @tc.steps: step2. get row component with one child
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<StackComponent> stack = AceType::DynamicCast<StackComponent>(component);
    ASSERT_TRUE(stack != nullptr);
    int32_t stackFit = static_cast<int32_t>(stack->GetStackFit());
    int32_t overflow = static_cast<int32_t>(stack->GetOverflow());
    int32_t childrenCount = stack->GetChildren().size();
    EXPECT_TRUE(childrenCount == SINGLE_CHILD_COUNT);
    EXPECT_TRUE(stackFit == 1);
    EXPECT_TRUE(overflow == 0);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(stack->GetChildren().front());
    ASSERT_TRUE(row != nullptr);
    ASSERT_TRUE(row->GetChildren().size() == SINGLE_CHILD_COUNT);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(row->GetChildren().front());
    ASSERT_TRUE(text != nullptr);
    EXPECT_TRUE(text->GetData() == TEST_TEXT_VALUE);
}

/**
 * @tc.name: StackChild002
 * @tc.desc: Verify stack component with two child is correct when load the json
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(StackCreatorTest, StackChild002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json with two child
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Stack\",         "
                           "  \"stackFit\": 1,           "
                           "  \"overflow\": 1,          "
                           "  \"child\": ["
                           "    {                             "
                           "     \"className\": \"Column\",   "
                           "      \"child\": {"
                           "      \"className\": \"Text\",    "
                           "      \"value\": \"Hi Ace\"       "
                           "               }"
                           "   },                             "
                           "   {                              "
                           "     \"className\": \"Column\",    "
                           "     \"child\": {"
                           "     \"className\": \"Text\",      "
                           "     \"value\": \"Hi Ace\"         "
                           "               }"
                           "   }                              "
                           "   ]"
                           "}";
    /**
     * @tc.steps: step2. get stack component with two children
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<StackComponent> stack = AceType::DynamicCast<StackComponent>(component);
    ASSERT_TRUE(stack != nullptr);
    int32_t stackFit = static_cast<int32_t>(stack->GetStackFit());
    int32_t overflow = static_cast<int32_t>(stack->GetOverflow());
    int32_t childrenCount = stack->GetChildren().size();
    EXPECT_TRUE(childrenCount == MULTI_CHILD_COUNT);
    EXPECT_TRUE(stackFit == 1);
    EXPECT_TRUE(overflow == 1);
    RefPtr<ColumnComponent> firstChild = AceType::DynamicCast<ColumnComponent>(stack->GetChildren().front());
    ASSERT_TRUE(firstChild != nullptr);
    RefPtr<TextComponent> firstChildText = AceType::DynamicCast<TextComponent>(firstChild->GetChildren().front());
    ASSERT_TRUE(firstChildText != nullptr);
    EXPECT_TRUE(firstChildText->GetData() == TEST_TEXT_VALUE);
    RefPtr<ColumnComponent> secondChild = AceType::DynamicCast<ColumnComponent>(stack->GetChildren().back());
    RefPtr<TextComponent> secondChildText = AceType::DynamicCast<TextComponent>(secondChild->GetChildren().front());
    ASSERT_TRUE(secondChildText != nullptr);
    EXPECT_TRUE(secondChildText->GetData() == TEST_TEXT_VALUE);
}

/**
 * @tc.name: StackChild003
 * @tc.desc: Verify stack component with no child is correct when load the json
 * @tc.type: FUNC
 * @tc.require: AR000DAQTN AR000DAQTO AR000DAQTP
 * @tc.author: yangfan
 */
HWTEST_F(StackCreatorTest, StackChild003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json with no child
     */
    std::string testJson = ""
                           "{                            "
                           "  \"className\": \"Stack\",  "
                           "  \"stackFit\": 1,           "
                           "  \"overflow\": 1            "
                           "}";
    /**
     * @tc.steps: step2. get row component with no child
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<StackComponent> stack = AceType::DynamicCast<StackComponent>(component);
    ASSERT_TRUE(stack != nullptr);
    int32_t stackFit = static_cast<int32_t>(stack->GetStackFit());
    int32_t overflow = static_cast<int32_t>(stack->GetOverflow());
    std::list<RefPtr<Component>> children = stack->GetChildren();
    EXPECT_TRUE(stackFit == 1);
    EXPECT_TRUE(overflow == 1);
    EXPECT_TRUE(children.size() == 0);
}

} // namespace OHOS::Ace