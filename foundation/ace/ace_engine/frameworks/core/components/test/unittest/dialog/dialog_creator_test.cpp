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
#include "core/components/dialog/dialog_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string TEXT_VALUE = "HiAce";
const Edge DIALOG_EDGE = Edge(20);
const Color DIALOG_COLOR = Color(0xffff0000);
const Color DIALOG_COLOR_DEFAULT = Color(0xff000000);
const double BUTTON_SIZE_SMALL = 100.0;
const double BUTTON_SIZE_LARGE = 200.0;

} // namespace

class DialogCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DialogCreatorTest::SetUpTestCase() {}
void DialogCreatorTest::TearDownTestCase() {}
void DialogCreatorTest::SetUp() {}
void DialogCreatorTest::TearDown() {}

/**
 * @tc.name: DialogCreator001
 * @tc.desc: Verify DialogCreator can create dialog component with content.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with content.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"content\": {               "
                             "    \"className\": \"Text\",   "
                             "    \"value\": \"HiAce\"       "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. content is set correctly, other properties are set as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(dialog->GetContent());
    ASSERT_TRUE(text);
    EXPECT_EQ(text->GetData(), TEXT_VALUE);
    ASSERT_TRUE(dialog->GetTitle());
    EXPECT_EQ(dialog->GetTitle()->GetData(), "");
    EXPECT_EQ(dialog->GetTitlePadding(), Edge::NONE);
    EXPECT_EQ(dialog->GetContentPadding(), Edge::NONE);
    EXPECT_TRUE(dialog->GetAutoCancel());
    EXPECT_EQ(dialog->GetBackgroundColor(), DIALOG_COLOR_DEFAULT);
}

/**
 * @tc.name: DialogCreator002
 * @tc.desc: Verify DialogCreator can create dialog component with wrong content type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong content type.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"content\": {               "
                             "    \"className\": \"Image\"   "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. content is null.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    ASSERT_TRUE(dialog->GetContent());
    EXPECT_EQ(dialog->GetContent()->GetData(), "");
}

/**
 * @tc.name: DialogCreator003
 * @tc.desc: Verify DialogCreator can create dialog component with title.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with title.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"title\": {                 "
                             "    \"className\": \"Text\",   "
                             "    \"value\": \"HiAce\"       "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set title correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(dialog->GetTitle());
    ASSERT_TRUE(text);
    EXPECT_EQ(text->GetData(), TEXT_VALUE);
}

/**
 * @tc.name: DialogCreator004
 * @tc.desc: Verify DialogCreator can create dialog component with wrong title type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong title type.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"title\": {                 "
                             "    \"className\": \"Image\"   "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. title is null.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    ASSERT_TRUE(dialog->GetTitle());
    EXPECT_EQ(dialog->GetTitle()->GetData(), "");
}

/**
 * @tc.name: DialogCreator005
 * @tc.desc: Verify DialogCreator can create dialog component with contentPadding.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with contentPadding.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"contentPadding\": {        "
                             "    \"className\": \"Edge\",   "
                             "    \"value\": 20              "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set contentPadding correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_EQ(dialog->GetContentPadding(), DIALOG_EDGE);
}

/**
 * @tc.name: DialogCreator006
 * @tc.desc: Verify DialogCreator can create dialog component with wrong title type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong title type.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"contentPadding\": {        "
                             "    \"className\": \"Text\",   "
                             "    \"value\": \"HiAce\"       "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set contentPadding as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_EQ(dialog->GetContentPadding(), Edge::NONE);
}

/**
 * @tc.name: DialogCreator007
 * @tc.desc: Verify DialogCreator can create dialog component with titlePadding.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with titlePadding.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"titlePadding\": {          "
                             "    \"className\": \"Edge\",   "
                             "    \"value\": 20              "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set titlePadding correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_EQ(dialog->GetTitlePadding(), DIALOG_EDGE);
}

/**
 * @tc.name: DialogCreator008
 * @tc.desc: Verify DialogCreator can create dialog component with wrong titlePadding type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong titlePadding type.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"titlePadding\": {          "
                             "    \"className\": \"Text\",   "
                             "    \"value\": \"HiAce\"       "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set titlePadding as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_EQ(dialog->GetTitlePadding(), Edge::NONE);
}

/**
 * @tc.name: DialogCreator009
 * @tc.desc: Verify DialogCreator can create dialog component with autoCancel.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with autoCancel.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"autoCancel\": false        "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set autoCancel correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_TRUE(!dialog->GetAutoCancel());
}

/**
 * @tc.name: DialogCreator010
 * @tc.desc: Verify DialogCreator can create dialog component with wrong autoCancel type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong autoCancel type.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"autoCancel\": \"default\"  "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set autoCancel as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_TRUE(dialog->GetAutoCancel());
}

/**
 * @tc.name: DialogCreator011
 * @tc.desc: Verify DialogCreator can create dialog component with backgroundColor.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator011, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with backgroundColor.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"backgroundColor\": {       "
                             "    \"className\": \"Color\",  "
                             "    \"alpha\": 255,            "
                             "    \"red\": 255               "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set backgroundColor correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_EQ(dialog->GetBackgroundColor(), DIALOG_COLOR);
}

/**
 * @tc.name: DialogCreator012
 * @tc.desc: Verify DialogCreator can create dialog component with wrong backgroundColor type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong backgroundColor type.
     */
    const std::string json = ""
                             "{                              "
                             "  \"className\": \"Dialog\",   "
                             "  \"backgroundColor\": {       "
                             "    \"className\": \"Text\",   "
                             "    \"value\": \"HiAce\"       "
                             "  }                            "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set backgroundColor as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_EQ(dialog->GetBackgroundColor(), DIALOG_COLOR_DEFAULT);
}

