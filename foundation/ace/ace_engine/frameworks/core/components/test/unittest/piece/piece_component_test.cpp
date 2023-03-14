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

#include "base/geometry/dimension.h"
#include "base/resource/internal_resource.h"
#include "core/components/display/render_display.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/image/image_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/piece/piece_component.h"
#include "core/components/text/text_component.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {
namespace {

const std::string PIECE_TEXT = "Chips";
const std::string PIECE_ICON_SRC = "test.svg";
constexpr Dimension PIECE_INTERVAL = 4.0_vp;
const Edge PIECE_IMAGE_PADDING_LTR = Edge(4.0_vp, 0.0_vp, 0.0_vp, 0.0_vp);
const Edge PIECE_IMAGE_PADDING_RTL = Edge(0.0_vp, 0.0_vp, 4.0_vp, 0.0_vp);

} // namespace

class PieceComponentTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: PieceComponentBuildChild001
 * @tc.desc: Verify the BuildChild Interface of PieceComponent work correctly with content and showDelete.
 * @tc.type: FUNC
 * @tc.require: AR000F3L33
 * @tc.author: caocan
 */
HWTEST_F(PieceComponentTest, PieceComponentBuildChild001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct PieceComponent with content and showDelete.
     */
    RefPtr<PieceComponent> piece = AceType::MakeRefPtr<PieceComponent>();
    piece->SetContent(PIECE_TEXT);
    piece->SetShowDelete(true);
    piece->SetInterval(PIECE_INTERVAL);
    piece->SetTextDirection(TextDirection::LTR);
    piece->SetIconResource(InternalResource::ResourceId::PIECE_DELETE_SVG);

    /**
     * @tc.steps: step2. call the BuildChild interface of PieceComponent.
     * @tc.expected: step2. child of PieceComponent is built correctly.
     */
    auto row = AceType::DynamicCast<RowComponent>(piece->BuildChild());
    ASSERT_TRUE(row);
    EXPECT_TRUE(row->GetChildren().size() == 2);
    auto textFlex = AceType::DynamicCast<FlexItemComponent>(row->GetChildren().front());
    ASSERT_TRUE(textFlex);
    auto text = AceType::DynamicCast<TextComponent>(textFlex->GetChild());
    ASSERT_TRUE(text);
    auto content = text->GetData();
    EXPECT_TRUE(PIECE_TEXT == content);
    auto displayComponent = AceType::DynamicCast<DisplayComponent>(row->GetChildren().back());
    ASSERT_TRUE(displayComponent);
    auto imageFlex = AceType::DynamicCast<FlexItemComponent>(displayComponent->GetChild());
    ASSERT_TRUE(imageFlex);
    auto padding = AceType::DynamicCast<PaddingComponent>(imageFlex->GetChild());
    ASSERT_TRUE(padding);
    EXPECT_TRUE(PIECE_IMAGE_PADDING_LTR == padding->GetPadding());
    auto gestureListener = AceType::DynamicCast<GestureListenerComponent>(padding->GetChild());
    ASSERT_TRUE(gestureListener);
    auto image = AceType::DynamicCast<ImageComponent>(gestureListener->GetChild());
    ASSERT_TRUE(image);
    EXPECT_TRUE(image->GetSrc().empty());
    EXPECT_TRUE(image->GetResourceId() == InternalResource::ResourceId::PIECE_DELETE_SVG);
}

/**
 * @tc.name: PieceComponentBuildChild002
 * @tc.desc: Verify the BuildChild Interface of PieceComponent work correctly in RTL.
 * @tc.type: FUNC
 * @tc.require: AR000F3L33
 * @tc.author: caocan
 */
