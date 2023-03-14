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
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomCardTextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomCardTextTest::SetUpTestCase() {}
void DomCardTextTest::TearDownTestCase() {}
void DomCardTextTest::SetUp() {}
void DomCardTextTest::TearDown() {}

/**
 * @tc.name: DomCardTextTest001
 * @tc.desc: Verify that DomText can be created successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardTextTest, DomCardTextTest001, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"text\"      "
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
    auto domText = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domText != nullptr);
    ASSERT_TRUE(domText->GetTag() == "text");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardTextTest002
 * @tc.desc: Verify that DomText data binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardTextTest, DomCardTextTest002, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                              "
                              "                   \"type\": \"text\",         "
                              "                   \"attr\": {                 "
                              "                      \"value\": \"{{tag}}\"   "
                              "                    }                          "
                              "                },                             "
                              "  \"styles\": {},                              "
                              "  \"actions\": {},                             "
                              "  \"data\": {                                  "
                              "                \"tag\": \"hi\"                "
                              "            }                                  "
                              "}                                              ";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domText = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domText != nullptr);
    ASSERT_TRUE(domText->GetTag() == "text");
    auto textComponent = AceType::DynamicCast<TextComponent>(domText->GetSpecializedComponent());
    ASSERT_TRUE(textComponent != nullptr);
    ASSERT_TRUE(textComponent->GetData() == "hi");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardTextTest003
 * @tc.desc: Verify that DomText append child successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardTextTest, DomCardTextTest003, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                             "
                              "                   \"type\": \"text\",        "
                              "                   \"children\": [            "
                              "                       {                      "
                              "                          \"type\": \"span\"  "
                              "                       },                     "
                              "                       {                      "
                              "                          \"type\": \"span\"  "
                              "                       },                     "
                              "                       {                      "
                              "                          \"type\": \"span\"  "
                              "                       }                      "
                              "                   ]                          "
                              "                },                            "
                              "  \"styles\": {},                             "
                              "  \"actions\": {},                            "
                              "  \"data\": {}                                "
                              "}                                             ";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domText = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domText != nullptr);
    ASSERT_TRUE(domText->GetTag() == "text");
    ASSERT_TRUE(domText->GetChildList().size() == 3);
    auto domSpan = document->GetDOMNodeById(1);
    ASSERT_TRUE(domSpan->GetTag() == "span");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 4);
}

/**
 * @tc.name: DomCardTextTest004
 * @tc.desc: Verify that DomText styles set successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardTextTest, DomCardTextTest004, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"text\",            "
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
    auto domText = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domText != nullptr);
    ASSERT_TRUE(domText->GetTag() == "text");
    ASSERT_TRUE(static_cast<int32_t>(domText->GetHeight().Value()) == 300);
    ASSERT_TRUE(static_cast<int32_t>(domText->GetWidth().Value()) == 300);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);

    auto declaration = domText->GetDeclaration();
    ASSERT_TRUE(declaration != nullptr);
    auto& marginStyle = static_cast<CommonMarginStyle&>(declaration->GetStyle(StyleTag::COMMON_MARGIN_STYLE));
    ASSERT_TRUE(marginStyle.IsValid());
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Top().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Bottom().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Left().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Right().Value()) == 30);
}

/**
 * @tc.name: DomCardTextTest005
 * @tc.desc: Verify that DomText event binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardTextTest, DomCardTextTest005, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"text\",            "
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
    auto domText = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domText != nullptr);
    auto gestureEventComponent = domText->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent != nullptr);
    ASSERT_TRUE(domText->GetTag() == "text");
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventId == DOM_DEFAULT_ROOT_NODE_ID);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}
} // namespace OHOS::Ace::Framework
