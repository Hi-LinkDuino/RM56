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
#include "frameworks/bridge/common/dom/dom_stack.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomCardStackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomCardStackTest::SetUpTestCase() {}
void DomCardStackTest::TearDownTestCase() {}
void DomCardStackTest::SetUp() {}
void DomCardStackTest::TearDown() {}

/**
 * @tc.name: DomCardStackTest001
 * @tc.desc: Verify that DomStack can be created successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardStackTest, DomCardStackTest001, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"stack\"     "
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
    auto domStack = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domStack != nullptr);
    ASSERT_TRUE(domStack->GetTag() == "stack");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardStackTest002
 * @tc.desc: Verify that DomStack data binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardStackTest, DomCardStackTest002, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"stack\",    "
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
    auto domStack = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domStack != nullptr);
    ASSERT_TRUE(domStack->GetTag() == "stack");
    ASSERT_TRUE(domStack->IsShow() == false);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardStackTest003
 * @tc.desc: Verify that DomStack append child successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardStackTest, DomCardStackTest003, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                             "
                              "                   \"type\": \"stack\",       "
                              "                   \"shown\": \"{{show}}\",   "
                              "                   \"children\": [            "
                              "                       {                      "
                              "                          \"type\": \"text\"  "
                              "                       },                     "
                              "                       {                      "
                              "                          \"type\": \"text\"  "
                              "                       },                     "
                              "                       {                      "
                              "                          \"type\": \"text\"  "
                              "                       }                      "
                              "                   ]                          "
                              "                },                            "
                              "  \"styles\": {},                             "
                              "  \"actions\": {},                            "
                              "  \"data\": {                                 "
                              "                \"show\": true                "
                              "            }                                 "
                              "}";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domStack = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domStack != nullptr);
    ASSERT_TRUE(domStack->GetTag() == "stack");
    ASSERT_TRUE(domStack->IsShow() == true);
    ASSERT_TRUE(domStack->GetChildList().size() == 3);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 4);
}

/**
 * @tc.name: DomCardStackTest004
 * @tc.desc: Verify that DomStack styles set successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardStackTest, DomCardStackTest004, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"stack\",           "
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
    auto domStack = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domStack != nullptr);
    ASSERT_TRUE(domStack->GetTag() == "stack");
    ASSERT_TRUE(static_cast<int32_t>(domStack->GetHeight().Value()) == 300);
    ASSERT_TRUE(static_cast<int32_t>(domStack->GetWidth().Value()) == 300);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);

    auto declaration = domStack->GetDeclaration();
    ASSERT_TRUE(declaration != nullptr);
    auto& marginStyle = static_cast<CommonMarginStyle&>(declaration->GetStyle(StyleTag::COMMON_MARGIN_STYLE));
    ASSERT_TRUE(marginStyle.IsValid());
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Top().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Bottom().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Left().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Right().Value()) == 30);
}

/**
 * @tc.name: DomCardStackTest005
 * @tc.desc: Verify that DomStack event binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardStackTest, DomCardStackTest005, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"stack\",           "
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
    auto domStack = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domStack != nullptr);
    auto gestureEventComponent = domStack->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent != nullptr);
    ASSERT_TRUE(domStack->GetTag() == "stack");
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventId == DOM_DEFAULT_ROOT_NODE_ID);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

} // namespace OHOS::Ace::Framework
