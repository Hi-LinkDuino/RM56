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

#include "core/components/semi_modal/semi_modal_element.h"

#include "core/components/box/box_element.h"
#include "core/components/flex/flex_element.h"
#include "core/components/flex/flex_item_element.h"
#include "core/components/semi_modal/render_semi_modal.h"

namespace OHOS::Ace {
namespace {

const auto BACKGROUND_COLOR_DEFAULT = Color::WHITE;
constexpr uint32_t COLUMN_CHILD_MIN = 2;

} // namespace

void SemiModalElement::AnimateToFullWindow(int32_t duration)
{
    auto render = AceType::DynamicCast<RenderSemiModal>(GetRenderNode());
    if (!render) {
        LOGE("Animate full window failed. render is null");
        return;
    }
    render->AnimateToFullWindow(duration);
}

const Color& SemiModalElement::GetBackgroundColor() const
{
    auto boxElement = GetFirstChild();
    if (!boxElement) {
        LOGE("Get background color failed. box element is null");
        return BACKGROUND_COLOR_DEFAULT;
    }
    auto boxRender = AceType::DynamicCast<RenderBox>(boxElement->GetRenderNode());
    if (!boxRender) {
        LOGE("Get background color failed. box render is null");
        return BACKGROUND_COLOR_DEFAULT;
    }
    auto decoration = boxRender->GetBackDecoration();
    if (!decoration) {
        LOGE("Get background color failed. box decoration is null");
        return BACKGROUND_COLOR_DEFAULT;
    }
    return decoration->GetBackgroundColor();
}

bool SemiModalElement::IsFullWindow() const
{
    return fullWindow_;
}

void SemiModalElement::SetFullWindow(bool fullWindow)
{
    fullWindow_ = fullWindow;
}

void SemiModalElement::UpdateSystemBarHeight(double statusBar, double navigationBar)
{
    auto box = GetFirstChild();
    if (!box) {
        LOGE("Update system bar failed. box is null.");
        return;
    }
    auto column = box->GetFirstChild();
    UpdateStatusBarHeight(column, statusBar);
    UpdateNavigationBarHeight(column, navigationBar);
}

void SemiModalElement::UpdateStatusBarHeight(const RefPtr<Element>& column, double height)
{
    auto renderSemi = AceType::DynamicCast<RenderSemiModal>(GetRenderNode());
    if (!renderSemi) {
        LOGE("Update status bar failed. RenderSemi is null.");
        return;
    }
    renderSemi->OnStatusBarHeightChanged(Dimension(height, DimensionUnit::VP));
    if (!column) {
        LOGE("Update status bar failed. column is null.");
        return;
    }
    auto dragBarElement = column->GetFirstChild();
    if (!dragBarElement) {
        LOGE("Update status bar failed. drag bar element is null.");
        return;
    }
    auto dragBarRender = AceType::DynamicCast<RenderDragBar>(dragBarElement->GetRenderNode());
    if (!dragBarRender) {
        LOGE("Update status bar failed. drag bar render is null.");
        return;
    }
    dragBarRender->SetStatusBarHeight(height);
}

void SemiModalElement::UpdateNavigationBarHeight(const RefPtr<Element>& column, double height)
{
    auto renderSemi = AceType::DynamicCast<RenderSemiModal>(GetRenderNode());
    if (!renderSemi) {
        LOGE("Update navigation bar failed. RenderSemi is null.");
        return;
    }
    renderSemi->OnNavigationBarHeightChanged(Dimension(height, DimensionUnit::VP));
    if (!column) {
        LOGE("Update navigation bar failed. column is null.");
        return;
    }
    auto navigationElement = column->GetLastChild();
    if (!navigationElement) {
        LOGE("Update navigation bar failed. navigation element is null.");
        return;
    }
    auto navigationRender = AceType::DynamicCast<RenderBoxBase>(navigationElement->GetRenderNode());
    if (!navigationRender) {
        LOGE("Update navigation bar failed. navigation render is null.");
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("Update navigation bar failed. context is null.");
        return;
    }
    navigationRender->SetHeight(context->NormalizeToPx(Dimension(height, DimensionUnit::VP)));
}

RefPtr<OverlayElement> SemiModalElement::GetOverlayElement() const
{
    auto box = AceType::DynamicCast<BoxElement>(GetFirstChild());
    if (!box) {
        LOGE("Get overlay element failed. Box element is null!");
        return RefPtr<OverlayElement>();
    }
    auto column = AceType::DynamicCast<ColumnElement>(box->GetFirstChild());
    if (!column || column->GetChildren().size() < COLUMN_CHILD_MIN) {
        // column should has more than 2 child
        LOGE("Get overlay element failed. Column is null or child size error!");
        return RefPtr<OverlayElement>();
    }
    // Get second child
    auto secondItr = std::next(column->GetChildren().begin());
    auto flexItem = AceType::DynamicCast<FlexItemElement>(*secondItr);
    if (!flexItem) {
        LOGE("Get overlay element failed. FlexItemElement is null!");
        return RefPtr<OverlayElement>();
    }
    auto clip = flexItem->GetFirstChild();
    if (!clip) {
        LOGE("Get overlay element failed. clip is null!");
        return RefPtr<OverlayElement>();
    }
    auto stack = clip->GetFirstChild();
    if (!stack) {
        return RefPtr<OverlayElement>();
    }
    auto child = stack->GetChildren();
    if (child.size() > 1) {
        auto it = child.begin();
        it++;
        return AceType::DynamicCast<OverlayElement>(*it);
    }
    return RefPtr<OverlayElement>();
}

RefPtr<StageElement> SemiModalElement::GetStageElement() const
{
    auto box = AceType::DynamicCast<BoxElement>(GetFirstChild());
    if (!box) {
        LOGE("Get stage element failed. Box element is null!");
        return RefPtr<StageElement>();
    }
    auto column = AceType::DynamicCast<ColumnElement>(box->GetFirstChild());
    if (!column || column->GetChildren().size() < COLUMN_CHILD_MIN) {
        // column should has more than 2 child
        LOGE("Get stage element failed. Column is null or child size error!");
        return RefPtr<StageElement>();
    }
    // Get second child
    auto secondItr = std::next(column->GetChildren().begin());
    auto flexItem = AceType::DynamicCast<FlexItemElement>(*secondItr);
    if (!flexItem) {
        LOGE("Get stage element failed. FlexItemElement is null!");
        return RefPtr<StageElement>();
    }
    auto clip = flexItem->GetFirstChild();
    if (!clip) {
        LOGE("Get stage element failed. clip is null!");
        return RefPtr<StageElement>();
    }
    auto stack = clip->GetFirstChild();
    if (!stack) {
        return RefPtr<StageElement>();
    }
    return AceType::DynamicCast<StageElement>(stack->GetFirstChild());
}

void SemiModalElement::AnimateToExitApp() const
{
    auto renderSemi = AceType::DynamicCast<RenderSemiModal>(GetRenderNode());
    if (!renderSemi) {
        LOGE("Animate To Exit failed. RenderSemi is null.");
        return;
    }
    renderSemi->AnimateToExitApp();
}

} // namespace OHOS::Ace