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
#include "frameworks/bridge/common/dom/dom_image.h"
#include "frameworks/bridge/test/unittest/cardfrontend/card_test_factory.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace::Framework {

class DomCardImageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void DomCardImageTest::SetUpTestCase() {}
void DomCardImageTest::TearDownTestCase() {}
void DomCardImageTest::SetUp() {}
void DomCardImageTest::TearDown() {}

/**
 * @tc.name: DomCardImageTest001
 * @tc.desc: Verify that DomImage can be created successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardImageTest, DomCardImageTest001, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"image\"     "
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
    auto domImage = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domImage != nullptr);
    ASSERT_TRUE(domImage->GetTag() == "image");
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardImageTest002
 * @tc.desc: Verify that DomImage data binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardImageTest, DomCardImageTest002, TestSize.Level1)
{
    const std::string index = "{"
                              "  \"template\": {                          "
                              "                   \"type\": \"image\",    "
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
    auto domImage = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domImage != nullptr);
    ASSERT_TRUE(domImage->GetTag() == "image");
    ASSERT_TRUE(domImage->IsShow() == false);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}

/**
 * @tc.name: DomCardImageTest003
 * @tc.desc: Verify that DomImage styles set successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardImageTest, DomCardImageTest003, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"image\",           "
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
    auto domImage = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domImage != nullptr);
    ASSERT_TRUE(domImage->GetTag() == "image");
    ASSERT_TRUE(static_cast<int32_t>(domImage->GetHeight().Value()) == 300);
    ASSERT_TRUE(static_cast<int32_t>(domImage->GetWidth().Value()) == 300);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);

    auto declaration = domImage->GetDeclaration();
    ASSERT_TRUE(declaration != nullptr);
    auto& marginStyle = static_cast<CommonMarginStyle&>(declaration->GetStyle(StyleTag::COMMON_MARGIN_STYLE));
    ASSERT_TRUE(marginStyle.IsValid());
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Top().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Bottom().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Left().Value()) == 30);
    ASSERT_TRUE(static_cast<int32_t>(marginStyle.margin.Right().Value()) == 30);
}

/**
 * @tc.name: DomCardImageTest004
 * @tc.desc: Verify that DomImage event binding successfully.
 * @tc.type: FUNC
 */
HWTEST_F(DomCardImageTest, DomCardImageTest004, TestSize.Level1)
{
    const std::string index = "{                                                   "
                              "   \"template\": {                                  "
                              "                     \"type\": \"image\",          "
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
    auto domImage = document->GetDOMNodeById(DOM_ROOT_NODE_ID_BASE);
    ASSERT_TRUE(domImage != nullptr);
    ASSERT_TRUE(domImage->GetTag() == "image");
    auto gestureEventComponent = domImage->GetGestureListenerComponent();
    ASSERT_TRUE(gestureEventComponent != nullptr);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventType == DOM_CLICK);
    EXPECT_TRUE(gestureEventComponent->GetOnClickId().GetData().eventId == DOM_DEFAULT_ROOT_NODE_ID);
    ASSERT_TRUE(CardTestFactory::GetInstance().GetNodeNumber() == 1);
}
} // namespace OHOS::Ace::Framework
