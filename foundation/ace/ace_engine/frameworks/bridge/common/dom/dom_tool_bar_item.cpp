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

#include "frameworks/bridge/common/dom/dom_tool_bar_item.h"

#include "base/i18n/localization.h"
#include "core/components/padding/padding_component.h"
#include "core/components/select/select_theme.h"
#include "frameworks/bridge/common/dom/dom_type.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t MAX_LINES = 2;
const char MORE[] = "common.more";

}

DOMToolBarItem::DOMToolBarItem(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    toolBarItemChild_ = AceType::MakeRefPtr<ToolBarItemComponent>();
    textChild_ = AceType::MakeRefPtr<TextComponent>("");
    imageChild_ = AceType::MakeRefPtr<ImageComponent>("");
}

void DOMToolBarItem::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMToolBarItem::ResetInitializedStyle()
{
    theme_ = GetTheme<ToolBarTheme>();
    if (!theme_) {
        LOGE("ToolBarTheme is null");
        return;
    }
    InitImageStyle();
    InitTextStyle();
    InitializedToolBarItemChild();
}

void DOMToolBarItem::InitImageStyle()
{
    const Size& iconSize = theme_->GetIconSize();
    imageChild_->SetWidth(Dimension(iconSize.Width(), DimensionUnit::VP));
    imageChild_->SetHeight(Dimension(iconSize.Height(), DimensionUnit::VP));
    imageChild_->SetMatchTextDirection(true);
}

void DOMToolBarItem::InitTextStyle()
{
    textStyle_ = theme_->GetToolBarTextStyle();
    textStyle_.SetTextAlign(TextAlign::CENTER);
    textStyle_.SetTextOverflow(TextOverflow::CLIP);
    textStyle_.SetMaxLines(MAX_LINES);
}

void DOMToolBarItem::InitializedToolBarItemChild()
{
    toolBarItemChild_->SetPressColor(theme_->GetPressColor());
    toolBarItemChild_->SetRadius(theme_->GetRadius());
    toolBarItemChild_->SetFocusColor(theme_->GetFocusColor());
    toolBarItemChild_->SetHoverColor(theme_->GetHoverColor());
    RefPtr<SelectTheme> selectTheme = GetTheme<SelectTheme>();
    if (selectTheme) {
        toolBarItemChild_->SetMenuMinWidth(selectTheme->GetPopupMinWidth());
    }
    if (!declaration_) {
        return;
    }
    declaration_->GetBackDecoration()->SetBackgroundColor(theme_->GetItemBackgroundColor());
    declaration_->SetHasDecorationStyle(true);
}

bool DOMToolBarItem::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // Operator map for attr
    static const LinearMapNode<void (*)(DOMToolBarItem&, const std::string&)> attrOperators[] = {
        { DOM_TOOL_BAR_ITEM_ICON,
            [](DOMToolBarItem& toolBarItem, const std::string& val) { toolBarItem.icon_ = std::move(val); } },
        { DOM_TOOL_BAR_ITEM_VALUE,
            [](DOMToolBarItem& toolBarItem, const std::string& val) { toolBarItem.value_ = std::move(val); } },
    };
    auto operatorIter = BinarySearchFindIndex(attrOperators, ArraySize(attrOperators), attr.first.c_str());
    if (operatorIter != -1) {
        LOGD("ToolBarItem attrs : %{public}s = %{public}s", attr.first.c_str(), attr.second.c_str());
        attrOperators[operatorIter].value(*this, attr.second);
        return true;
    }
    return false;
}

bool DOMToolBarItem::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // Operator map for style
    static const LinearMapNode<void (*)(DOMToolBarItem&, const std::string&)> styleOperators[] = {
        { DOM_TOOL_BAR_ITEM_ALLOW_SCALE,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetAllowScale(StringToBool(val));
            } },
        { DOM_TOOL_BAR_ITEM_COLOR,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetTextColor(Color::FromString(val));
            } },
        { DOM_TOOL_BAR_ITEM_FONT_FAMILY,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetFontFamilies(ConvertStrToFontFamilies(val));
            } },
        { DOM_TOOL_BAR_ITEM_FONT_SIZE,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetFontSize(StringToDimension(val));
            } },
        { DOM_TOOL_BAR_ITEM_FONT_STYLE,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetFontStyle(ConvertStrToFontStyle(val));
            } },
        { DOM_TOOL_BAR_ITEM_FONT_WEIGHT,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetFontWeight(ConvertStrToFontWeight(val));
            } },
        { DOM_TOOL_BAR_ITEM_TEXT_COLOR,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetTextColor(Color::FromString(val));
            } },
        { DOM_TOOL_BAR_ITEM_TEXT_DECORATION,
            [](DOMToolBarItem& toolBarItem, const std::string& val) {
                toolBarItem.textStyle_.SetTextDecoration(ConvertStrToTextDecoration(val));
            } },
    };
    auto operatorIter = BinarySearchFindIndex(styleOperators, ArraySize(styleOperators), style.first.c_str());
    if (operatorIter != -1) {
        LOGD("ToolBarItem styles : %{public}s = %{public}s", style.first.c_str(), style.second.c_str());
        styleOperators[operatorIter].value(*this, style.second);
        return true;
    }
    return false;
}

