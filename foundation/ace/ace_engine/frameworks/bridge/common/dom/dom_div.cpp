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

#include "frameworks/bridge/common/dom/dom_div.h"

#include "base/log/event_report.h"
#include "core/common/ace_application_info.h"
#include "core/components/focus_collaboration/focus_collaboration_component.h"
#include "core/components/scroll/scroll_bar_theme.h"
#include "core/components/scroll/scroll_fade_effect.h"
#include "core/components/scroll/scroll_spring_effect.h"
#include "core/components/theme/theme_manager.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

const Alignment ALIGN_ARRAY[3][3] = { { Alignment::TOP_LEFT, Alignment::TOP_CENTER, Alignment::TOP_RIGHT },
    { Alignment::CENTER_LEFT, Alignment::CENTER, Alignment::CENTER_RIGHT },
    { Alignment::BOTTOM_LEFT, Alignment::BOTTOM_CENTER, Alignment::BOTTOM_RIGHT } };

} // namespace

DOMDiv::DOMDiv(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}

void DOMDiv::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    ACE_DCHECK(child);
    if (GetDisplay() == DisplayType::GRID) {
        if (!grid_) {
            LOGE("DOMDiv GridLayout is null");
            return;
        }
        grid_->InsertChild(slot, child->GetRootComponent());
    } else {
        if (isFlexWrap_) {
            if (!wrapChild_) {
                LOGE("DOMDiv wrapChild is null");
                return;
            }
            wrapChild_->InsertChild(slot, child->GetRootComponent());
        } else {
            if (!flexChild_) {
                LOGE("DOMDiv FlexChild is null");
                return;
            }
            flexChild_->InsertChild(slot, child->GetRootComponent());
        }
    }
}

void DOMDiv::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (GetDisplay() == DisplayType::GRID) {
        if (!grid_) {
            LOGE("DOMDiv GridLayout is null");
            return;
        }
        grid_->RemoveChild(child->GetRootComponent());
    } else {
        if (isFlexWrap_) {
            if (!wrapChild_) {
                LOGE("DOMDiv wrapChild is null");
                return;
            }
            wrapChild_->RemoveChild(child->GetRootComponent());
        } else {
            if (!flexChild_) {
                LOGE("DOMDiv FlexChild is null");
                return;
            }
            flexChild_->RemoveChild(child->GetRootComponent());
        }
    }
}

Alignment DOMDiv::ComputeFlexAlign(FlexAlign flexMainAlign, FlexAlign flexCrossAlign, bool isColumn, bool isRtl)
{
    AxisAlign mainAlign;
    if (flexMainAlign == FlexAlign::FLEX_END) {
        mainAlign = AxisAlign::END;
    } else if (flexMainAlign == FlexAlign::CENTER || flexMainAlign == FlexAlign::SPACE_AROUND) {
        mainAlign = AxisAlign::CENTER;
    } else {
        mainAlign = AxisAlign::START;
    }

    AxisAlign crossAlign;
    if (flexCrossAlign == FlexAlign::FLEX_END) {
        crossAlign = isRtl && isColumn ? AxisAlign::START : AxisAlign::END;
    } else if (flexCrossAlign == FlexAlign::CENTER) {
        crossAlign = AxisAlign::CENTER;
    } else {
        crossAlign = isRtl && isColumn ? AxisAlign::END : AxisAlign::START;
    }

    return isColumn ? ALIGN_ARRAY[static_cast<int32_t>(mainAlign)][static_cast<int32_t>(crossAlign)]
                    : ALIGN_ARRAY[static_cast<int32_t>(crossAlign)][static_cast<int32_t>(mainAlign)];
}

void DOMDiv::CreateOrUpdateGrid()
{
    if (!grid_) {
        grid_ = AceType::MakeRefPtr<GridLayoutComponent>(std::list<RefPtr<Component>>());
    }
    if (boxWrap_) {
        grid_->SetDirection(direction_ == DOM_FLEX_ROW ? FlexDirection::COLUMN : FlexDirection::ROW);
    } else {
        grid_->SetDirection(direction_ == DOM_FLEX_COLUMN ? FlexDirection::COLUMN : FlexDirection::ROW);
    }
    grid_->SetColumnsArgs(columnsArgs_);
    grid_->SetRowsArgs(rowsArgs_);
    grid_->SetUseScroll(false);
    grid_->SetColumnGap(columnGap_);
    grid_->SetRowGap(rowGap_);
    grid_->SetRightToLeft(IsRightToLeft());
}

