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
#include "core/components/positioned/positioned_component.h"
#include "core/components/test/json/positioned_creator.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

constexpr double TEST_TOP = 10.0;
constexpr double TEST_BOTTOM = 10.0;
constexpr double TEST_LEFT = 10.0;
constexpr double TEST_RIGHT = 10.0;
constexpr double TEST_WIDTH = 10.0;
constexpr double TEST_HEIGHT = 10.0;
const std::string TEST_TEXT_VALUE = "Hi Ace";

} // namespace

class PositionedCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<Component> CreateComponent(const std::string& testJson);
    void CheckItemProperties(const RefPtr<PositionedComponent>& positioned, bool isDefault);
};

void PositionedCreatorTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "PositionedCreatorTest SetUpTestCase";
}

void PositionedCreatorTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "PositionedCreatorTest TearDownTestCase";
}

void PositionedCreatorTest::SetUp() {}

void PositionedCreatorTest::TearDown() {}

RefPtr<Component> PositionedCreatorTest::CreateComponent(const std::string& testJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(testJson.c_str());
    size_t dslLen = testJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    return component;
}

void PositionedCreatorTest::CheckItemProperties(const RefPtr<PositionedComponent>& positioned, bool isDefault)
{
    double bottom = positioned->GetBottom().Value();
    double top = positioned->GetTop().Value();
    double left = positioned->GetLeft().Value();
    double right = positioned->GetRight().Value();
    double width = positioned->GetWidth();
    double height = positioned->GetHeight();
    if (isDefault) {
        EXPECT_TRUE(NearEqual(top, 0));
        EXPECT_TRUE(NearEqual(bottom, 0));
        EXPECT_TRUE(NearEqual(left, 0));
        EXPECT_TRUE(NearEqual(right, 0));
        EXPECT_TRUE(NearEqual(width, 0));
        EXPECT_TRUE(NearEqual(height, 0));
    } else {
        EXPECT_TRUE(NearEqual(top, TEST_TOP));
        EXPECT_TRUE(NearEqual(bottom, TEST_BOTTOM));
        EXPECT_TRUE(NearEqual(left, TEST_LEFT));
        EXPECT_TRUE(NearEqual(right, TEST_RIGHT));
        EXPECT_TRUE(NearEqual(width, TEST_WIDTH));
        EXPECT_TRUE(NearEqual(height, TEST_HEIGHT));
    }
    RefPtr<Component> child = positioned->GetChild();
    ASSERT_TRUE(child != nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(child);
    ASSERT_TRUE(text != nullptr);
    EXPECT_TRUE(text->GetData() == TEST_TEXT_VALUE);
}

/**
 * @tc.name: PositionedProperties001
 * @tc.desc: Check positioned component with no properties can be created with default properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(PositionedCreatorTest, PositionedProperties001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string where positioned has no properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Positioned\",  "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get positioned component.
     * @tc.expected: step2. properties are set as default.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(component);
    ASSERT_TRUE(positioned != nullptr);
    CheckItemProperties(positioned, true);
}

/**
 * @tc.name: PositionedProperties002
 * @tc.desc: Check positioned component with invalid properties can be created with default properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(PositionedCreatorTest, PositionedProperties002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of flex item with invalid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Positioned\",  "
                           "  \"top\": \"-1\",                "
                           "  \"bottom\": \"-1\",             "
                           "  \"left\": \"-0.5\",             "
                           "  \"right\": \"-1\",              "
                           "  \"width\": \"-1\",              "
                           "  \"height\": \"-0.5\",           "
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
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(component);
    ASSERT_TRUE(positioned != nullptr);
    CheckItemProperties(positioned, true);
}

/**
 * @tc.name: PositionedProperties003
 * @tc.desc: Check positioned component with valid properties can be created with correct properties.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(PositionedCreatorTest, PositionedProperties003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of flex item with valid properties.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Positioned\",  "
                           "  \"top\": 10.0,               "
                           "  \"right\": 10.0,             "
                           "  \"bottom\": 10.0,            "
                           "  \"left\": 10.0,              "
                           "  \"width\": 10.0,             "
                           "  \"height\": 10.0,            "
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
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(component);
    ASSERT_TRUE(positioned != nullptr);
    CheckItemProperties(positioned, false);
}

/**
 * @tc.name: PositionedChild001
 * @tc.desc: Verify the positioned component with single flex child is correct.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(PositionedCreatorTest, PositionedChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of positioned with single child.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Positioned\",  "
                           "  \"left\": 0,               "
                           "  \"right\": 0,              "
                           "  \"width\": 0,              "
                           "  \"child\": {                    "
                           "           \"className\":\"Text\","
                           "             \"value\":\"Hi Ace\" "
                           "          }                       "
                           "}";
    /**
     * @tc.steps: step2. get positioned component.
     * @tc.expected: step2. properties are set correctly.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(component);
    ASSERT_TRUE(positioned != nullptr);
    CheckItemProperties(positioned, true);
}

/**
 * @tc.name: PositionedChild002
 * @tc.desc: Verify the positioned component with double child returns single child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(PositionedCreatorTest, PositionedChild002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of positioned with double child.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Positioned\",  "
                           "  \"left\": 0,                "
                           "  \"right\": 0,               "
                           "  \"top\": 0,                 "
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
     * @tc.steps: step2. get positioned component.
     * @tc.expected: step2. properties are set correctly, only one child in the positioned.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(component);
    ASSERT_TRUE(positioned != nullptr);
    CheckItemProperties(positioned, true);
}

/**
 * @tc.name: PositionedChild003
 * @tc.desc: Verify the positioned component with no child returns no child.
 * @tc.type: FUNC
 * @tc.require: AR000DAR27
 * @tc.author: yangfan
 */
HWTEST_F(PositionedCreatorTest, PositionedChild003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of positioned with double child.
     */
    std::string testJson = ""
                           "{                                 "
                           "  \"className\": \"Positioned\",  "
                           "  \"left\": 0,                "
                           "  \"right\": 0,               "
                           "  \"top\": 0                  "
                           "}";
    /**
     * @tc.steps: step2. get positioned component.
     * @tc.expected: step2. properties are set correctly, only one child in the positioned.
     */
    RefPtr<Component> component = CreateComponent(testJson);
    ASSERT_TRUE(component != nullptr);
    RefPtr<PositionedComponent> positioned = AceType::DynamicCast<PositionedComponent>(component);
    ASSERT_TRUE(positioned != nullptr);
    double bottom = positioned->GetBottom().Value();
    double top = positioned->GetTop().Value();
    double left = positioned->GetLeft().Value();
    double right = positioned->GetRight().Value();
    double width = positioned->GetWidth();
    double height = positioned->GetHeight();
    EXPECT_TRUE(NearEqual(top, 0));
    EXPECT_TRUE(NearEqual(bottom, 0));
    EXPECT_TRUE(NearEqual(left, 0));
    EXPECT_TRUE(NearEqual(right, 0));
    EXPECT_TRUE(NearEqual(width, 0));
    EXPECT_TRUE(NearEqual(height, 0));
    RefPtr<Component> child = positioned->GetChild();
    ASSERT_TRUE(child == nullptr);
}

} // namespace OHOS::Ace