bool DOMToolBarItem::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    static const LinearMapNode<void (*)(DOMToolBarItem&, const EventMarker&)> toolBarItemEventOperators[] = {
        { DOM_CATCH_BUBBLE_CLICK,
            [](DOMToolBarItem& toolBarItem, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(true);
                toolBarItem.clickEventId_ = eventMarker;
            } },
        { DOM_CLICK,
            [](DOMToolBarItem& toolBarItem, const EventMarker& event) {
                EventMarker eventMarker(event);
                eventMarker.SetCatchMode(false);
                toolBarItem.clickEventId_ = eventMarker;
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(toolBarItemEventOperators, ArraySize(toolBarItemEventOperators), event.c_str());
    if (operatorIter != -1) {
        LOGD("ToolBarItem events : %{public}s", event.c_str());
        toolBarItemEventOperators[operatorIter].value(*this, EventMarker(GetNodeIdForEvent(), event, pageId));
        return true;
    }
    return false;
}

void DOMToolBarItem::PrepareSpecializedComponent()
{
    toolBarItemChild_->SetChild(nullptr);
    toolBarItemChild_->SetIsEndItem(isEndItem_);

    std::list<RefPtr<Component>> children;
    if (isEndItem_) {
        BuildEndItemComponent(children);
    } else {
        BuildCommonComponent(children);
        toolBarItemChild_->SetClickedEventId(clickEventId_);
    }

    if (children.empty()) {
        toolBarItemChild_->SetChild(nullptr);
        return;
    }
    auto column = AceType::MakeRefPtr<ColumnComponent>(FlexAlign::FLEX_START, FlexAlign::CENTER, children);
    if (static_cast<uint32_t>(icon_.empty()) ^ static_cast<uint32_t>(value_.empty())) {
        column->SetMainAxisAlign(FlexAlign::CENTER);
    }
    toolBarItemChild_->SetChild(column);
}

void DOMToolBarItem::BuildCommonComponent(std::list<RefPtr<Component>>& children)
{
    // Generate common icon
    if (!icon_.empty()) {
        if (!imageChild_) {
            imageChild_ = AceType::MakeRefPtr<ImageComponent>(icon_);
            InitImageStyle();
        }
        imageChild_->SetResourceId(InternalResource::ResourceId::NO_ID);
        imageChild_->SetSrc(icon_);
        imageChild_->SetImageFill(GetImageFill());
        children.emplace_back(SetPadding(imageChild_, Edge(theme_->GetIconEdge())));
    }

    // Generate common text
    if (!value_.empty()) {
        if (!textChild_) {
            textChild_ = AceType::MakeRefPtr<TextComponent>(value_);
            InitTextStyle();
        }
        textChild_->SetData(value_);
        textStyle_.SetAdaptMaxFontSize(textStyle_.GetFontSize());
        textChild_->SetFocusColor(textStyle_.GetTextColor());
        textChild_->SetTextStyle(textStyle_);
        children.emplace_back(SetPadding(textChild_, Edge(theme_->GetTextEdge())));
    }
}

void DOMToolBarItem::BuildEndItemComponent(std::list<RefPtr<Component>>& children)
{
    // Generate endItem icon
    if (!imageChild_) {
        imageChild_ = AceType::MakeRefPtr<ImageComponent>("");
        InitImageStyle();
    }
    imageChild_->SetSrc("");
    imageChild_->SetResourceId(InternalResource::ResourceId::IC_MORE);
    imageChild_->SetColor(theme_->GetIconColor());
    children.emplace_back(SetPadding(imageChild_, Edge(theme_->GetIconEdge())));

    // Generate endItem text
    if (!textChild_) {
        textChild_ = AceType::MakeRefPtr<TextComponent>("");
        InitTextStyle();
    }
    textChild_->SetData(Localization::GetInstance()->GetEntryLetters(MORE));
    textChild_->SetFocusColor(textStyle_.GetTextColor());
    textChild_->SetTextStyle(textStyle_);
    children.emplace_back(SetPadding(textChild_, Edge(theme_->GetTextEdge())));
}

const RefPtr<Component> DOMToolBarItem::SetPadding(const RefPtr<Component>& component, Edge&& edge)
{
    auto paddingComponent = AceType::MakeRefPtr<PaddingComponent>();
    paddingComponent->SetPadding(std::move(edge));
    paddingComponent->SetChild(component);

    return paddingComponent;
}

RefPtr<OptionComponent> DOMToolBarItem::BuildOptionComponent()
{
    RefPtr<OptionComponent> optionComponent;
    if (!value_.empty()) {
            RefPtr<TextComponent> textComponent = AceType::MakeRefPtr<TextComponent>(value_);
            if (!optionComponent) {
                optionComponent = AceType::MakeRefPtr<OptionComponent>();
            }
            optionComponent->SetText(textComponent);
            optionComponent->SetValue(value_);
            optionComponent->SetClickEventForToolBarItem(clickEventId_);
    }
    return optionComponent;
}

} // namespace OHOS::Ace::Framework
