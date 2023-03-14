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

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/flex/flex_component.h"
#include "core/components/root/root_component.h"
#include "core/components/test/json/flex_creator.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr int32_t SINGLE_CHILD_COUNT = 1;
constexpr int32_t MULTI_CHILD_COUNT = 2;
constexpr int32_t TEST_MAIN_ALIGN = 3;
constexpr int32_t DEFAULT_MAIN_ALIGN = 1;
constexpr int32_t TEST_CROSS_ALIGN = 2;
constexpr int32_t DEFAULT_CROSS_ALIGN = 1;
const std::string TEST_TEXT_VALUE = "Hi Ace";

} // namespace

class FlexCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<Component> CreateComponent(const std::string& testJson);
};

void FlexCreatorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "FlexCreatorTest SetUpTestCase";
}

void FlexCreatorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "FlexCreatorTest TearDownTestCase";
}

void FlexCreatorTest::SetUp() {}

void FlexCreatorTest::TearDown() {}

RefPtr<Component> FlexCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    return component;
}

/**
 * @tc.name: FlexClassname001
 * @tc.desc: Check flex component with right classname can create correctly
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexClassname001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexClassname001 start";
    /**
     * @tc.steps: step1. construct the json string where column has correct className.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Column\",      "
                           "  \"mainAxisAlign\": 3,          "
                           "  \"crossAxisAlign\": 2,          "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get column component
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ColumnComponent> column = AceType::DynamicCast<ColumnComponent>(component);
    ASSERT_TRUE(column != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(column->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(column->GetMainAxisAlign());
    std::list<RefPtr<Component>> children = column->GetChildren();
    EXPECT_TRUE(crossAxisAlign == TEST_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    EXPECT_TRUE(children.size() == SINGLE_CHILD_COUNT);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexClassname001 stop";
}

/**
 * @tc.name: FlexClassname002
 * @tc.desc: Check flex component with wrong classname cannot create
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexClassname002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexClassname002 start";
    /**
     * @tc.steps: step1. construct the json string where column has wrong className.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"ErrorColumn\", "
                           "  \"mainAxisAlign\": 3,           "
                           "  \"crossAxisAlign\": 2,          "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get column component
     * @tc.expected: step2. column is not created
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component == nullptr);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexClassname002 stop";
}

/**
 * @tc.name: FlexProperties001
 * @tc.desc: Check flex component with properties bigger than enum in the json returns default properties
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexProperties001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties001 start";
    /**
     * @tc.steps: step1. construct the json string where column has invalid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Column\",      "
                           "  \"mainAxisAlign\": 10,          "
                           "  \"crossAxisAlign\": 9,          "
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
    RefPtr<ColumnComponent> column = AceType::DynamicCast<ColumnComponent>(component);
    ASSERT_TRUE(column != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(column->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(column->GetMainAxisAlign());
    std::list<RefPtr<Component>> children = column->GetChildren();
    EXPECT_TRUE(crossAxisAlign == DEFAULT_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == DEFAULT_MAIN_ALIGN);
    EXPECT_TRUE(children.size() == SINGLE_CHILD_COUNT);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties001 stop";
}

/**
 * @tc.name: FlexProperties002
 * @tc.desc: Check flex component with properties smaller than enum in the json returns default properties
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexProperties002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties002 start";
    /**
     * @tc.steps: step1. construct the json string where column has invalid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Row\",         "
                           "  \"mainAxisAlign\": -1.5,        "
                           "  \"crossAxisAlign\": -1,         "
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
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(component);
    ASSERT_TRUE(row != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(row->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(row->GetMainAxisAlign());
    std::list<RefPtr<Component>> children = row->GetChildren();
    EXPECT_TRUE(crossAxisAlign == DEFAULT_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == DEFAULT_MAIN_ALIGN);
    EXPECT_TRUE(children.size() == SINGLE_CHILD_COUNT);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties002 stop";
}

/**
 * @tc.name: FlexProperties003
 * @tc.desc: Check flex component with no crossAxisAlign in the json returns default crossAxisAlign
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexProperties003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties003 start";
    /**
     * @tc.steps: step1. construct the json string where column has no crossAxisAlign.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Row\",         "
                           "  \"mainAxisAlign\": 3,           "
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
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(component);
    ASSERT_TRUE(row != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(row->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(row->GetMainAxisAlign());
    std::list<RefPtr<Component>> children = row->GetChildren();
    EXPECT_TRUE(crossAxisAlign == DEFAULT_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    EXPECT_TRUE(children.size() == MULTI_CHILD_COUNT);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties003 stop";
}

/**
 * @tc.name: FlexProperties004
 * @tc.desc: Check flex component with no mainAxisAlign in the json returns default mainAxisAlign
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexProperties004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties004 start";
    /**
     * @tc.steps: step1. construct the json string where column has no crossAxisAlign.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Column\",      "
                           "  \"crossAxisAlign\": 2,          "
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
     * @tc.steps: step2. get column component
     * @tc.expected: step2. properties are set as default
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ColumnComponent> column = AceType::DynamicCast<ColumnComponent>(component);
    ASSERT_TRUE(column != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(column->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(column->GetMainAxisAlign());
    std::list<RefPtr<Component>> children = column->GetChildren();
    EXPECT_TRUE(crossAxisAlign == TEST_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == DEFAULT_MAIN_ALIGN);
    EXPECT_TRUE(children.size() == MULTI_CHILD_COUNT);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexProperties004 stop";
}

/**
 * @tc.name: FlexChild001
 * @tc.desc: Verify flex component with single flex child is correct when load the json
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexChild001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild001 start";
    /**
     * @tc.steps: step1. construct the json with one flex child
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Column\",      "
                           "  \"mainAxisAlign\": 3,           "
                           "  \"crossAxisAlign\": 2,          "
                           "  \"child\": {                    "
                           "    \"className\": \"Row\",      "
                           "    \"child\": {"
                           "    \"className\": \"Text\",      "
                           "    \"value\": \"Hi Ace\"         "
                           "               }         "
                           "   }                              "
                           "}";
    /**
     * @tc.steps: step2. get row component with one child
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<ColumnComponent> column = AceType::DynamicCast<ColumnComponent>(component);
    ASSERT_TRUE(column != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(column->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(column->GetMainAxisAlign());
    int32_t childrenCount = column->GetChildren().size();
    EXPECT_TRUE(childrenCount == SINGLE_CHILD_COUNT);
    EXPECT_TRUE(crossAxisAlign == TEST_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(column->GetChildren().front());
    ASSERT_TRUE(row != nullptr);
    ASSERT_TRUE(row->GetChildren().size() == SINGLE_CHILD_COUNT);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(row->GetChildren().front());
    ASSERT_TRUE(text != nullptr);
    EXPECT_TRUE(text->GetData() == TEST_TEXT_VALUE);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild001 stop";
}

/**
 * @tc.name: FlexChild002
 * @tc.desc: Verify flex component with two flex child is correct when load the json
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexChild002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild002 start";
    /**
     * @tc.steps: step1. construct the json with two flex child
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Row\",         "
                           "  \"mainAxisAlign\": 3,           "
                           "  \"crossAxisAlign\": 2,          "
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
     * @tc.steps: step2. get row component with one child
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(component);
    ASSERT_TRUE(row != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(row->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(row->GetMainAxisAlign());
    int32_t childrenCount = row->GetChildren().size();
    EXPECT_TRUE(childrenCount == MULTI_CHILD_COUNT);
    EXPECT_TRUE(crossAxisAlign == TEST_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    RefPtr<ColumnComponent> firstChild = AceType::DynamicCast<ColumnComponent>(row->GetChildren().front());
    ASSERT_TRUE(firstChild != nullptr);
    RefPtr<TextComponent> firstChildText = AceType::DynamicCast<TextComponent>(firstChild->GetChildren().front());
    ASSERT_TRUE(firstChildText != nullptr);
    EXPECT_TRUE(firstChildText->GetData() == TEST_TEXT_VALUE);
    RefPtr<ColumnComponent> secondChild = AceType::DynamicCast<ColumnComponent>(row->GetChildren().back());
    RefPtr<TextComponent> secondChildText = AceType::DynamicCast<TextComponent>(secondChild->GetChildren().front());
    ASSERT_TRUE(secondChildText != nullptr);
    EXPECT_TRUE(secondChildText->GetData() == TEST_TEXT_VALUE);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild002 stop";
}

/**
 * @tc.name: FlexChild003
 * @tc.desc: Verify flex component with complex child is correct when load the json
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexChild003, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild003 start";
    /**
     * @tc.steps: step1. construct the json with complex child
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Row\",         "
                           "  \"mainAxisAlign\": 3,           "
                           "  \"crossAxisAlign\": 2,          "
                           "  \"child\": ["
                           "    {                             "
                           "    \"className\": \"Column\",    "
                           "    \"mainAxisAlign\": 3,         "
                           "    \"child\": [{"
                           "        \"className\": \"Text\",  "
                           "         \"value\": \"Hi Ace\"    "
                           "               },                 "
                           "               {"
                           "        \"className\": \"Text\",  "
                           "         \"value\": \"Hi Ace\"    "
                           "               }"
                           "          ]"
                           "    },                            "
                           "   {                              "
                           "    \"className\": \"Text\",      "
                           "    \"value\": \"Hi Ace\"         "
                           "   }                              "
                           "   ]"
                           "}";
    /**
     * @tc.steps: step2. get row component with complex child
     * @tc.expected: step2. all the properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(component);
    ASSERT_TRUE(row != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(row->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(row->GetMainAxisAlign());
    int32_t childrenCount = row->GetChildren().size();
    EXPECT_TRUE(childrenCount == MULTI_CHILD_COUNT);
    EXPECT_TRUE(crossAxisAlign == TEST_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    RefPtr<ColumnComponent> column = AceType::DynamicCast<ColumnComponent>(row->GetChildren().front());
    ASSERT_TRUE(column != nullptr);
    int32_t columnMainAlign = static_cast<int32_t>(column->GetMainAxisAlign());
    EXPECT_TRUE(columnMainAlign == TEST_MAIN_ALIGN);
    EXPECT_TRUE(column->GetChildren().size() == MULTI_CHILD_COUNT);
    RefPtr<TextComponent> columnText = AceType::DynamicCast<TextComponent>(column->GetChildren().front());
    EXPECT_TRUE(columnText->GetData() == TEST_TEXT_VALUE);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(row->GetChildren().back());
    EXPECT_TRUE(text->GetData() == TEST_TEXT_VALUE);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild003 stop";
}

/**
 * @tc.name: FlexChild004
 * @tc.desc: Verify flex component with no child is correct when load the json
 * @tc.type: FUNC
 */
HWTEST_F(FlexCreatorTest, FlexChild004, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild004 start";
    /**
     * @tc.steps: step1. construct the json with no child
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Row\",         "
                           "  \"mainAxisAlign\": 3,           "
                           "  \"crossAxisAlign\": -1          "
                           "}";
    /**
     * @tc.steps: step2. get row component with no child
     * @tc.expected: step2. properties are set correctly
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<RowComponent> row = AceType::DynamicCast<RowComponent>(component);
    ASSERT_TRUE(row != nullptr);
    int32_t crossAxisAlign = static_cast<int32_t>(row->GetCrossAxisAlign());
    int32_t mainAxisAlign = static_cast<int32_t>(row->GetMainAxisAlign());
    std::list<RefPtr<Component>> children = row->GetChildren();
    EXPECT_TRUE(crossAxisAlign == DEFAULT_CROSS_ALIGN);
    EXPECT_TRUE(mainAxisAlign == TEST_MAIN_ALIGN);
    EXPECT_TRUE(children.size() == 0);
    GTEST_LOG_(INFO) << "FlexCreatorTest FlexChild004 stop";
}

} // namespace OHOS::Ace