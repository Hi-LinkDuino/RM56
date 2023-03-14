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

#include "frameworks/bridge/common/dom/dom_list_item.h"

#include "base/log/event_report.h"
#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "core/components/focus_animation/focus_animation_theme.h"
#include "frameworks/bridge/common/dom/dom_list.h"
#include "frameworks/bridge/common/dom/dom_list_item_group.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

DOMListItem::DOMListItem(NodeId nodeId, const std::string& nodeName, int32_t itemIndex) : DOMNode(nodeId, nodeName)
{
    itemIndex_ = itemIndex;
    listItemComponent_ = AceType::MakeRefPtr<ListItemComponent>(type_, RefPtr<Component>());
}

bool DOMListItem::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    auto parent = AceType::DynamicCast<DOMList>(parentNode_.Upgrade());
    if (!parent) {
        auto group = AceType::DynamicCast<DOMListItemGroup>(parentNode_.Upgrade());
        if (!group) {
            LOGE("DOMListItem parent is null");
            EventReport::SendComponentException(ComponentExcepType::LIST_ITEM_ERR);
            return false;
        } else {
            LOGD("DOMListItem parent is list item group");
        }
    } else {
        LOGD("DOMListItem parent is list");
    }

    if (attr.first == DOM_LISTITEM_TYPE) {
        type_ = attr.second;
        return true;
    } else if (attr.first == DOM_LISTITEM_CARD_TYPE) {
        isCard_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_LISTITEM_CARD_BLUR) {
        isCardBlur_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_LISTITEM_STICKY) {
        sticky_ = StringToBool(attr.second);
        if (attr.second == "normal" || attr.second == "true") {
            sticky_ = true;
            stickyMode_ = StickyMode::NORMAL;
        } else if (attr.second == "opacity") {
            sticky_ = true;
            stickyMode_ = StickyMode::OPACITY;
        } else {
            sticky_ = false;
            stickyMode_ = StickyMode::NONE;
        }
        return true;
    } else if (attr.first == DOM_LISTITEM_IS_TITLE) {
        isTitle_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_LISTITEM_CLICK_EFFECT) {
        clickEffect_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_LISTITEM_STICKY_RADIUS) {
        stickyRadius_ = ParseDimension(attr.second);
    } else if (attr.first == DOM_LISTITEM_INDEX_KEY) {
        indexKey_ = attr.second;
        return true;
    } else if (attr.first == DOM_LISTITEM_PRIMARY) {
        primary_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_LISTITEM_ACTIVE) {
        isActive_ = StringToBool(attr.second);
        return true;
    } else if (attr.first == DOM_LISTITEM_KEY) {
        key_ = StringUtils::StringToInt(attr.second);
        return true;
    }
    return false;
}

void DOMListItem::SetCardThemeAttrs()
{
    cardTheme_ = GetTheme<CardTheme>();
    if (!cardTheme_) {
        LOGE("cardTheme is null");
        EventReport::SendComponentException(ComponentExcepType::GET_THEME_ERR);
        return;
    }
    if (!boxComponent_) {
        LOGE("boxComponent is null");
        return;
    }

    if (!isCard_) {
        LOGW("it is not card");
        return;
    }
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
    SetCardTransitionEffect();
}

void DOMListItem::SetCardTransitionEffect()
{
    // set default style for card if user did not set clickSpringEffect
    if (!transformComponent_) {
        transformComponent_ = AceType::MakeRefPtr<TransformComponent>();
    }
    if (!declaration_) {
        return;
    }
    if (!declaration_->HasClickEffect()) {
        transformComponent_->SetClickSpringEffectType(ClickSpringEffectType::MEDIUM);
    }
    if (!declaration_->HasTransitionAnimation()) {
        transformComponent_->SetTransitionEffect(TransitionEffect::UNFOLD);
        if (listItemComponent_) {
            listItemComponent_->SetTransitionEffect(TransitionEffect::UNFOLD);
        }
    } else {
        listItemComponent_->SetTransitionEffect(transformComponent_->GetTransitionEffect());
    }
}

