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
#include "core/components/flex/flex_item_component.h"
#include "core/components/root/root_component.h"
#include "core/components/test/json/flex_creator.h"
#include "core/components/test/json/flex_item_creator.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double DEFAULT_FLEX_SHRINK = 0;
constexpr double DEFAULT_FLEX_GROW = 0;
constexpr double DEFAULT_FLEX_BASIS = 0.0;
constexpr double TEST_FLEX_SHRINK = 2;
constexpr double TEST_FLEX_GROW = 1;
constexpr double TEST_FLEX_BASIS = 20.0;
const std::string TEST_TEXT_VALUE = "Hi Ace";

} // namespace

class FlexItemCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<Component> CreateComponent(const std::string& testJson);
    void CheckItemProperties(const RefPtr<FlexItemComponent>& flexItem, bool isDefault);
};

void FlexItemCreatorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest SetUpTestCase";
}

void FlexItemCreatorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest TearDownTestCase";
}

void FlexItemCreatorTest::SetUp() {}

void FlexItemCreatorTest::TearDown() {}

RefPtr<Component> FlexItemCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    return component;
}

void FlexItemCreatorTest::CheckItemProperties(const RefPtr<FlexItemComponent>& flexItem, bool isDefault)
{
    double basis = flexItem->GetFlexBasis().Value();
    double grow = flexItem->GetFlexGrow();
    double shrink = flexItem->GetFlexShrink();
    if (isDefault) {
        EXPECT_TRUE(NearEqual(basis, DEFAULT_FLEX_BASIS));
        EXPECT_TRUE(NearEqual(grow, DEFAULT_FLEX_GROW));
        EXPECT_TRUE(NearEqual(shrink, DEFAULT_FLEX_SHRINK));
    } else {
        EXPECT_TRUE(NearEqual(basis, TEST_FLEX_BASIS));
        EXPECT_TRUE(NearEqual(grow, TEST_FLEX_GROW));
        EXPECT_TRUE(NearEqual(shrink, TEST_FLEX_SHRINK));
    }
    RefPtr<Component> child = flexItem->GetChild();
    ASSERT_TRUE(child != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(child);
    ASSERT_TRUE(text != nullptr);
    EXPECT_TRUE(text->GetData() == TEST_TEXT_VALUE);
}

/**
 * @tc.name: FlexItemProperties001
 * @tc.desc: Check flex item component with no properties can be created with default properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1Q
 * @tc.author: yangfan
 */
HWTEST_F(FlexItemCreatorTest, FlexItemProperties001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemProperties001 start";
    /**
     * @tc.steps: step1. construct the json string where flex item has no properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"FlexItem\",      "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get flex item component.
     * @tc.expected: step2. properties are set as default.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(component);
    ASSERT_TRUE(flexItem != nullptr);
    CheckItemProperties(flexItem, true);
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemProperties001 stop";
}

/**
 * @tc.name: FlexItemProperties002
 * @tc.desc: Check flex item component with invalid properties can be created with default properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1Q
 * @tc.author: yangfan
 */
HWTEST_F(FlexItemCreatorTest, FlexItemProperties002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemProperties002 start";
    /**
     * @tc.steps: step1. construct the json string of flex item with invalid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"FlexItem\",    "
                           "  \"flexGrow\": -1,               "
                           "  \"flexShrink\": -1,             "
                           "  \"flexBasis\": -0.5,            "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get flex item component.
     * @tc.expected: step2. properties are set as default.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(component);
    ASSERT_TRUE(flexItem != nullptr);
    CheckItemProperties(flexItem, true);
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemProperties002 stop";
}

/**
 * @tc.name: FlexItemProperties003
 * @tc.desc: Check flex item component with valid properties can be created with correct properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1Q
 * @tc.author: yangfan
 */
HWTEST_F(FlexItemCreatorTest, FlexItemProperties003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemProperties003 start";
    /**
     * @tc.steps: step1. construct the json string of flex item with valid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"FlexItem\",    "
                           "  \"flexGrow\": 1,               "
                           "  \"flexShrink\": 2,             "
                           "  \"flexBasis\": 20.0,            "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get flex item component.
     * @tc.expected: step2. properties are set correctly.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(component);
    ASSERT_TRUE(flexItem != nullptr);
    CheckItemProperties(flexItem, false);
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemProperties003 stop";
}

/**
 * @tc.name: FlexItemChild001
 * @tc.desc: Verify the flex item component with single flex child is correct.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1Q
 * @tc.author: yangfan
 */
HWTEST_F(FlexItemCreatorTest, FlexItemChild001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemChild001 start";
    /**
     * @tc.steps: step1. construct the json string of flex item with single child.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"FlexItem\",    "
                           "  \"flexGrow\": 0,               "
                           "  \"flexShrink\": 0,             "
                           "  \"flexBasis\": 0,            "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get flex item component.
     * @tc.expected: step2. properties are set correctly.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(component);
    ASSERT_TRUE(flexItem != nullptr);
    CheckItemProperties(flexItem, true);
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemChild001 stop";
}

/**
 * @tc.name: FlexItemChild002
 * @tc.desc: Verify the flex item component with double child returns single child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1Q
 * @tc.author: yangfan
 */
HWTEST_F(FlexItemCreatorTest, FlexItemChild002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemChild002 start";
    /**
     * @tc.steps: step1. construct the json string of flex item with double child.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"FlexItem\",    "
                           "  \"flexGrow\": 0,                "
                           "  \"flexShrink\": 0,              "
                           "  \"flexBasis\": 0,               "
                           "  \"child\": ["
                           "          {                       "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          },                      "
                           "          {                       "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "          ]                       "
                           "}";
    /**
     * @tc.steps: step2. get flex item component.
     * @tc.expected: step2. properties are set correctly, only one child in the flex item.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(component);
    ASSERT_TRUE(flexItem != nullptr);
    CheckItemProperties(flexItem, true);
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemChild002 stop";
}

/**
 * @tc.name: FlexItemChild003
 * @tc.desc: Verify the flex item component with no child returns no child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR1Q
 * @tc.author: yangfan
 */
HWTEST_F(FlexItemCreatorTest, FlexItemChild003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemChild003 start";
    /**
     * @tc.steps: step1. construct the json string of flex item with double child.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"FlexItem\",    "
                           "  \"flexGrow\": 0,                "
                           "  \"flexShrink\": 0,              "
                           "  \"flexBasis\": 0                "
                           "}";
    /**
     * @tc.steps: step2. get flex item component.
     * @tc.expected: step2. properties are set correctly, only one child in the flex item.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<FlexItemComponent> flexItem = AceType::DynamicCast<FlexItemComponent>(component);
    ASSERT_TRUE(flexItem != nullptr);
    double basis = flexItem->GetFlexBasis().Value();
    int32_t grow = flexItem->GetFlexGrow();
    int32_t shrink = flexItem->GetFlexShrink();
    EXPECT_TRUE(NearEqual(basis, DEFAULT_FLEX_BASIS));
    EXPECT_TRUE(grow == DEFAULT_FLEX_GROW);
    EXPECT_TRUE(shrink == DEFAULT_FLEX_SHRINK);
    RefPtr<Component> child = flexItem->GetChild();
    ASSERT_TRUE(child == nullptr);
    GTEST_LOG_(INFO) << "FlexItemCreatorTest FlexItemChild003 stop";
}

} // namespace OHOS::Ace