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

#include "frameworks/bridge/common/dom/dom_popup.h"

#include <string>

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/box/box_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/popup/popup_theme.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr uint32_t MASK_COLOR_ALPHA = 0x4c000000; // UX standand, 30% opacity

} // namespace

DOMPopup::DOMPopup(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    popupChild_ = AceType::MakeRefPtr<PopupComponent>(std::to_string(nodeId), nodeName);
}

DOMPopup::~DOMPopup()
{
    RemoveMarker();
}

void DOMPopup::InitializeStyle()
{
    RefPtr<PopupTheme> theme = GetTheme<PopupTheme>();
    if (!theme) {
        return;
    }
    maskColor_ = theme->GetMaskColor();
    popupChild_->GetPopupParam()->SetMaskColor(maskColor_);
    backgroundColor_ = theme->GetBackgroundColor();
    popupChild_->GetPopupParam()->SetBackgroundColor(backgroundColor_);

    if (!declaration_) {
        return;
    }
    auto& borderStyle = declaration_->MaybeResetStyle<CommonBorderStyle>(StyleTag::COMMON_BORDER_STYLE);
    if (borderStyle.IsValid()) {
        borderStyle.border.SetBorderRadius(theme->GetRadius());
    }

    auto& paddingStyle = declaration_->MaybeResetStyle<CommonPaddingStyle>(StyleTag::COMMON_PADDING_STYLE);
    if (paddingStyle.IsValid()) {
        paddingStyle.padding = theme->GetPadding();
    }
    declaration_->SetHasBoxStyle(true);
    declaration_->SetHasDecorationStyle(true);
}

void DOMPopup::RemoveMarker()
{
    BackEndEventManager<void(const ClickInfo&)>::GetInstance().RemoveBackEndEvent(clickMarker_);
}

void DOMPopup::ResetInitializedStyle()
{
    InitializeStyle();
}

void DOMPopup::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    popupChild_->SetChild(nullptr);
}

bool DOMPopup::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    LOGD("DOMPopup SetChildAttr");
    // static linear map must be sorted by key.
    static const LinearMapNode<Placement> placeMap[] = {
        { "bottom", Placement::BOTTOM },
        { "bottomLeft", Placement::BOTTOM_LEFT },
        { "bottomRight", Placement::BOTTOM_RIGHT },
        { "left", Placement::LEFT },
        { "right", Placement::RIGHT },
        { "top", Placement::TOP },
        { "topLeft", Placement::TOP_LEFT },
        { "topRight", Placement::TOP_RIGHT },
    };
    if (attr.first == DOM_PLACEMENT) {
        auto valueIt = BinarySearchFindIndex(placeMap, ArraySize(placeMap), attr.second.c_str());
        if (valueIt != -1) {
            popupChild_->GetPopupParam()->SetPlacement(placeMap[valueIt].value);
        } else {
            LOGW("illegal placement value");
        }
        return true;
    } else if (attr.first == DOM_ARROW_OFFSET) {
        popupChild_->GetPopupParam()->SetArrowOffset(ParseDimension(attr.second));
        return true;
    } else if (attr.first == DOM_CLICKABLE) {
        clickable_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_KEEP_ALIVE) {
        popupChild_->GetPopupParam()->SetHasAction(StringToBool(attr.second));
        return true;
    }
    return false;
}

bool DOMPopup::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    LOGD("DOMPopup SetSpecializedStyle");
    if (style.first == DOM_BACKGROUND_COLOR || style.first == DOM_BACKGROUND || style.first == DOM_BACKGROUND_IMAGE) {
        hasBackground_ = true;
    }

    // popup don't support position
    if (style.first == DOM_POSITION || style.first == DOM_POSITION_LEFT || style.first == DOM_POSITION_RIGHT ||
        style.first == DOM_POSITION_TOP || style.first == DOM_POSITION_BOTTOM) {
        return true;
    }

    if (style.first == DOM_MASK_COLOR) {
        maskColor_ = ParseColor(style.second, MASK_COLOR_ALPHA);
        popupChild_->GetPopupParam()->SetMaskColor(maskColor_);
        return true;
    } else if (style.first == DOM_BACKGROUND_COLOR) {
        backgroundColor_ = ParseColor(style.second);
        popupChild_->GetPopupParam()->SetBackgroundColor(backgroundColor_);
        return false;
    } else {
        LOGW("DOMPopup unsupported style");
        return false;
    }
}