void DOMDiv::CreateOrUpdateGridItem()
{
    if (!gridItem_) {
        gridItem_ = AceType::MakeRefPtr<GridLayoutItemComponent>(RefPtr<Component>());
    }
    gridItem_->SetRowIndex(rowStart_);
    gridItem_->SetColumnIndex(columnStart_);
    gridItem_->SetRowSpan(rowEnd_ - rowStart_ + 1);
    gridItem_->SetColumnSpan(columnEnd_ - columnStart_ + 1);
}

void DOMDiv::CreateOrUpdateFlex()
{
    auto direction = FlexDirectionMap.find(direction_);
    if (direction != FlexDirectionMap.end()) {
        flexDirection_ = direction->second;
    }
    auto flexMainAlign = FlexAlign::FLEX_START;
    auto justifyContent = FlexJustifyContentMap.find(justifyContent_);
    if (justifyContent != FlexJustifyContentMap.end()) {
        flexMainAlign = justifyContent->second;
    }

    auto flexCrossAlign = FlexAlign::STRETCH;
    auto alignItems = FlexAlignItemsMap.find(alignItems_);
    if (alignItems != FlexAlignItemsMap.end()) {
        flexCrossAlign = alignItems->second;
    }
    LOGD("DOMDiv GetFlexAlign end ,Direction:%{public}d, flexMainAlign:%{public}d, flexCrossAlign:%{public}d",
        flexDirection_, flexMainAlign, flexCrossAlign);
    if (!flexChild_) {
        flexChild_ = AceType::MakeRefPtr<FlexComponent>(
            flexDirection_, flexMainAlign, flexCrossAlign, std::list<RefPtr<Component>>());
    } else {
        flexChild_->SetDirection(flexDirection_);
        flexChild_->SetMainAxisAlign(flexMainAlign);
        flexChild_->SetCrossAxisAlign(flexCrossAlign);
    }
    if (IsRightToLeft()) {
        textDirection_ = TextDirection::RTL;
    } else {
        textDirection_ = TextDirection::LTR;
    }
    flexChild_->SetTextDirection(textDirection_);
    flexChild_->SetMainAxisAlign(flexMainAlign);
    flexChild_->SetCrossAxisAlign(flexCrossAlign);
    if (boxWrap_) {
        flexChild_->SetMainAxisSize(MainAxisSize::MIN);
    }
    bool isColumn = flexDirection_ == FlexDirection::COLUMN;
    boxComponent_->SetAlignment(ComputeFlexAlign(flexMainAlign, flexCrossAlign, isColumn, IsRightToLeft()));
    SetRootBoxHeight();
    if (!boxWrap_) {
        SetFlexHeight(flexMainAlign);
    }
    if ((flexDirection_ == FlexDirection::ROW && boxComponent_->GetWidthDimension().IsValid()) ||
        (flexDirection_ == FlexDirection::COLUMN && boxComponent_->GetHeightDimension().IsValid())) {
        flexChild_->SetMainAxisSize(MainAxisSize::MAX);
    }
    // When cross size is determined by developers, the flex cross size should be as large as the box.
    // Otherwise, flex cross size is as large as the max child's size.
    if ((flexDirection_ == FlexDirection::ROW && boxComponent_->GetHeightDimension().IsValid()) ||
        (flexDirection_ == FlexDirection::COLUMN && boxComponent_->GetWidthDimension().IsValid())) {
        flexChild_->SetStretchToParent(!boxWrap_);
        flexChild_->SetCrossAxisSize(CrossAxisSize::MAX);
    }
    SetSpecializedOverflow();
}