bool DOMListItem::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMListItem&)> listItemStyleOperators[] = {
        { DOM_ALIGN_ITEMS, [](const std::string& val,
                           DOMListItem& listItem) { listItem.flexCrossAlign_ = ConvertStrToFlexAlign(val); } },
        { DOM_ALIGN_SELF, [](const std::string& val,
                         DOMListItem& listItem) { listItem.alignSelf_ = ConvertStrToFlexAlign(val); } },
        { DOM_LISTITEM_CLICK_COLOR,
            [](const std::string& val, DOMListItem& listItem) { listItem.clickColor_ = listItem.ParseColor(val); } },
        { DOM_FLEX_DIRECTION, [](const std::string& val,
                              DOMListItem& listItem) { listItem.flexDirection_ = ConvertStrToFlexDirection(val); } },
        { DOM_JUSTIFY_CONTENT, [](const std::string& val,
                               DOMListItem& listItem) { listItem.flexMainAlign_ = ConvertStrToFlexAlign(val); } },
    };
    auto operatorIter = BinarySearchFindIndex(listItemStyleOperators,
        ArraySize(listItemStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        listItemStyleOperators[operatorIter].value(style.second, *this);
        return true;
    }
    static const LinearMapNode<void (*)(const std::string&, DOMListItem&)> listItemRadiusStyleOperators[] = {
        // Set border radius
        { DOM_BORDER_BOTTOM_LEFT_RADIUS,
            [](const std::string& val, DOMListItem& listItem) {
                listItem.bottomLeftRadius_ = Radius(listItem.ParseDimension(val));
            } },
        { DOM_BORDER_BOTTOM_RIGHT_RADIUS,
            [](const std::string& val, DOMListItem& listItem) {
                listItem.bottomRightRadius_ = Radius(listItem.ParseDimension(val));
            } },
        { DOM_BORDER_RADIUS,
            [](const std::string& val, DOMListItem& listItem) {
                listItem.topLeftRadius_ = Radius(listItem.ParseDimension(val));
                listItem.topRightRadius_ = Radius(listItem.ParseDimension(val));
                listItem.bottomLeftRadius_ = Radius(listItem.ParseDimension(val));
                listItem.bottomRightRadius_ = Radius(listItem.ParseDimension(val));
            } },
        { DOM_BORDER_TOP_LEFT_RADIUS,
            [](const std::string& val, DOMListItem& listItem) {
                listItem.topLeftRadius_ = Radius(listItem.ParseDimension(val));
            } },
        { DOM_BORDER_TOP_RIGHT_RADIUS,
            [](const std::string& val, DOMListItem& listItem) {
                listItem.topRightRadius_ = Radius(listItem.ParseDimension(val));
            } },
    };
    // The radius still needs to be set to the radius of the box component, so return false.
    auto radiusIter = BinarySearchFindIndex(listItemRadiusStyleOperators,
                            ArraySize(listItemRadiusStyleOperators), style.first.c_str());
    if (radiusIter != -1) {
        listItemRadiusStyleOperators[radiusIter].value(style.second, *this);
        return false;
    }

    if (style.first == DOM_LISTITEM_COLUMN_SPAN) {
        auto columnSpan = StringUtils::StringToInt(style.second);
        if (columnSpan <= 0) {
            LOGD("ListItem column-span must greater than 0");
            columnSpan = DEFAULT_COLUMN_SPAN;
        }
        columnSpan_ = columnSpan;
        return true;
    }
    return false;
}

bool DOMListItem::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    LOGD("DOMListItem AddEvent");
    if (event == DOM_LIST_ITEM_EVENT_STICKY) {
        stickyEventId_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        listItemComponent_->SetStickyEventId(stickyEventId_);
        return true;
    }

    if (event == DOM_CLICK || event == DOM_CATCH_BUBBLE_CLICK) {
        EventMarker eventMarker(GetNodeIdForEvent(), event, pageId);
        listItemComponent_->SetClickEventId(eventMarker);
        return true;
    }
    return false;
}