bool DOMPopup::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_VISIBILITY_CHANGE) {
        visibilityChangeEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        popupChild_->GetPopupParam()->SetOnVisibilityChange(visibilityChangeEventId_);
        return true;
    } else {
        LOGW("event type not supported");
        return false;
    }
}

void DOMPopup::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    auto popup = AceType::DynamicCast<PopupComponent>(popupChild_);
    if (!popup) {
        return;
    }
    auto controller = popup->GetPopupController();
    if (!controller) {
        return;
    }
    if (method == DOM_SHOW) {
        controller->ShowPopup();
    } else if (method == DOM_HIDE) {
        controller->CancelPopup();
    }
}

void DOMPopup::BindIdNode(const RefPtr<DOMNode>& idNode)
{
    if (!idNode) {
        return;
    }
    if (!clickMarker_.IsEmpty()) {
        RemoveMarker();
    }
    clickMarker_ = BackEndEventManager<void(const ClickInfo&)>::GetInstance().GetAvailableMarker();
    popupChild_->GetPopupParam()->SetTargetId(idNode->GetRootComponent()->GetId());
    popupChild_->GetPopupParam()->SetTargetMargin(idNode->GetBoxComponent()->GetMargin());

    if (clickable_) {
        BackEndEventManager<void(const ClickInfo&)>::GetInstance().BindBackendEvent(
            clickMarker_, [weakChild = WeakPtr<PopupComponent>(popupChild_), weakNode = WeakPtr<DOMNode>(idNode)](
                              const ClickInfo& clickInfo) {
                auto popupChild = weakChild.Upgrade();
                auto idNode = weakNode.Upgrade();
                if (popupChild && idNode) {
                    auto controller = popupChild->GetPopupController();
                    controller->ShowPopup();
                }
            });
        idNode->SetOnClick(clickMarker_);
        idNode->MarkNeedUpdate();
    }
}

void DOMPopup::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child) {
        return;
    }
    if (lastComponent_) {
        auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
        focusCollaboration->InsertChild(0, child->GetRootComponent());
        lastComponent_->SetChild(focusCollaboration);
    }
}

void DOMPopup::PrepareSpecializedComponent()
{
    if (boxComponent_->GetBackDecoration()) {
        auto boxBorder = boxComponent_->GetBackDecoration()->GetBorder();
        popupChild_->GetPopupParam()->SetBorder(boxBorder);
        Border border;
        border.SetTopLeftRadius(boxBorder.TopLeftRadius());
        border.SetTopRightRadius(boxBorder.TopRightRadius());
        border.SetBottomLeftRadius(boxBorder.BottomLeftRadius());
        border.SetBottomRightRadius(boxBorder.BottomRightRadius());
        boxComponent_->GetBackDecoration()->SetBorder(border);
        boxComponent_->GetBackDecoration()->SetBackgroundColor(Color::TRANSPARENT);
    }
    popupChild_->GetPopupParam()->SetIsShow(IsShow());
    popupChild_->GetPopupParam()->SetPadding(boxComponent_->GetPadding());
    popupChild_->GetPopupParam()->SetMargin(boxComponent_->GetMargin());
    boxComponent_->SetMargin(Edge());
}

RefPtr<Component> DOMPopup::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    lastComponent_ = components.back();
    popupChild_->SetChild(AceType::DynamicCast<Component>(components.front()));
    auto box = AceType::MakeRefPtr<BoxComponent>();
    box->SetChild(popupChild_);
    return box;
}

} // namespace OHOS::Ace::Framework