void DOMDiv::CreateOrUpdateWrap()
{
    if (!wrapChild_) {
        wrapChild_ = AceType::MakeRefPtr<WrapComponent>(spacing_, contentSpacing_, std::list<RefPtr<Component>>());
    }

    auto wrapDirection = WrapDirection::HORIZONTAL;
    auto direction = WrapDirectionMap.find(direction_);
    if (direction != WrapDirectionMap.end()) {
        wrapDirection = direction->second;
    }
    wrapChild_->SetDirection(wrapDirection);
    SetBoxWidthFlex(wrapDirection == WrapDirection::HORIZONTAL);

    auto wrapMainAlign = WrapAlignment::START;
    auto justifyContent = WrapJustifyContentMap.find(justifyContent_);
    if (justifyContent != WrapJustifyContentMap.end()) {
        wrapMainAlign = justifyContent->second;
    }
    wrapChild_->SetMainAlignment(wrapMainAlign);

    auto wrapCrossAlign = WrapAlignment::STRETCH;
    auto alignItems = WrapAlignItemsMap.find(alignItems_);
    if (alignItems != WrapAlignItemsMap.end()) {
        wrapCrossAlign = alignItems->second;
    }
    wrapChild_->SetCrossAlignment(wrapCrossAlign);
    auto wrapAlignContent = WrapAlignment::START;
    auto alignContent = WrapAlignContentMap.find(alignContent_);
    if (alignContent != WrapAlignContentMap.end()) {
        wrapAlignContent = alignContent->second;
    }
    wrapChild_->SetAlignment(wrapAlignContent);

    if (IsRightToLeft()) {
        textDirection_ = TextDirection::RTL;
    } else {
        textDirection_ = TextDirection::LTR;
    }
    wrapChild_->SetTextDirection(textDirection_);

    LOGD("DOMDiv GetWrapAlign end ,Direction:%{public}d, flexMainAlign:%{public}d, flexCrossAlign:%{public}d, "
         "AlignContent :%{public}d",
        wrapDirection, wrapMainAlign, wrapCrossAlign, wrapAlignContent);
    // final set box default alignment.
    boxComponent_->SetAlignment(IsRightToLeft() ? Alignment::TOP_RIGHT : Alignment::TOP_LEFT);
}

// If not set div width, Div width should fill the row width
void DOMDiv::SetBoxWidthFlex(bool isHorizontal) const
{
    // There is no custom width and the orientation is horizontal.
    if (boxComponent_->GetWidthDimension().Value() < 0.0 && isHorizontal) {
        boxComponent_->SetFlex(BoxFlex::FLEX_X);
    }
}

void DOMDiv::SetSpecializedOverflow()
{
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));
    if (overflowStyle.IsValid() && flexChild_) {
        flexChild_->SetOverflow(overflowStyle.overflow);
    }
}

// If not set div height, The root node(id=0) should be fill the column height
void DOMDiv::SetRootBoxHeight() const
{
    // not the root node 0 or Height != 0
    if (boxWrap_ || (!isRootNode_) || GreatOrEqual(boxComponent_->GetHeightDimension().Value(), 0.0)) {
        return;
    }
    if (boxComponent_->GetWidthDimension().IsValid()) {
        boxComponent_->SetFlex(BoxFlex::FLEX_Y);
        return;
    }
    auto context = GetPipelineContext().Upgrade();
    if (context && (context->GetWindowModal() == WindowModal::SEMI_MODAL ||
                       context->GetWindowModal() == WindowModal::DIALOG_MODAL)) {
        boxComponent_->SetFlex(BoxFlex::FLEX_X);
    } else {
        boxComponent_->SetFlex(BoxFlex::FLEX_XY);
        flexChild_->SetCrossAxisSize(CrossAxisSize::MAX);
    }
}

// If div and div parent direction is column,Set div height filed by his children
void DOMDiv::SetFlexHeight(FlexAlign flexMainAlign)
{
    auto parent = AceType::DynamicCast<DOMDiv>(parentNode_.Upgrade());
    if (!parent) {
        return;
    }
    flexChild_->SetMainAxisSize(MainAxisSize::MAX);
    if (flexMainAlign == FlexAlign::SPACE_BETWEEN || flexMainAlign == FlexAlign::SPACE_AROUND ||
        flexMainAlign == FlexAlign::SPACE_EVENLY) {
        return;
    }
    // When parent and child are all column, child should be wrap-content to fit frontend standard.
    // In this case, the alignment is calculated by boxComponent_.
    if (parent->flexDirection_ == flexDirection_) {
        flexChild_->SetMainAxisSize(MainAxisSize::MIN);
        flexChild_->SetStretchToParent(flexDirection_ == FlexDirection::COLUMN);
    }
}

