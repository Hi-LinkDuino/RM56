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
#include "frameworks/bridge/common/dom/dom_button.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomCardButtonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomCardButtonTest::SetUpTestCase() {}
void DomCardButtonTest::TearDownTestCase() {}
void DomCardButtonTest::SetUp() {}
void DomCardButtonTest::TearDown() {}

/**
 * @tc.name: DomCardButtonTest001
 * @tc.desc: Verify that DomButton can be created successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardButtonTest, DomCardButtonTest001, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"button\"    "
                              "                },                         "
                              "  \"styles\": {},                          "
                              "  \"actions\": {},                         "
                              "  \"data\": {}                             "
                              "}                                          ";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domButton = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domButton != nullptr);
    ASSERT_TRUE(domButton->GetTag() == "button");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardButtonTest002
 * @tc.desc: Verify that DomButton data binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardButtonTest, DomCardButtonTest002, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"button\",   "
                              "                   \"shown\": \"{{show}}\" "
                              "                },                         "
                              "  \"styles\": {},                          "
                              "  \"actions\": {},                         "
                              "  \"data\": {                              "
                              "                \"show\": false            "
                              "            }                              "
                              "}                                          ";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domButton = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domButton != nullptr);
    ASSERT_TRUE(domButton->GetTag() == "button");
    ASSERT_TRUE(domButton->IsShow() == false);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardButtonTest003
 * @tc.desc: Verify that DomButton styles set successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardButtonTest, DomCardButtonTest003, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"button\",          "
                              "                     \"classList\": [               "
                              "                         \"container\"              "
                              "                      ]                             "
                              "                 },                                 "
                              "   \"styles\": {                                    "
                              "                   \".container\": {                "
                              "                       \"height\": \"300px\",       "
                              "                       \"width\": \"300px\",        "
                              "                       \"marginTop\": \"30px\",     "
                              "                       \"marginBottom\": \"30px\",  "
                              "                       \"marginLeft\": \"30px\",    "
                              "                       \"marginRight\": \"30px\"    "
                              "                    }                               "
                              "               },                                   "
                              "   \"actions\": {},                                 "
                              "   \"data\": {}                                     "
                              "}";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domButton = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domButton != nullptr);
    ASSERT_TRUE(domButton->GetTag() == "button");
    auto declaration = domButton->GetDeclaration();
    auto& buttonStyle = static_cast<ButtonStyle&>(declaration->GetStyle(StyleTag::SPECIALIZED_STYLE));
    ASSERT_TRUE(static_cast<int32_t>(buttonStyle.width.Value()) == 300);
    ASSERT_TRUE(static_cast<int32_t>(buttonStyle.height.Value()) == 300);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);

    ASSERT_TRUE(declaration != nullptr);
    auto& marginStyle = static_cast<CommonMarginStyle&>(declaration->GetStyle(StyleTag::COMMON_MARGIN_STYLE));
    ASSERT_TRUE(marginStyle.IsValid());
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Top().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Bottom().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Left().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Right().Value()) == 30);
}

/**
 * @tc.name: DomCardButtonTest004
 * @tc.desc: Verify that DomButton event binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardButtonTest, DomCardButtonTest004, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"button\",          "
                              "                     \"events\": {                  "
                              "                         \"click\": \"test\"        "
                              "                      }                             "
                              "                 },                                 "
                              "   \"styles\": {},                                  "
                              "   \"actions\": {},                                 "
                              "   \"data\": {}                                     "
                              "}";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domButton = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domButton->GetTag() == "button");
    ASSERT_TRUE(domButton != nullptr);
    auto buttonComponent = AceType::DynamicCast<ButtonComponent>(domButton->GetSpecializedComponent());
    ASSERT_TRUE(buttonComponent != nullptr);
    EXPECT_TRUE(buttonComponent->GetClickedEventId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(buttonComponent->GetClickedEventId().GetData().eventId == DOM_DEFAULT_ROOT_NODE_ID);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}
} // namespace OHOS::Ace::Framework