void DOMListItem::AddListItem(const RefPtr<DOMNode>& node, int32_t slot)
{
    const auto& childComponent = node->GetRootComponent();
    if (!flexComponent_) {
        flexComponent_ = AceType::MakeRefPtr<FlexComponent>(
            flexDirection_, flexMainAlign_, flexCrossAlign_, std::list<RefPtr<Component>>());
    }
    flexComponent_->InsertChild(slot, childComponent);
}

void DOMListItem::RemoveListItem(const RefPtr<DOMNode>& node)
{
    const auto& childComponent = node->GetRootComponent();
    if (flexComponent_) {
        flexComponent_->RemoveChild(childComponent);
    }
}

void DOMListItem::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (child) {
        AddListItem(child, slot);
    }
}

void DOMListItem::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (child) {
        RemoveListItem(child);
    }
}

void DOMListItem::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    if (!parentNode) {
        return;
    }
    if (parentNode->GetTag() == DOM_NODE_TAG_LIST) {
        const auto& parentNodeTmp = AceType::DynamicCast<DOMList>(parentNode);
        if (listItemComponent_ && parentNodeTmp) {
            listItemComponent_->SetSupportScale(parentNodeTmp->GetItemScale());
            listItemComponent_->SetSupportOpacity(parentNodeTmp->GetItemOpacity());
            // NeedVibrate means scroll and rotation all trigger vibrate.
            // RotationVibrate means only rotation trigger vibrate.
            listItemComponent_->MarkNeedVibrate(parentNodeTmp->NeedVibrate());
            listItemComponent_->MarkNeedRotationVibrate(parentNodeTmp->NeedRotationVibrate());
            SetDividerStyle(parentNode);
        }
    } else if (parentNode->GetTag() == DOM_NODE_TAG_LIST_ITEM_GROUP) {
        const auto& parentNodeTmp = AceType::DynamicCast<DOMListItemGroup>(parentNode);
        if (listItemComponent_ && parentNodeTmp) {
            listItemComponent_->SetSupportScale(false);
            // Divider style is set in DOMList
            SetDividerStyle(parentNode->GetParentNode());
        }
    } else {
        LOGW("list item parent is valid type.");
    }
}

void DOMListItem::SetDividerStyle(const RefPtr<DOMNode>& parentNode)
{
    auto parentList = AceType::DynamicCast<DOMList>(parentNode);
    if (parentList) {
        listItemComponent_->MarkNeedDivider(parentList->NeedDivider());
        listItemComponent_->SetDividerLength(parentList->GetDividerLength());
        listItemComponent_->SetDividerOrigin(parentList->GetDividerOrigin());
        listItemComponent_->SetDividerHeight(parentList->GetDividerHeight());
        auto dividerColor = parentList->GetDividerColor();
        RefPtr<ListTheme> listTheme = GetTheme<ListTheme>();
        if (dividerColor == Color::TRANSPARENT && listTheme) {
            dividerColor = listTheme->GetDividerColor();
        }
        listItemComponent_->SetDividerColor(dividerColor);
    }
}

void DOMListItem::ResetInitializedStyle()
{
    if (!listItemComponent_) {
        LOGE("list item is null, reset style failed.");
        EventReport::SendComponentException(ComponentExcepType::LIST_ITEM_ERR);
        return;
    }
    RefPtr<FocusAnimationTheme> theme = GetTheme<FocusAnimationTheme>();
    if (theme) {
        listItemComponent_->SetFocusAnimationColor(theme->GetColor());
    }
    if (isCard_ || (isCard_ && isCardBlur_)) {
        SetCardThemeAttrs();
    }

    if (SystemProperties::GetDeviceType() == DeviceType::TV && boxComponent_) {
        RefPtr<ListItemTheme> itemTheme = GetTheme<ListItemTheme>();
        if (itemTheme && declaration_) {
            Edge padding;
            auto& style = static_cast<CommonPaddingStyle&>(declaration_->GetStyle(StyleTag::COMMON_PADDING_STYLE));
            if (style.IsValid() && style.padding.IsEffective()) {
                return;
            }
            // Add theme padding to item when not set customized padding.
            double additionalPadding = itemTheme->GetItemPaddingInPercent();
            boxComponent_->SetPadding(Edge(), Edge(Dimension(additionalPadding, DimensionUnit::PERCENT)));
        }
    }
}