void DOMDiv::OnMounted(const RefPtr<DOMNode>& parentNode)
{
    if (!declaration_) {
        return;
    }
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));

    // overflowFlag means that default tabcontent, dialog and panel support scroll.
    auto overflowFlag = !parentNode->HasOverflowStyle() && !declaration_->HasOverflowStyle();
    if (parentNode->GetTag() == DOM_NODE_TAG_TAB_CONTENT && direction_ == DOM_FLEX_COLUMN && overflowFlag) {
        auto child = boxComponent_->GetChild();
        scroll_ = AceType::MakeRefPtr<ScrollComponent>(child);
        scroll_->SetOnReachStart(onReachStart_);
        scroll_->SetOnReachEnd(onReachEnd_);
        scroll_->SetOnReachTop(onReachTop_);
        scroll_->SetOnReachBottom(onReachBottom_);
        if (overflowStyle.IsValid()) {
            scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor,
                overflowStyle.scrollBarWidth, overflowStyle.edgeEffect);
        }
        boxComponent_->SetChild(scroll_);
        if (flexChild_) {
            flexChild_->SetUseViewPortFlag(true);
        }
        rootComponent_->MarkNeedUpdate();
    }

    if (parentNode->GetTag() == DOM_NODE_TAG_REFRESH && flexDirection_ == FlexDirection::COLUMN) {
        if (flexChild_) {
            flexChild_->SetStretchToParent(flexDirection_ == FlexDirection::COLUMN);
        }
    }

    if (parentNode->GetTag() == DOM_NODE_TAG_DIALOG && direction_ == DOM_FLEX_COLUMN && overflowFlag) {
        if (flexChild_) {
            flexChild_->SetMainAxisSize(MainAxisSize::MIN);
        }
        boxComponent_->SetFlex(BoxFlex::FLEX_X);
        // dialog child should be scrollable
        auto child = rootComponent_->GetChild();
        scroll_ = AceType::MakeRefPtr<ScrollComponent>(child);
        if (overflowStyle.IsValid()) {
            scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor,
                overflowStyle.scrollBarWidth, overflowStyle.edgeEffect);
        }
        // use takeBoundary to expand the size of dialog
        scroll_->SetTakeBoundary(false);
        rootComponent_->SetChild(scroll_);
    }

    if (parentNode->GetTag() == DOM_NODE_TAG_PANEL && direction_ == DOM_FLEX_COLUMN && overflowFlag) {
        auto child = rootComponent_->GetChild();
        scroll_ = AceType::MakeRefPtr<ScrollComponent>(child);
        if (overflowStyle.IsValid()) {
            scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor,
                overflowStyle.scrollBarWidth, overflowStyle.edgeEffect);
        }
        rootComponent_->SetChild(scroll_);
    }
}

