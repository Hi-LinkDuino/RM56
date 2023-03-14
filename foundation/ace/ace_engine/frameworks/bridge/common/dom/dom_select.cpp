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

#include "frameworks/bridge/common/dom/dom_select.h"

#include "base/utils/linear_map.h"
#include "base/utils/utils.h"
#include "frameworks/bridge/common/dom/dom_option.h"
#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {

bool DOMSelect::IsParentNavigation() const
{
    auto parent = GetParentNode();
    if (!parent) {
        LOGE("parent of dom select is null.");
        return false;
    }
    return parent->GetTag() == DOM_NODE_TAG_NAVIGATION_BAR;
}

DOMSelect::DOMSelect(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    selectComponent_ = AceType::MakeRefPtr<SelectComponent>();
    tipText_ = AceType::MakeRefPtr<TextComponent>("");
    selectComponent_->SetTipText(tipText_);
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    selectComponent_->SetNodeId(nodeId);
#endif
}

void DOMSelect::ResetInitializedStyle()
{
    InitializeStyle();
}

void DOMSelect::InitializeStyle()
{
    if (!selectComponent_) {
        LOGE("selectComponent is null");
        return;
    }
    selectComponent_->InitTheme(GetThemeManager());
    theme_ = GetTheme<SelectTheme>();
    if (theme_ && selectComponent_) {
        selectComponent_->SetClickedColor(theme_->GetClickedColor());
        selectComponent_->SetDisabledColor(theme_->GetDisabledColor());
        selectComponent_->SetSelectedColor(theme_->GetSelectedColor());
        selectComponent_->SetOptionSize(theme_->GetOptionSize());
        selectComponent_->SetRRectSize(theme_->GetRRectSize());
        selectComponent_->SetPopupBorderWidth(theme_->GetPopupBorderWidth());
        selectComponent_->SetPopupShadowWidth(theme_->GetPopupShadowWidth());
        selectComponent_->SetFontFamily(theme_->GetFontFamily());
        selectComponent_->SetFontSize(theme_->GetFontSize());
        selectComponent_->SetAllowScale(theme_->IsAllowScale());
        selectComponent_->SetTextColor(theme_->GetFontColor());
        selectComponent_->SetFontWeight(FontWeight::W500);
        selectComponent_->SetTextDecoration(theme_->GetTextDecoration());
        selectComponent_->SetBackgroundColor(Color::TRANSPARENT);
    }
}

void DOMSelect::UpdateBoxSize(const CalcDimension& width, const CalcDimension& height)
{
    if (!selectComponent_) {
        return;
    }

    if (width.Unit() != DimensionUnit::PERCENT && height.Unit() != DimensionUnit::PERCENT) {
        selectComponent_->SetInnerSize(width, height);
        return;
    }

    if (width.Unit() == DimensionUnit::PERCENT && height.Unit() == DimensionUnit::PERCENT) {
        DOMNode::UpdateBoxSize(width, height);
        selectComponent_->SetInnerSize(100.0_pct, 100.0_pct);
        return;
    }

    if (width.Unit() == DimensionUnit::PERCENT) {
        DOMNode::UpdateBoxSize(width, height);
        selectComponent_->SetInnerSize(100.0_pct, height);
        return;
    }

    DOMNode::UpdateBoxSize(width, height);
    selectComponent_->SetInnerSize(100.0_pct, 100.0_pct);
}

void DOMSelect::UpdateBoxPadding(const Edge& padding)
{
    if (!selectComponent_) {
        return;
    }

    selectComponent_->SetInnerPadding(padding);
}

void DOMSelect::UpdateBoxBorder(const Border& border)
{
    if (!selectComponent_) {
        return;
    }

    selectComponent_->SetInnerBorder(border);
}

bool DOMSelect::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (event == DOM_CHANGE) {
        onChanged_ = EventMarker(GetNodeIdForEvent(), event, pageId);
        selectComponent_->SetOnChanged(onChanged_);
        return true;
    }
    return false;
}

bool DOMSelect::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    if (IsParentNavigation()) {
        // not support style setting for navigation.
        return true;
    }

    static const LinearMapNode<void (*)(const std::string&, const DOMSelect&, SelectComponent&)>
        selectStyleOperators[] = {
            { DOM_TEXT_ALLOW_SCALE, [](const std::string& val, const DOMSelect& node,
                                    SelectComponent& select) { select.SetAllowScale(StringToBool(val)); } },
            { DOM_BACKGROUND_COLOR, [](const std::string& val, const DOMSelect& node,
                                    SelectComponent& select) { select.SetBackgroundColor(node.ParseColor(val)); } },
            { DOM_TEXT_COLOR, [](const std::string& val, const DOMSelect& node,
                              SelectComponent& select) { select.SetTextColor(node.ParseColor(val)); } },
            { DOM_TEXT_FONT_FAMILY, [](const std::string& val, const DOMSelect& node,
                                    SelectComponent& select) { select.SetFontFamily(val); } },
            { DOM_TEXT_FONT_SIZE,
                [](const std::string& val, const DOMSelect& node, SelectComponent& select) {
                    select.SetFontSize(node.ParseDimension(val));
                    select.SetIsFontSetFlag(true);
                } },
            { DOM_TEXT_FONT_WEIGHT, [](const std::string& val, const DOMSelect& node,
                                    SelectComponent& select) { select.SetFontWeight(ConvertStrToFontWeight(val)); } },
            { DOM_TEXT_DECORATION,
                [](const std::string& val, const DOMSelect& node, SelectComponent& select) {
                    select.SetTextDecoration(ConvertStrToTextDecoration(val));
                } },
        };
    auto operatorIter =
        BinarySearchFindIndex(selectStyleOperators, ArraySize(selectStyleOperators), style.first.c_str());
    if (operatorIter != -1) {
        selectStyleOperators[operatorIter].value(style.second, *this, *selectComponent_);
        return true;
    }
    return false;
}

void DOMSelect::OnChildNodeAdded(const RefPtr<DOMNode>& child, int32_t slot)
{
    if (!child || child->GetTag() != DOM_NODE_TAG_OPTION) {
        LOGE("DOMSelect child is not correct");
        return;
    }

    auto option = AceType::DynamicCast<DOMOption>(child);
    if (option) {
        auto optionComponent = AceType::DynamicCast<OptionComponent>(option->GetSpecializedComponent());
        if (!optionComponent) {
            return;
        }
        if (!selectComponent_->GetSelectOptionCount() || optionComponent->GetSelected()) {
            auto text = optionComponent->GetText();
            if (text) {
                tipText_->SetData(text->GetData());
            } else {
                LOGE("text of option component is null.");
                tipText_->SetData("");
            }
        }
        if (slot < 0) {
            selectComponent_->AppendSelectOption(optionComponent);
        } else {
            selectComponent_->InsertSelectOption(optionComponent, static_cast<uint32_t>(slot));
        }
        LOGD("DOMSelect %{public}s, appendChild %{public}s", GetTag().c_str(), child->GetTag().c_str());
    }
}

void DOMSelect::OnChildNodeRemoved(const RefPtr<DOMNode>& child)
{
    if (!child || !selectComponent_) {
        return;
    }

    auto option = AceType::DynamicCast<OptionComponent>(child->GetSpecializedComponent());
    if (!option) {
        return;
    }

    selectComponent_->RemoveSelectOption(option);
}

void DOMSelect::PrepareSpecializedComponent()
{
    if (selectComponent_) {
        selectComponent_->SetTextDirection((IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR));
    }
}

} // namespace OHOS::Ace::Framework
