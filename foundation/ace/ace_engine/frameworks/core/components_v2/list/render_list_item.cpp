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

#include "core/components_v2/list/render_list_item.h"

#include "core/components/button/button_component.h"
#include "core/components/image/image_component.h"

namespace OHOS::Ace::V2 {
namespace {

constexpr Dimension ICON_WIDTH = 24.0_px;
constexpr Dimension BUTTON_WIDTH = 72.0_px;

} // namespace

RefPtr<RenderNode> RenderListItem::Create()
{
    return AceType::MakeRefPtr<RenderListItem>();
}

void RenderListItem::Update(const RefPtr<Component>& component)
{
    auto item = AceType::DynamicCast<ListItemComponent>(component);
    if (!item) {
        LOGW("Not ListItemComponent, nothing to do");
        return;
    }

    component_ = item;
    SetEditMode(false);

    if (IsDeletable()) {
        CreateDeleteButton();
    } else {
        button_.Reset();
    }

    onSelectId_ = item->GetOnSelectId();
    selectable_ = item->GetSelectable();
    borderRadius_ = item->GetBorderRadius();

    MarkNeedLayout();
}

void RenderListItem::Paint(RenderContext& context, const Offset& offset)
{
    if (child_) {
        PaintChild(child_, context, offset);
    }
    if (button_ && editMode_) {
        PaintChild(button_, context, offset);
    }
}

void RenderListItem::PerformLayout()
{
    const auto& layoutParam = GetLayoutParam();
    Size childLayoutSize;
    Size buttonLayoutSize;

    if (button_ && editMode_) {
        button_->Layout(LayoutParam(layoutParam.GetMaxSize(), Size()));
        buttonLayoutSize = button_->GetLayoutSize();
    }

    if (child_) {
        auto maxSize = layoutParam.GetMaxSize();
        auto minSize = layoutParam.GetMinSize();
        if (!NearEqual(maxSize.Width(), Size::INFINITE_SIZE)) {
            maxSize.SetWidth(std::max(maxSize.Width() - buttonLayoutSize.Width(), 0.0));
            minSize.SetWidth(std::min(minSize.Width(), maxSize.Width()));
        }

        child_->Layout(LayoutParam(maxSize, minSize));
        childLayoutSize = child_->GetLayoutSize();
    }

    double width = childLayoutSize.Width() + buttonLayoutSize.Width();
    double height = std::max(childLayoutSize.Height(), buttonLayoutSize.Height());

    if (child_) {
        child_->SetPosition(Offset(0.0, (height - childLayoutSize.Height()) / 2.0));
    }

    if (button_ && editMode_) {
        button_->SetPosition(Offset(childLayoutSize.Width(), (height - buttonLayoutSize.Height()) / 2.0));
    }

    SetLayoutSize(layoutParam.Constrain(Size(width, height)));
}

void RenderListItem::OnChildAdded(const RefPtr<RenderNode>& child)
{
    if (child != button_) {
        child_ = child;
    }
}

void RenderListItem::OnChildRemoved(const RefPtr<RenderNode>& child)
{
    if (child != button_) {
        child_.Reset();
    }
}

void RenderListItem::SetEditMode(bool editMode)
{
    if (editMode_ == editMode) {
        return;
    }

    editMode_ = editMode;
    if (!button_) {
        return;
    }

    if (editMode_) {
        AddChild(button_);
    } else {
        RemoveChild(button_);
    }
}

void RenderListItem::CreateDeleteButton()
{
    if (!button_) {
        auto imageComponent = AceType::MakeRefPtr<ImageComponent>(InternalResource::ResourceId::CLOSE_SVG);
        imageComponent->SetImageSourceSize({ ICON_WIDTH, ICON_WIDTH });
        imageComponent->SetFitMaxSize(true);

        auto buttonComponent = AceType::MakeRefPtr<ButtonComponent>(std::list<RefPtr<Component>>());
        buttonComponent->SetType(ButtonType::ICON);
        buttonComponent->SetWidth(BUTTON_WIDTH);
        buttonComponent->SetHeight(BUTTON_WIDTH);

        buttonComponent->SetClickFunction([weak = AceType::WeakClaim(this)] {
            auto spThis = weak.Upgrade();
            if (spThis) {
                ResumeEventCallback(spThis, &RenderListItem::GetOnDeleteClick, spThis);
            }
        });

        auto button = buttonComponent->CreateRenderNode();
        button->Attach(context_);
        button->Update(buttonComponent);
        auto image = imageComponent->CreateRenderNode();
        image->Attach(context_);
        image->Update(imageComponent);

        button->AddChild(image);
        button_ = button;
    }
}

void RenderListItem::UpdateTouchRect()
{
    RenderNode::UpdateTouchRect();
    if (button_ && IsResponseRegion()) {
        auto buttonTouchRect = button_->GetPaintRect();
        std::vector<Rect> touchRectList;
        for (auto& region : responseRegion_) {
            double x = GetPxValue(touchRect_.Width(), region.GetOffset().GetX());
            double y = GetPxValue(touchRect_.Height(), region.GetOffset().GetY());
            double width = GetPxValue(buttonTouchRect.Width(), region.GetWidth());
            double height = GetPxValue(buttonTouchRect.Height(), region.GetHeight());
            Rect responseRegion(buttonTouchRect.GetOffset().GetX() + x,
                buttonTouchRect.GetOffset().GetY() + y, width, height);
            touchRectList.emplace_back(responseRegion);
        }
        button_->ChangeTouchRectList(touchRectList);
    }
}

} // namespace OHOS::Ace::V2
