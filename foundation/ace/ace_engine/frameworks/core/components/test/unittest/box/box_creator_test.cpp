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
#include "core/components/box/box_component.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/common/properties/border.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/edge.h"
#include "core/components/test/json/box_creator.h"
#include "core/components/test/json/component_factory.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const double BOX_WIDTH = 100.0;
const double BOX_WIDTH_DEFAULT = -1.0;
const double BOX_HEIGHT = 200.0;
const double BOX_HEIGHT_DEFAULT = -1.0;
const double BOX_ALIGN = -1.0;
const double BOX_ALIGN_DEFAULT = 0.0;
const Edge BOX_EDGE_VALUE = Edge(0.0);
const Edge BOX_MARGIN_VALUE = Edge(10.0);
const Edge BOX_PADDING_VALUE = Edge(20.0);
const Color BOX_COLOR_VALUE = Color(255);
const Border BOX_BACK_BORDER =
    Border(BorderEdge(Color(0xff000000), Dimension(1.0, DimensionUnit::PX), BorderStyle::NONE));
const Border BOX_FRONT_BORDER =
    Border(BorderEdge(Color(0xff000000), Dimension(2.0, DimensionUnit::PX), BorderStyle::NONE));
const Border BOX_BORDER_DEFAULT =
    Border(BorderEdge(Color(0xffff0000), Dimension(0.0, DimensionUnit::PX), BorderStyle::NONE));
const std::string TEXT_VALUE = "Hi Ace";

} // namespace

class BoxCreatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BoxCreatorTest::SetUpTestCase() {}
void BoxCreatorTest::TearDownTestCase() {}
void BoxCreatorTest::SetUp() {}
void BoxCreatorTest::TearDown() {}

/**
 * @tc.name: BoxCreator001
 * @tc.desc: Verify BoxCreator can create box with all properties.
 * @tc.type: FUNC
 */
HWTEST_F(BoxCreatorTest, BoxCreator001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of box component with all properties.
     */
    const std::string content = ""
                                "{                                                                          "
                                "  \"className\": \"Box\", \"width\": 100, \"height\": 200,                 "
                                "  \"align\": { \"horizontal\": \"left\", \"vertical\": \"top\" },          "
                                "  \"margin\": { \"className\": \"Edge\", \"value\": 10.0 },                "
                                "  \"padding\": { \"className\": \"Edge\", \"value\": 20.0 },               "
                                "  \"color\": { \"className\": \"Color\", \"value\": 255 },                 "
                                "  \"backDecoration\": {                                                    "
                                "    \"className\": \"Decoration\",                                         "
                                "    \"border\": { \"className\": \"Border\",                               "
                                "      \"value\": { \"className\": \"BorderEdge\", \"width\": 1 }           "
                                "    }                                                                      "
                                "  },                                                                       "
                                "  \"frontDecoration\": {                                                   "
                                "    \"className\": \"Decoration\",                                         "
                                "    \"border\": {                                                          "
                                "      \"className\": \"Border\",                                           "
                                "      \"value\": { \"className\": \"BorderEdge\", \"width\": 2 }           "
                                "    }                                                                      "
                                "  },                                                                       "
                                "  \"child\": { \"className\": \"Text\", \"value\": \"Hi Ace\" }            "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get box component.
     * @tc.expected: step2. properties and child are set correctly.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(component);
    ASSERT_TRUE(box != nullptr);
    ASSERT_TRUE(NearEqual(box->GetWidthDimension().Value(), BOX_WIDTH));
    ASSERT_TRUE(NearEqual(box->GetHeightDimension().Value(), BOX_HEIGHT));
    ASSERT_TRUE(NearEqual(box->GetAlignment().GetHorizontal(), BOX_ALIGN));
    ASSERT_TRUE(NearEqual(box->GetAlignment().GetVertical(), BOX_ALIGN));
    ASSERT_TRUE(box->GetMargin() == BOX_MARGIN_VALUE);
    ASSERT_TRUE(box->GetPadding() == BOX_PADDING_VALUE);
    ASSERT_TRUE(box->GetColor() == BOX_COLOR_VALUE);
    ASSERT_TRUE(box->GetBackDecoration()->GetBorder() == BOX_BACK_BORDER);
    ASSERT_TRUE(box->GetFrontDecoration()->GetBorder() == BOX_FRONT_BORDER);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(box->GetChild());
    ASSERT_TRUE(text != nullptr);
    ASSERT_TRUE(text->GetData() == TEXT_VALUE);
}

/**
 * @tc.name: BoxCreator002
 * @tc.desc: Verify BoxCreator can create box with className only.
 * @tc.type: FUNC
 */
HWTEST_F(BoxCreatorTest, BoxCreator002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct the json string of box component with className only.
     */
    const std::string content = ""
                                "{                           "
                                "  \"className\": \"Box\"    "
                                "}";

    /**
     * @tc.steps: step2. call CreateFromJson interface and get box component.
     * @tc.expected: step2. properties are set as default, child is nullptr.
     */
    const auto dslBuf = reinterpret_cast<const uint8_t*>(content.c_str());
    size_t dslLen = content.length();
    RefPtr<Component> component = ComponentFactory::GetInstance().CreateComponentFromDsl(dslBuf, dslLen);
    ASSERT_TRUE(component != nullptr);
    RefPtr<BoxComponent> box = AceType::DynamicCast<BoxComponent>(component);
    ASSERT_TRUE(box != nullptr);
    ASSERT_TRUE(NearEqual(box->GetWidthDimension().Value(), BOX_WIDTH_DEFAULT));
    ASSERT_TRUE(NearEqual(box->GetHeightDimension().Value(), BOX_HEIGHT_DEFAULT));
    ASSERT_TRUE(NearEqual(box->GetAlignment().GetHorizontal(), BOX_ALIGN_DEFAULT));
    ASSERT_TRUE(NearEqual(box->GetAlignment().GetVertical(), BOX_ALIGN_DEFAULT));
    ASSERT_TRUE(box->GetMargin() == BOX_EDGE_VALUE);
    ASSERT_TRUE(box->GetPadding() == BOX_EDGE_VALUE);
    ASSERT_TRUE(box->GetColor() == Color::TRANSPARENT);
    ASSERT_TRUE(box->GetBackDecoration() == nullptr);
    ASSERT_TRUE(box->GetFrontDecoration() == nullptr);
    RefPtr<TextComponent> text = AceType::DynamicCast<TextComponent>(box->GetChild());
    ASSERT_TRUE(text == nullptr);
}

} // namespace OHOS::Ace