HWTEST_F(PieceComponentTest, PieceComponentBuildChild002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct PieceComponent with content, showDelete and icon in RTL.
     */
    RefPtr<PieceComponent> piece = AceType::MakeRefPtr<PieceComponent>();
    piece->SetContent(PIECE_TEXT);
    piece->SetShowDelete(true);
    piece->SetIcon(PIECE_ICON_SRC);
    piece->SetInterval(PIECE_INTERVAL);
    piece->SetTextDirection(TextDirection::RTL);

    /**
     * @tc.steps: step2. call the BuildChild interface of PieceComponent.
     * @tc.expected: step2. child of PieceComponent is built correctly.
     */
    auto row = AceType::DynamicCast<RowComponent>(piece->BuildChild());
    ASSERT_TRUE(row);
    EXPECT_TRUE(row->GetChildren().size() == 2);
    auto displayComponent = AceType::DynamicCast<DisplayComponent>(row->GetChildren().back());
    ASSERT_TRUE(displayComponent);
    auto imageFlex = AceType::DynamicCast<FlexItemComponent>(displayComponent->GetChild());
    ASSERT_TRUE(imageFlex);
    auto padding = AceType::DynamicCast<PaddingComponent>(imageFlex->GetChild());
    ASSERT_TRUE(padding);
    EXPECT_TRUE(PIECE_IMAGE_PADDING_RTL == padding->GetPadding());
    auto gestureListener = AceType::DynamicCast<GestureListenerComponent>(padding->GetChild());
    ASSERT_TRUE(gestureListener);
    auto image = AceType::DynamicCast<ImageComponent>(gestureListener->GetChild());
    ASSERT_TRUE(image);
    EXPECT_TRUE(PIECE_ICON_SRC == image->GetSrc());
    EXPECT_TRUE(image->GetResourceId() == InternalResource::ResourceId::NO_ID);
}

/**
 * @tc.name: PieceComponentBuildChild003
 * @tc.desc: Verify the BuildChild Interface of PieceComponent work correctly with content only.
 * @tc.type: FUNC
 * @tc.require: AR000F3L33
 * @tc.author: caocan
 */
HWTEST_F(PieceComponentTest, PieceComponentBuildChild003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct PieceComponent with content and showDelete.
     */
    RefPtr<PieceComponent> piece = AceType::MakeRefPtr<PieceComponent>();
    piece->SetContent(PIECE_TEXT);

    /**
     * @tc.steps: step2. call the BuildChild interface of PieceComponent.
     * @tc.expected: step2. child of PieceComponent is built correctly.
     */
    auto row = AceType::DynamicCast<RowComponent>(piece->BuildChild());
    ASSERT_TRUE(row);
    EXPECT_TRUE(row->GetChildren().size() == 2);
    auto flex = AceType::DynamicCast<FlexItemComponent>(row->GetChildren().front());
    ASSERT_TRUE(flex);
    auto text = AceType::DynamicCast<TextComponent>(flex->GetChild());
    ASSERT_TRUE(text);
}

/**
 * @tc.name: PieceComponentBuildChild004
 * @tc.desc: Verify the BuildChild Interface of PieceComponent work correctly with showDelete only.
 * @tc.type: FUNC
 * @tc.require: AR000F3L33
 * @tc.author: caocan
 */
HWTEST_F(PieceComponentTest, PieceComponentBuildChild004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct PieceComponent with showDelete only.
     */
    RefPtr<PieceComponent> piece = AceType::MakeRefPtr<PieceComponent>();
    piece->SetShowDelete(true);

    /**
     * @tc.steps: step2. call the BuildChild interface of PieceComponent.
     * @tc.expected: step2. child of PieceComponent is built correctly.
     */
    auto row = AceType::DynamicCast<RowComponent>(piece->BuildChild());
    EXPECT_TRUE(row == nullptr);
}

/**
 * @tc.name: PieceComponentBuildChild005
 * @tc.desc: Verify the BuildChild Interface of PieceComponent work correctly no attributes.
 * @tc.type: FUNC
 * @tc.require: AR000F3L33
 * @tc.author: caocan
 */
HWTEST_F(PieceComponentTest, PieceComponentBuildChild005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. construct PieceComponent with no attributes.
     */
    RefPtr<PieceComponent> piece = AceType::MakeRefPtr<PieceComponent>();

    /**
     * @tc.steps: step2. call the BuildChild interface of PieceComponent.
     * @tc.expected: step2. child of PieceComponent is built correctly.
     */
    auto row = AceType::DynamicCast<RowComponent>(piece->BuildChild());
    EXPECT_TRUE(row == nullptr);
}

} // namespace OHOS::Ace