bool DOMDiv::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    static const LinearMapNode<void (*)(const std::string&, DOMDiv&)> styleSetters[] {
        { DOM_ALIGN_CONTENT, [](const std::string& value, DOMDiv& div) { div.alignContent_ = value; } },
        { DOM_ALIGN_ITEMS, [](const std::string& value, DOMDiv& div) { div.alignItems_ = value; } },
        { DOM_FLEX_DIRECTION, [](const std::string& value, DOMDiv& div) { div.direction_ = value; } },
        { DOM_FLEX_WRAP, [](const std::string& value, DOMDiv& div) { div.isFlexWrap_ = value == DOM_WRAP; } },
        { DOM_GRID_AUTO_FLOW, [](const std::string& value, DOMDiv& div) { div.direction_ = value; } },
        { DOM_GRID_COLUMN_END, [](const std::string& value, DOMDiv& div) { div.columnEnd_ = StringToInt(value); } },
        { DOM_GRID_COLUMN_START, [](const std::string& value, DOMDiv& div) { div.columnStart_ = StringToInt(value); } },
        { DOM_GRID_COLUMN_GAP,
            [](const std::string& value, DOMDiv& div) { div.columnGap_ = StringToDimension(value); } },
        { DOM_GRID_ROW_END, [](const std::string& value, DOMDiv& div) { div.rowEnd_ = StringToInt(value); } },
        { DOM_GRID_ROW_START, [](const std::string& value, DOMDiv& div) { div.rowStart_ = StringToInt(value); } },
        { DOM_GRID_ROW_GAP, [](const std::string& value, DOMDiv& div) { div.rowGap_ = StringToDimension(value); } },
        { DOM_GRID_TEMPLATE_COLUMNS, [](const std::string& value, DOMDiv& div) { div.columnsArgs_ = value; } },
        { DOM_GRID_TEMPLATE_ROWS, [](const std::string& value, DOMDiv& div) { div.rowsArgs_ = value; } },
        { DOM_JUSTIFY_CONTENT, [](const std::string& value, DOMDiv& div) { div.justifyContent_ = value; } },
    };
    auto operatorIter = BinarySearchFindIndex(styleSetters, ArraySize(styleSetters), style.first.c_str());
    if (operatorIter != -1) {
        styleSetters[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

bool DOMDiv::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (attr.first == DOM_DIV_CARD_TYPE) {
        isCard_ = StringToBool(attr.second);
        return true;
    }
    if (attr.first == DOM_DIV_CARD_BLUR) {
        isCardBlur_ = StringToBool(attr.second);
        return true;
    }
    return false;
}

bool DOMDiv::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    // static linear map must be sorted by key.
    static const LinearMapNode<void (*)(int32_t, DOMDiv&)> eventOperators[] = {
        {
            DOM_DIV_EVENT_REACH_BOTTOM,
            [](int32_t pageId, DOMDiv& div) {
                div.onReachBottom_ = EventMarker(div.GetNodeIdForEvent(), DOM_DIV_EVENT_REACH_BOTTOM, pageId);
            },
        },
        {
            DOM_DIV_EVENT_REACH_END,
            [](int32_t pageId, DOMDiv& div) {
                div.onReachEnd_ = EventMarker(div.GetNodeIdForEvent(), DOM_DIV_EVENT_REACH_END, pageId);
            },
        },
        {
            DOM_DIV_EVENT_REACH_START,
            [](int32_t pageId, DOMDiv& div) {
                div.onReachStart_ = EventMarker(div.GetNodeIdForEvent(), DOM_DIV_EVENT_REACH_START, pageId);
            },
        },
        {
            DOM_DIV_EVENT_REACH_TOP,
            [](int32_t pageId, DOMDiv& div) {
                div.onReachTop_ = EventMarker(div.GetNodeIdForEvent(), DOM_DIV_EVENT_REACH_TOP, pageId);
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

void DOMDiv::SetCardThemeAttrs()
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
            RefPtr<Decoration> frontDecoration = boxComponent_->GetFrontDecoration();
            if (isCardBlur_) {
                if (!frontDecoration) {
                    frontDecoration = AceType::MakeRefPtr<Decoration>();
                    frontDecoration->SetBlurRadius(cardTheme_->GetBlurRadius());
                    boxComponent_->SetFrontDecoration(frontDecoration);
                }
                if (frontDecoration && !frontDecoration->GetBlurRadius().IsValid()) {
                    frontDecoration->SetBlurRadius(cardTheme_->GetBlurRadius());
                }
            } else {
                if (frontDecoration && frontDecoration->GetBlurRadius().IsValid()) {
                    frontDecoration->SetBlurRadius(Dimension());
                }
            }
        }
    }
}

RefPtr<Component> DOMDiv::GetSpecializedComponent()
{
    SetCardThemeAttrs();
    auto parentNode = GetParentNode();
    if (parentNode && parentNode->GetDisplay() == DisplayType::GRID) {
        return gridItem_;
    } else {
        if (isFlexWrap_) {
            return wrapChild_;
        } else {
            return flexChild_;
        }
    }
}

void DOMDiv::PrepareSpecializedComponent()
{
    RefPtr<ComponentGroup> layoutChild;
    if (isFlexWrap_) {
        CreateOrUpdateWrap();
        layoutChild = wrapChild_;
    } else {
        CreateOrUpdateFlex();
        layoutChild = flexChild_;
    }

    if (GetDisplay() == DisplayType::GRID) {
        // Self is grid, node: flex/wrap -> grid
        CreateOrUpdateGrid();
        layoutChild->ClearChildren();
        layoutChild->AppendChild(grid_);
    }
    if (GetParentNode() && GetParentNode()->GetDisplay() == DisplayType::GRID) {
        // Parent is grid, node: gridItem -> flex/wrap.
        CreateOrUpdateGridItem();
        gridItem_->SetChild(flexChild_);
    }
}

void DOMDiv::CompositeComponents()
{
    DOMNode::CompositeComponents();

    if (!declaration_) {
        return;
    }
    auto& overflowStyle = static_cast<CommonOverflowStyle&>(declaration_->GetStyle(StyleTag::COMMON_OVERFLOW_STYLE));
    if (!overflowStyle.IsValid()) {
        return;
    }

    scroll_.Reset();
    // root div is scrollable
    bool isRootScroll =
        isRootNode_ && (!declaration_->HasOverflowStyle() || overflowStyle.overflow == Overflow::SCROLL);
    if (isRootScroll) {
        auto child = rootComponent_->GetChild();
        auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
        focusCollaboration->InsertChild(0, child);
        bool isCard = AceApplicationInfo::GetInstance().GetIsCardType();
        if (isCard) {
            rootComponent_->SetChild(focusCollaboration);
        } else if (direction_ == DOM_FLEX_COLUMN) {
            scroll_ = AceType::MakeRefPtr<ScrollComponent>(focusCollaboration);
            scroll_->SetAxisDirection(Axis::VERTICAL);
            scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor,
                overflowStyle.scrollBarWidth, overflowStyle.edgeEffect);
            rootComponent_->SetChild(scroll_);
        } else if (direction_ == DOM_FLEX_ROW) {
            scroll_ = AceType::MakeRefPtr<ScrollComponent>(focusCollaboration);
            scroll_->SetAxisDirection(Axis::HORIZONTAL);
            scroll_->SetEnable(false);
            scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor,
                overflowStyle.scrollBarWidth, overflowStyle.edgeEffect);
            rootComponent_->SetChild(scroll_);
        } else {
            rootComponent_->SetChild(focusCollaboration);
        }

        if (flexChild_) {
            flexChild_->SetUseViewPortFlag(true);
            if ((flexDirection_ == FlexDirection::ROW && boxComponent_->GetWidthDimension().IsValid()) ||
                (flexDirection_ == FlexDirection::COLUMN && boxComponent_->GetHeightDimension().IsValid())) {
                flexChild_->SetMainAxisSize(MainAxisSize::MAX);
            } else {
                flexChild_->SetMainAxisSize(MainAxisSize::MIN);
            }
        }
    } else if (isRootNode_) {
        auto child = rootComponent_->GetChild();
        auto focusCollaboration = AceType::MakeRefPtr<FocusCollaborationComponent>();
        focusCollaboration->InsertChild(0, child);
        rootComponent_->SetChild(focusCollaboration);
    }
    if (!isRootNode_ && overflowStyle.overflow == Overflow::SCROLL) {
        auto child = boxComponent_->GetChild();
        scroll_ = AceType::MakeRefPtr<ScrollComponent>(child);
        scroll_->SetAxisDirection(direction_ == DOM_FLEX_COLUMN ? Axis::VERTICAL : Axis::HORIZONTAL);
        scroll_->InitScrollBar(GetTheme<ScrollBarTheme>(), overflowStyle.scrollBarColor, overflowStyle.scrollBarWidth,
            overflowStyle.edgeEffect);
        boxComponent_->SetChild(scroll_);
    }
    if (scroll_ != nullptr) {
        scroll_->SetOnReachStart(onReachStart_);
        scroll_->SetOnReachEnd(onReachEnd_);
        scroll_->SetOnReachTop(onReachTop_);
        scroll_->SetOnReachBottom(onReachBottom_);
    }
}

void DOMDiv::AdjustSpecialParamInLiteMode()
{
    if (alignItems_ == DOM_ALIGN_ITEMS_STRETCH) {
        alignItems_ = DOM_ALIGN_ITEMS_START;
    }
}

} // namespace OHOS::Ace::Framework
