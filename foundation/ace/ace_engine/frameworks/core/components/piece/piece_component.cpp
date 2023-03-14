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

#include "core/components/piece/piece_component.h"

#include "core/components/display/render_display.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/flex_item_component.h"
#include "core/components/gesture_listener/gesture_listener_component.h"
#include "core/components/image/image_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/piece/piece_element.h"
#include "core/components/piece/render_piece.h"

namespace OHOS::Ace {

PieceComponent::PieceComponent()
{
    if (!declaration_) {
        declaration_ = AceType::MakeRefPtr<PieceDeclaration>();
        declaration_->Init();
    }
}
void PieceComponent::InitializeStyle(RefPtr<PieceTheme>& theme)
{
    declaration_->InitializeStyle(theme);
}

RefPtr<Element> PieceComponent::CreateElement()
{
    return AceType::MakeRefPtr<PieceElement>();
}

RefPtr<RenderNode> PieceComponent::CreateRenderNode()
{
    return RenderPiece::Create();
}

RefPtr<Component> PieceComponent::BuildChild()
{
    if (GetContent().empty()) {
        return nullptr;
    }
    std::list<RefPtr<Component>> rowChildren;
    auto row = AceType::MakeRefPtr<RowComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, rowChildren);
    row->SetMainAxisSize(MainAxisSize::MIN);
    if (iconPosition_ == IconPosition::Start) {
        SetImage(row);
        SetText(row);
    } else {
        SetText(row);
        SetImage(row);
    }
    row->SetTextDirection(GetTextDirection());
    return row;
}

const std::string& PieceComponent::GetContent() const
{
    return declaration_->GetContent();
}
void PieceComponent::SetContent(const std::string& content)
{
    declaration_->SetContent(content);
}

const std::string& PieceComponent::GetIcon() const
{
    return declaration_->GetIcon();
}
void PieceComponent::SetIcon(const std::string& icon)
{
    declaration_->SetIcon(icon);
}

const TextStyle& PieceComponent::GetTextStyle() const
{
    return declaration_->GetTextStyle();
}
void PieceComponent::SetTextStyle(const TextStyle& textStyle)
{
    declaration_->SetTextStyle(textStyle);
}

const Dimension& PieceComponent::GetInterval() const
{
    return declaration_->GetInterval();
}
void PieceComponent::SetInterval(const Dimension& interval)
{
    declaration_->SetInterval(interval);
}

InternalResource::ResourceId PieceComponent::GetIconResource() const
{
    return declaration_->GetIconResource();
}
void PieceComponent::SetIconResource(InternalResource::ResourceId iconResource)
{
    declaration_->SetIconResource(iconResource);
}

const Dimension& PieceComponent::GetIconSize() const
{
    return declaration_->GetIconSize();
}
void PieceComponent::SetIconSize(const Dimension& iconSize)
{
    declaration_->SetIconSize(iconSize);
}

const EventMarker& PieceComponent::GetOnDelete() const
{
    return declaration_->GetOnDelete();
}
void PieceComponent::SetOnDelete(const EventMarker& onDelete)
{
    declaration_->SetOnDelete(onDelete);
}

bool PieceComponent::ShowDelete() const
{
    return declaration_->ShowDelete();
}
void PieceComponent::SetShowDelete(bool showDelete)
{
    declaration_->SetShowDelete(showDelete);
}

const Edge& PieceComponent::GetMargin() const
{
    return declaration_->GetMargin();
}
void PieceComponent::SetMargin(const Edge& margin)
{
    declaration_->SetMargin(margin);
}

const Border& PieceComponent::GetBorder() const
{
    return declaration_->GetBorder();
}
void PieceComponent::SetBorder(const Border& border)
{
    declaration_->SetBorder(border);
}

const Color& PieceComponent::GetHoverColor() const
{
    return declaration_->GetHoverColor();
}
void PieceComponent::SetHoverColor(const Color& hoverColor)
{
    declaration_->SetHoverColor(hoverColor);
}

const Color& PieceComponent::GetBackGroundColor() const
{
    return declaration_->GetBackGroundColor();
}
void PieceComponent::SetBackGroundColor(const Color& backGroundColor)
{
    declaration_->SetBackGroundColor(backGroundColor);
}

void PieceComponent::SetDeclaration(const RefPtr<PieceDeclaration>& declaration)
{
    if (declaration) {
        declaration_ = declaration;
    }
}

void PieceComponent::SetImage(RefPtr<RowComponent>& row)
{
    RefPtr<ImageComponent> image =
        GetIcon().empty() ? MakeRefPtr<ImageComponent>(GetIconResource()) : MakeRefPtr<ImageComponent>(GetIcon());
    image->SetWidth(GetIconSize());
    image->SetHeight(GetIconSize());
    image->SetImageFill(declaration_->GetImageFill());
    auto gestureListener = MakeRefPtr<GestureListenerComponent>();
    gestureListener->SetOnClickId(GetOnDelete());
    gestureListener->SetChild(image);
    auto padding = MakeRefPtr<PaddingComponent>();
    if (GetTextDirection() == TextDirection::RTL || GetIconPosition() == IconPosition::Start) {
        padding->SetPadding(Edge(0.0_vp, 0.0_vp, GetInterval(), 0.0_vp));
    } else {
        padding->SetPadding(Edge(GetInterval(), 0.0_vp, 0.0_vp, 0.0_vp));
    }
    padding->SetChild(gestureListener);
    auto iconFlex = MakeRefPtr<FlexItemComponent>(0, 0, 0.0, padding);

    auto displayComponent = MakeRefPtr<DisplayComponent>();
    displayComponent->SetChild(iconFlex);
    if (ShowDelete()) {
        displayComponent->SetVisible(VisibleType::VISIBLE);
    } else {
        displayComponent->SetVisible(VisibleType::GONE);
    }
    row->AppendChild(displayComponent);
}

void PieceComponent::SetText(RefPtr<RowComponent>& row)
{
    auto text = MakeRefPtr<TextComponent>(GetContent());
    text->SetTextStyle(GetTextStyle());
    text->SetFocusColor(GetTextStyle().GetTextColor());
    auto textFlex = MakeRefPtr<FlexItemComponent>(0, 1, 0.0, text);
    row->AppendChild(textFlex);
}

} // namespace OHOS::Ace