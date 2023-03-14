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
#include "core/components/popup/popup_component.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const Placement PLACEMENT_VALUE = Placement::LEFT;
const Placement PLACEMENT_DEFAULT = Placement::BOTTOM;
const Color MASK_COLOR_VALUE = Color(255);
const Color MASK_COLOR_DEFAULT = Color(0xff000000);
const std::string ON_VISIBILITY_CHANGE = "testCallback";
const std::string TEXT_VALUE_DEFAULT = "HiAce";
const std::string TEXT_VALUE = "Hello World";

} // namespace

class PopupCreatorTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: PopupCreator001
 * @tc.desc: Verify PopupCreator can create popup component with all properties.
 * @tc.type: FUNC
 */
HWTEST_F(PopupCreatorTest, PopupCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of popup component with all properties.
     */
    const std::string json = ""
                             "{                                            "
                             "  \"className\": \"Popup\",                  "
                             "  \"id\": \"1\",                             "
                             "  \"name\": \"popup\",                       "
                             "  \"placement\": \"left\",                   "
                             "  \"maskColor\": {                           "
                             "    \"className\": \"Color\",                "
                             "    \"value\": 255                           "
                             "  },                                         "
                             "  \"onVisibilityChange\": \"testCallback\",  "
                             "  \"child\": {                               "
                             "    \"className\": \"Text\",                 "
                             "    \"value\": \"HiAce\"                     "
                             "  }                                          "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get popup component.
     * @tc.expected: step2. Set all properties correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<PopupComponent> popup = AceType::DynamicCast<PopupComponent>(component);
    ASSERT_TRUE(popup);
    EXPECT_EQ(popup->GetPopupParam()->GetPlacement(), PLACEMENT_VALUE);
    EXPECT_EQ(popup->GetPopupParam()->GetMaskColor(), MASK_COLOR_VALUE);
    EXPECT_EQ(popup->GetPopupParam()->GetOnVisibilityChange(), ON_VISIBILITY_CHANGE);
    RefPtr<TextComponent> child = AceType::DynamicCast<TextComponent>(popup->GetChild());
    ASSERT_TRUE(child);
    EXPECT_EQ(child->GetData(), TEXT_VALUE_DEFAULT);
}

/**
 * @tc.name: PopupCreator002
 * @tc.desc: Verify PopupCreator can handle exception with className only.
 * @tc.type: FUNC
 */
HWTEST_F(PopupCreatorTest, PopupCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of popup component with className only.
     */
    const std::string json = ""
                             "{                           "
                             "  \"className\": \"Popup\"  "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get popup component.
     * @tc.expected: step2. popup is null.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    EXPECT_TRUE(!component);
}

/**
 * @tc.name: PopupCreator003
 * @tc.desc: Verify PopupCreator can create popup component with child property only.
 * @tc.type: FUNC
 */
HWTEST_F(PopupCreatorTest, PopupCreator003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of popup component with child property only.
     */
    const std::string json = ""
                             "{                                            "
                             "  \"className\": \"Popup\",                  "
                             "  \"id\": \"1\",                             "
                             "  \"name\": \"popup\",                       "
                             "  \"placement\": \"default\",                "
                             "  \"maskColor\": 10,                         "
                             "  \"onVisibilityChange\": 10,                "
                             "  \"child\": {                               "
                             "    \"className\": \"Text\",                 "
                             "    \"value\": \"HiAce\"                     "
                             "  }                                          "
                             "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get popup component.
     * @tc.expected: step2. properties are set as default.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(json.c_str());
    size_t dslLen = json.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component);
    RefPtr<PopupComponent> popup = AceType::DynamicCast<PopupComponent>(component);
    ASSERT_TRUE(popup);
    EXPECT_EQ(popup->GetPopupParam()->GetPlacement(), PLACEMENT_DEFAULT);
    EXPECT_EQ(popup->GetPopupParam()->GetMaskColor(), MASK_COLOR_DEFAULT);
    EXPECT_EQ(popup->GetPopupParam()->GetOnVisibilityChange(), "");
}

} // namespace OHOS::Ace