void DOMListItem::PrepareSpecializedComponent()
{
    if (!listItemComponent_) {
        listItemComponent_ = AceType::MakeRefPtr<ListItemComponent>(type_, RefPtr<Component>());
    }

    ResetInitializedStyle();
    listItemComponent_->SetType(type_.empty() ? "default" : type_);
    listItemComponent_->SetSticky(sticky_);
    listItemComponent_->SetStickyMode(stickyMode_);
    listItemComponent_->SetStickyRadius(stickyRadius_);
    listItemComponent_->SetPrimary(primary_);
    listItemComponent_->SetSupportClick(clickEffect_);
    listItemComponent_->MarkTitle(isTitle_);
    listItemComponent_->SetFlags(LIST_ITEM_FLAG_FROM_CHILD);
    listItemComponent_->SetColumnSpan(columnSpan_);
    listItemComponent_->SetTopLeftRadius(topLeftRadius_);
    listItemComponent_->SetTopRightRadius(topRightRadius_);
    listItemComponent_->SetBottomLeftRadius(bottomLeftRadius_);
    listItemComponent_->SetBottomRightRadius(bottomRightRadius_);
    if (clickColor_ != Color::TRANSPARENT) {
        listItemComponent_->SetClickColor(clickColor_);
    }
    listItemComponent_->SetAlignSelf(alignSelf_);
    if (!indexKey_.empty()) {
        listItemComponent_->SetIndexKey(indexKey_);
    }
    if (key_ != -1) {
        listItemComponent_->SetKey(key_);
    }
    if (flexComponent_) {
        flexComponent_->SetDirection(flexDirection_);
        flexComponent_->SetMainAxisAlign(flexMainAlign_);
        flexComponent_->SetCrossAxisAlign(flexCrossAlign_);
    } else {
        flexComponent_ = AceType::MakeRefPtr<FlexComponent>(
            flexDirection_, flexMainAlign_, flexCrossAlign_, std::list<RefPtr<Component>>());
        flexComponent_->SetTextDirection(IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR);
    }

    if (displayComponent_) {
        displayComponent_->SetVisible(VisibleType::VISIBLE);
        displayComponent_->SetOpacity(1.0);
    } else {
        displayComponent_ = AceType::MakeRefPtr<DisplayComponent>();
    }

    if (boxComponent_) {
        boxComponent_->SetMouseAnimationType(HoverAnimationType::OPACITY);
    }
}

RefPtr<Component> DOMListItem::GetSpecializedComponent()
{
    SetCardThemeAttrs();
    return listItemComponent_;
}

RefPtr<Component> DOMListItem::CompositeSpecializedComponent(const std::vector<RefPtr<SingleChild>>& components)
{
    RefPtr<Component> component;
    if (sticky_ && stickyMode_ == StickyMode::OPACITY) {
        RefPtr<DisplayComponent> opacity = AceType::MakeRefPtr<DisplayComponent>();
        opacity->SetVisible(VisibleType::VISIBLE);
        opacity->SetOpacity(1.0);
        RefPtr<TransformComponent> scale = AceType::MakeRefPtr<TransformComponent>();
        opacity->SetChild(scale);
        scale->SetChild(flexComponent_);
        component = opacity;
    } else {
        component = flexComponent_;
    }
    if (!components.empty()) {
        const auto& parent = components.back();
        if (parent) {
            parent->SetChild(component);
        }
        component = AceType::DynamicCast<Component>(components.front());
    }
    if (listItemComponent_) {
        if (displayComponent_) {
            displayComponent_->SetChild(component);
            listItemComponent_->SetChild(displayComponent_);
        } else {
            listItemComponent_->SetChild(component);
        }
    }
    if (declaration_) {
        declaration_->SetHasPositionProcessed(true);
    }
    return listItemComponent_;
}

NodeId DOMListItem::GetDirtyNodeId() const
{
    if (key_ == -1) {
        return DOMNode::GetNodeId();
    }
    return GetParentId();
}

} // namespace OHOS::Ace::Framework