/**
 * @tc.name: DialogCreator013
 * @tc.desc: Verify DialogCreator can create dialog component with actions.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator013, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with actions.
     */
    const std::string json = ""
                             "{                                "
                             "  \"className\": \"Dialog\",     "
                             "  \"actions\": [                 "
                             "    {                            "
                             "      \"className\": \"Button\", "
                             "      \"width\": 100,            "
                             "      \"height\": 100            "
                             "    },{                          "
                             "      \"className\": \"Button\", "
                             "      \"width\": 200,            "
                             "      \"height\": 200            "
                             "    }                            "
                             "  ]                              "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. Set actions correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_TRUE(dialog->GetActions().size() == 2);
    EXPECT_TRUE(dialog->GetActions().front()->GetWidth() == Dimension(BUTTON_SIZE_SMALL));
    EXPECT_TRUE(dialog->GetActions().front()->GetHeight() == Dimension(BUTTON_SIZE_SMALL));
    EXPECT_TRUE(dialog->GetActions().back()->GetWidth() == Dimension(BUTTON_SIZE_LARGE));
    EXPECT_TRUE(dialog->GetActions().back()->GetHeight() == Dimension(BUTTON_SIZE_LARGE));
}

/**
 * @tc.name: DialogCreator014
 * @tc.desc: Verify DialogCreator can create dialog component with wrong action type.
 * @tc.type: FUNC
 */
HWTEST_F(DialogCreatorTest, DialogCreator014, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of dialog component with wrong action type.
     */
    const std::string json = ""
                             "{                               "
                             "  \"className\": \"Dialog\",    "
                             "  \"actions\": [                "
                             "    {                           "
                             "      \"className\": \"Text\",  "
                             "      \"value\": \"HiAce\"      "
                             "    }                           "
                             "  ]                             "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get dialog component.
     * @tc.expected: step2. actions is empty.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<DialogComponent> dialog = AceType::DynamicCast<DialogComponent>(component);
    ASSERT_TRUE(dialog);
    EXPECT_TRUE(dialog->GetActions().empty());
}

} // namespace OHOS::Ace
