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

#include "core/components/test/unittest/flex/flex_test_utils.h"

#include "core/components/box/box_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {
namespace {

constexpr double RECT_WIDTH = 1080.0;
constexpr double RECT_HEIGHT = 2244.0;
constexpr double MOCK_IDEO_BASELINE_RATE = 0.8;
constexpr double MOCK_ALPHA_BASELINE_RATE = 0.6;
constexpr double DEFAULT_WIDTH = 100.0;
constexpr double DEFAULT_HEIGHT = 100.0;
constexpr double DEFAULT_FLEX_BASIS = 0.0;
constexpr double DEFAULT_FLEX_GROW = 0.0;
constexpr double DEFAULT_FLEX_SHRINK = 1.0;

} // namespace

double MockRenderText::GetBaselineDistance(TextBaseline textBaseline)
{
    switch (textBaseline) {
        case TextBaseline::IDEOGRAPHIC:
            return textStyle_.GetFontSize().Value() * MOCK_IDEO_BASELINE_RATE;
        case TextBaseline::ALPHABETIC:
            return textStyle_.GetFontSize().Value() * MOCK_ALPHA_BASELINE_RATE;
        default:
            return textStyle_.GetFontSize().Value() * MOCK_ALPHA_BASELINE_RATE;
    }
}

Size MockRenderText::Measure()
{
    return Size(textStyle_.GetFontSize().Value(), textStyle_.GetFontSize().Value());
}

double MockRenderText::GetTextWidth()
{
    return textStyle_.GetFontSize().Value();
}

RefPtr<RenderRoot> FlexTestUtils::CreateRenderRoot()
{
    RefPtr<RenderRoot> root = AceType::MakeRefPtr<MockRenderRoot>();
    const Rect paintRect(0.0f, 0.0f, RECT_WIDTH, RECT_HEIGHT);
    root->SetPaintRect(paintRect);
    return root;
}

RefPtr<RenderFlex> FlexTestUtils::CreateRenderFlex(FlexDirection direction, FlexAlign main, FlexAlign cross)
{
    RefPtr<RenderFlex> flex = AceType::MakeRefPtr<RenderFlex>();
    std::list<RefPtr<Component>> children;
    if (direction == FlexDirection::ROW) {
        RefPtr<RowComponent> flexComponent = AceType::MakeRefPtr<RowComponent>(main, cross, children);
        flex->Update(flexComponent);
        return flex;
    } else {
        RefPtr<ColumnComponent> flexComponent = AceType::MakeRefPtr<ColumnComponent>(main, cross, children);
        flex->Update(flexComponent);
        return flex;
    }
}

RefPtr<RenderBox> FlexTestUtils::CreateRenderBox(double width, double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<RenderBox> FlexTestUtils::CreateWeightedBox(double weight, double width, double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetFlexWeight(weight);
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->UpdateAll(boxComponent);
    return renderBox;
}

RefPtr<RenderFlexItem> FlexTestUtils::CreateMagicNode(double aspectRatio, double flexWeight, int32_t displayIndex,
    const LayoutParam& constraints, const RefPtr<PipelineContext>& context)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(DEFAULT_WIDTH);
    boxComponent->SetHeight(DEFAULT_HEIGHT);
    boxComponent->SetMinWidth(Dimension(constraints.GetMinSize().Width()));
    boxComponent->SetMinHeight(Dimension(constraints.GetMinSize().Height()));
    boxComponent->SetMaxWidth(Dimension(constraints.GetMaxSize().Width()));
    boxComponent->SetMaxHeight(Dimension(constraints.GetMaxSize().Height()));
    boxComponent->SetAspectRatio(aspectRatio);
    renderBox->UpdateAll(boxComponent);
    renderBox->Attach(context);
    RefPtr<RenderFlexItem> renderFlexItem = AceType::MakeRefPtr<RenderFlexItem>();
    auto flexItemComponent =
        AceType::MakeRefPtr<FlexItemComponent>(DEFAULT_FLEX_GROW, DEFAULT_FLEX_SHRINK, DEFAULT_FLEX_BASIS);
    flexItemComponent->SetFlexWeight(flexWeight);
    flexItemComponent->SetDisplayIndex(displayIndex);
    flexItemComponent->SetMinWidth(Dimension(constraints.GetMinSize().Width()));
    flexItemComponent->SetMinHeight(Dimension(constraints.GetMinSize().Height()));
    flexItemComponent->SetMaxWidth(Dimension(constraints.GetMaxSize().Width()));
    flexItemComponent->SetMaxHeight(Dimension(constraints.GetMaxSize().Height()));
    renderFlexItem->UpdateAll(flexItemComponent);
    renderFlexItem->Attach(context);
    renderFlexItem->AddChild(renderBox);
    return renderFlexItem;
}

RefPtr<RenderFlexItem> FlexTestUtils::CreateRenderFlexItem(double flexBasis, double flexGrow, double flexShrink)
{
    RefPtr<RenderFlexItem> renderFlexItem = AceType::MakeRefPtr<RenderFlexItem>();
    renderFlexItem->SetFlexBasis(flexBasis);
    renderFlexItem->SetFlexShrink(flexShrink);
    renderFlexItem->SetFlexGrow(flexGrow);
    return renderFlexItem;
}

RefPtr<MockRenderText> FlexTestUtils::CreateRenderText(double fontSize)
{
    RefPtr<MockRenderText> renderText = AceType::MakeRefPtr<MockRenderText>();
    renderText->SetFontSize(fontSize);
    return renderText;
}

} // namespace OHOS::Ace
