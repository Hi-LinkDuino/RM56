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

#include "base/json/json_util.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string TEXT_DATA_JSON = "test1234@?!";
constexpr int32_t TEXT_ALIGN_JSON = 3;
constexpr int32_t TEXT_ALIGN_DEFAULT = 4;
constexpr uint32_t TEXT_MAX_LINES_JSON = 10;
constexpr uint32_t TEXT_MAX_LINES_DEFAULT = UINT32_MAX;
constexpr int32_t TEXT_OVERFLOW_JSON = 1;
constexpr int32_t TEXT_OVERFLOW_DEFAULT = 0;
constexpr TextDirection TEXT_DIRECTION_JSON = TextDirection::RTL;
constexpr TextDirection TEXT_DIRECTION_DEFAULT = TextDirection::LTR;

} // namespace

class TextCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    RefPtr<TextComponent> CreateComponent(const std::string& textJson);
};

void TextCreatorTest::SetUpTestCase() {}
void TextCreatorTest::TearDownTestCase() {}
void TextCreatorTest::SetUp() {}
void TextCreatorTest::TearDown() {}

RefPtr<TextComponent> TextCreatorTest::CreateComponent(const std::string& textJson)
{
    const auto dslBuf = reinterpret_cast<const uint8_t*>(textJson.c_str());
    size_t dslLen = textJson.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    if (!component) {
        return nullptr;
    }
    return AceType::DynamicCast<TextComponent>(component);
}

/**
 * @tc.name: TextComponentCreator001
 * @tc.desc: Test all the properties of text.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVN AR000DAQVP AR000DAQVQ AR000DAQVS
 * @tc.author: shanshurong
 */
HWTEST_F(TextCreatorTest, TextComponentCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonTextStr = "{                                 "
                                    "  \"className\": \"Text\",        "
                                    "  \"value\": \"test1234@?!\",   "
                                    "  \"textAlign\": 3,               "
                                    "  \"textDirection\": 1,           "
                                    "  \"textMaxLines\": 10,           "
                                    "  \"textOverflow\": 1             "
                                    "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    RefPtr<TextComponent> text = CreateComponent(jsonTextStr);
    EXPECT_TRUE(text != nullptr);
    EXPECT_EQ(text->GetData(), TEXT_DATA_JSON);
    EXPECT_EQ(static_cast<int32_t>(text->GetTextStyle().GetTextAlign()), TEXT_ALIGN_JSON);
    EXPECT_EQ(text->GetTextDirection(), TEXT_DIRECTION_JSON);
    EXPECT_EQ(text->GetTextStyle().GetMaxLines(), TEXT_MAX_LINES_JSON);
    EXPECT_EQ(static_cast<int32_t>(text->GetTextStyle().GetTextOverflow()), TEXT_OVERFLOW_JSON);
}

/**
 * @tc.name: TextComponentCreator002
 * @tc.desc: Test all the properties set by default value when value is invalid.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVP AR000DAQVQ AR000DAQVS
 * @tc.author: shanshurong
 */
HWTEST_F(TextCreatorTest, TextComponentCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonTextStr = "{                                  "
                                    "  \"className\": \"Text\",         "
                                    "  \"textAlign\": 10,               "
                                    "  \"textDirection\": 10,           "
                                    "  \"textMaxLines\": 10,            "
                                    "  \"textOverflow\": 10             "
                                    "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    RefPtr<TextComponent> text = CreateComponent(jsonTextStr);
    EXPECT_TRUE(text != nullptr);
    EXPECT_EQ(static_cast<int32_t>(text->GetTextStyle().GetTextAlign()), TEXT_ALIGN_DEFAULT);
    EXPECT_EQ(text->GetTextDirection(), TEXT_DIRECTION_DEFAULT);
    EXPECT_EQ(text->GetTextStyle().GetMaxLines(), TEXT_MAX_LINES_JSON);
    EXPECT_EQ(static_cast<int32_t>(text->GetTextStyle().GetTextOverflow()), TEXT_OVERFLOW_DEFAULT);
}

/**
 * @tc.name: TextComponentCreator003
 * @tc.desc: Test all the properties when initialized by default value.
 * @tc.type: FUNC
 * @tc.require: AR000DAQVP AR000DAQVQ AR000DAQVS
 * @tc.author: shanshurong
 */
HWTEST_F(TextCreatorTest, TextComponentCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. Construct the json string with all property.
     */
    const std::string jsonTextStr = "{                                  "
                                    "  \"className\": \"Text\"          "
                                    "}";
    /**
     * @tc.steps: step2. Check all the properties matched.
     * @tc.expected: step2. All the properties are matched.
     */
    RefPtr<TextComponent> text = CreateComponent(jsonTextStr);
    EXPECT_TRUE(text != nullptr);
    EXPECT_EQ(static_cast<int32_t>(text->GetTextStyle().GetTextAlign()), TEXT_ALIGN_DEFAULT);
    EXPECT_EQ(text->GetTextDirection(), TEXT_DIRECTION_DEFAULT);
    EXPECT_EQ(text->GetTextStyle().GetMaxLines(), TEXT_MAX_LINES_DEFAULT);
    EXPECT_EQ(static_cast<int32_t>(text->GetTextStyle().GetTextOverflow()), TEXT_OVERFLOW_DEFAULT);
}

} // namespace OHOS::Ace