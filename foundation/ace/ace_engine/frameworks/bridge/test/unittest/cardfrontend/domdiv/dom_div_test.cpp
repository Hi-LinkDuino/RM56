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
#include "frameworks/bridge/common/dom/dom_div.h"
#include "frameworks/bridge/common/dom/dom_document.h"
#include "frameworks/bridge/common/dom/dom_text.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomCardDivTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomCardDivTest::SetUpTestCase() {}
void DomCardDivTest::TearDownTestCase() {}
void DomCardDivTest::SetUp() {}
void DomCardDivTest::TearDown() {}

/**
 * @tc.name: DomCardDivTest001
 * @tc.desc: Verify that DomDiv can be created successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardDivTest, DomCardDivTest001, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"div\"       "
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
    auto domDiv = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domDiv != nullptr);
    ASSERT_TRUE(domDiv->GetTag() == "div");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardDivTest002
 * @tc.desc: Verify that DomDiv data binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardDivTest, DomCardDivTest002, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"div\",      "
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
    auto domDiv = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domDiv != nullptr);
    ASSERT_TRUE(domDiv->GetTag() == "div");
    ASSERT_TRUE(domDiv->IsShow() == false);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardDivTest003
 * @tc.desc: Verify that DomDiv append child successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardDivTest, DomCardDivTest003, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                             "
                              "                   \"type\": \"div\",         "
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
                              "}                                             ";
    /**
     * @tc.steps: step1. parse card dsl by CardTestFactory.
     */
    auto document = AceType::MakeRefPtr<DOMDocument>(0);
    auto page = AceType::MakeRefPtr<JsAcePage>(0, document, "");
    CardTestFactory::GetInstance().ParseCardDsl(page, index);
    auto domDiv = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domDiv != nullptr);
    ASSERT_TRUE(domDiv->GetTag() == "div");
    ASSERT_TRUE(domDiv->IsShow() == true);
    ASSERT_TRUE(domDiv->GetChildList().size() == 3);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 4);
}

/**
 * @tc.name: DomCardDivTest004
 * @tc.desc: Verify that DomDiv styles set successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardDivTest, DomCardDivTest004, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"div\",             "
                              "                     \"classList\": [               "
                              "                         \"container\"             "
                              "                      ]                             "
                              "                 },                                 "
                              "   \"styles\": {                                    "
                              "                   \".container\": {                "
                              "                       \"height\": \"300px\",        "
                              "                       \"width\": \"300px\",         "
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
    auto domDiv = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domDiv != nullptr);
    ASSERT_TRUE(domDiv->GetTag() == "div");
    ASSERT_TRUE(static_cast<int32_t>(domDiv->GetHeight().Value()) == 300);
    ASSERT_TRUE(static_cast<int32_t>(domDiv->GetWidth().Value()) == 300);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);

    auto declaration = domDiv->GetDeclaration();
    ASSERT_TRUE(declaration != nullptr);
    auto& marginStyle = static_cast<CommonMarginStyle&>(declaration->GetStyle(StyleTag::COMMON_MARGIN_STYLE));
    ASSERT_TRUE(marginStyle.IsValid());
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Top().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Bottom().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Left().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Right().Value()) == 30);
}

/**
 * @tc.name: DomCardDivTest005
 * @tc.desc: Verify that DomDiv event binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardDivTest, DomCardDivTest005, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"div\",             "
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
    auto domDiv = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domDiv != nullptr);
    auto gestureEventComponent = domDiv->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent != nullptr);
    ASSERT_TRUE(domDiv->GetTag() == "div");
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventId == DOM_DEFAULT_ROOT_NODE_ID);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}
} // namespace OHOS::Ace::Framework
