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

#include "frameworks/bridge/common/dom/dom_list_item_group.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/focus_animation/focus_animation_theme.h"
#include "frameworks/bridge/common/dom/dom_list.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t DEFAULT_NODE_INDEX = -1;

} // namespace

DOMListItemGroup::DOMListItemGroup(NodeId nodeId, const std::string& nodeName, int32_t itemIndex)
    : DOMListItem(nodeId, nodeName, itemIndex)
{}

bool DOMListItemGroup::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    auto parent = AceType::DynamicCast<DOMList>(parentNode_.Upgrade());
    if (!parent) {
        LOGW("DOMListItemGroup parent is null");
        return false;
    }
    if (attr.first == DOM_LISTITEM_TYPE) {
        type_ = attr.second;
        return true;
    }
    if (attr.first == DOM_LISTITEM_CARD_TYPE) {
        isCard_ = StringToBool(attr.second);
        return true;
    }
    if (attr.first == DOM_LISTITEM_CARD_BLUR) {
        isCardBlur_ = StringToBool(attr.second);
        return true;
    }
    if (attr.first == DOM_ID) {
        // Do not return true, ID still need be handled outside.
        groupId_ = attr.second;
    }
    return false;
}

void DOMListItemGroup::SetCardThemeAttrs()
{
    cardTheme_ = GetTheme<CardTheme>();
    if (!cardTheme_) {
        LOGE("cardTheme is null");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }
    if (boxComponent_) {
        if (isCard_) {
            RefPtr<Decoration> backDecoration = boxComponent_->GetBackDecoration();
            if (!backDecoration) {
                RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
                decoration->SetBackgroundColor(cardTheme_->GetBackgroundColor());
                decoration->SetBorderRadius(Radius(cardTheme_->GetBorderRadius()));
                boxComponent_->SetBackDecoration(decoration);
            }
            if (backDecoration && !backDecoration->GetBorder().HasRadius()) {
                backDecoration->SetBorderRadius(Radius(cardTheme_->GetBorderRadius()));
            }
            if (backDecoration && (backDecoration->GetBackgroundColor() == Color::TRANSPARENT)) {
                backDecoration->SetBackgroundColor(cardTheme_->GetBackgroundColor());
            }
            if (isCardBlur_) {
                RefPtr<Decoration> frontDecoration = boxComponent_->GetFrontDecoration();
                if (!frontDecoration) {
                    RefPtr<Decoration> frontDecoration = AceType::MakeRefPtr<Decoration>();
                    frontDecoration->SetBlurRadius(cardTheme_->GetBlurRadius());
                    boxComponent_->SetFrontDecoration(frontDecoration);
                }
                if (frontDecoration && !frontDecoration->GetBlurRadius().IsValid()) {
                    frontDecoration->SetBlurRadius(cardTheme_->GetBlurRadius());
                }
            } else {
                RefPtr<Decoration> frontDecoration = boxComponent_->GetFrontDecoration();
                if (frontDecoration && frontDecoration->GetBlurRadius().IsValid()) {
                    frontDecoration->SetBlurRadius(Dimension());
                }
            }
        }
    }
}

bool DOMListItemGroup::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    static const LinearMapNode<void (*)(int32_t, DOMListItemGroup&)> eventOperators[] = {
        {
            DOM_LIST_ITEM_GROUP_EVENT_GROUPCLICK,
            [](int32_t pageId, DOMListItemGroup& listItemGroup) {
                listItemGroup.onClicked_ =
                    EventMarker(listItemGroup.GetNodeIdForEvent(), DOM_LIST_ITEM_GROUP_EVENT_GROUPCLICK, pageId);
            },
        },
        {
            DOM_LIST_ITEM_GROUP_EVENT_GROUPCOLLAPSE,
            [](int32_t pageId, DOMListItemGroup& listItemGroup) {
                listItemGroup.onCollapse_ =
                    EventMarker(listItemGroup.GetNodeIdForEvent(), DOM_LIST_ITEM_GROUP_EVENT_GROUPCOLLAPSE, pageId);
            },
        },
        {
            DOM_LIST_ITEM_GROUP_EVENT_GROUPEXPAND,
            [](int32_t pageId, DOMListItemGroup& listItemGroup) {
                listItemGroup.onExpand_ =
                    EventMarker(listItemGroup.GetNodeIdForEvent(), DOM_LIST_ITEM_GROUP_EVENT_GROUPEXPAND, pageId);
            },
        },
    };
    auto iter = BinarySearchFindIndex(eventOperators, ArraySize(eventOperators), event.c_str());
    if (iter != -1) {
        eventOperators[iter].value(pageId, *this);
        return true;
    }
    return false;
}

void DOMListItemGroup::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (child) {
        const auto& childComponent = child->GetRootComponent();
        listItemGroupComponent_->AppendChild(childComponent);
    }
}

void DOMListItemGroup::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (child) {
        const auto& childComponent = child->GetRootComponent();
        listItemGroupComponent_->RemoveChild(childComponent);
    }
}

void DOMListItemGroup::PrepareSpecializedComponent()
{
    if (!listItemGroupComponent_) {
        listItemGroupComponent_ = AceType::MakeRefPtr<ListItemGroupComponent>(type_);
    }

    ResetInitializedStyle();
    listItemGroupComponent_->SetGroupId(groupId_);
    listItemGroupComponent_->SetType(type_);
    listItemGroupComponent_->SetFlags(LIST_ITEM_FLAG_FROM_CHILD);
    listItemGroupComponent_->SetTopLeftRadius(topLeftRadius_);
    listItemGroupComponent_->SetTopRightRadius(topRightRadius_);
    listItemGroupComponent_->SetBottomLeftRadius(bottomLeftRadius_);
    listItemGroupComponent_->SetBottomRightRadius(bottomRightRadius_);
    listItemGroupComponent_->SetOnClicked(onClicked_);
    listItemGroupComponent_->SetOnCollapse(onCollapse_);
    listItemGroupComponent_->SetOnExpand(onExpand_);

    auto domList = AceType::DynamicCast<DOMList>(parentNode_.Upgrade());
    if (domList) {
        listItemGroupComponent_->SetDirection(domList->GetDirection());
    } else {
        LOGW("item group has a wrong parent");
    }
}

void DOMListItemGroup::ResetInitializedStyle()
{
    if (!listItemGroupComponent_) {
        LOGE("list item is null, reset style failed.");
        EventReport::SendComponentException(ComponentExcepType::LIST_ITEM_ERR);
        return;
    }
    RefPtr<FocusAnimationTheme> theme = GetTheme<FocusAnimationTheme>();
    if (theme) {
        listItemGroupComponent_->SetFocusAnimationColor(theme->GetColor());
    }
    if (isCard_ || (isCard_ && isCardBlur_)) {
        SetCardThemeAttrs();
    }
}

RefPtr<Component> DOMListItemGroup::GetSpecializedComponent()
{
    SetCardThemeAttrs();
    return listItemGroupComponent_;
}

RefPtr<Component> DOMListItemGroup::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    return DOMNode::CompositeSpecializedComponent(components);
}

void DOMListItemGroup::Update()
{
    auto pipelineContext = pipelineContext_.Upgrade();
    if (pipelineContext) {
        pipelineContext->ScheduleUpdate(GetRootComponent());
    }
}

int32_t DOMListItemGroup::GetItemIndex() const
{
    if (listItemGroupComponent_) {
        return listItemGroupComponent_->GetIndex();
    }
    return DEFAULT_NODE_INDEX;
}

} // namespace OHOS::Ace::Framework
