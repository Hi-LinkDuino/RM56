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

#include "frameworks/bridge/common/dom/dom_option.h"

#include "frameworks/bridge/common/dom/dom_reflect_map.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

const char MENU_OPTION_SHOW_TYPE[] = "show";
const char MENU_OPTION_POPUP_TYPE[] = "popup";

}

DOMOption::DOMOption(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    selectOptionComponent_ = AceType::MakeRefPtr<OptionComponent>();
    selectOptionComponent_->SetId(std::to_string(nodeId));
}

void DOMOption::InitializeStyle()
{
    ResetInitializedStyle();
}

void DOMOption::ResetInitializedStyle()
{
    selectOptionComponent_->InitTheme(GetThemeManager());
    theme_ = GetTheme<SelectTheme>();
    if (theme_) {
        selectOptionComponent_->SetClickedColor(theme_->GetClickedColor());
        selectOptionComponent_->SetSelectedColor(theme_->GetSelectedColor());
        selectOptionComponent_->SetSelectedBackgroundColor(theme_->GetSelectedColor());
        selectOptionComponent_->SetFontColor(theme_->GetFontColor());
        selectOptionComponent_->SetFontSize(theme_->GetFontSize());
        selectOptionComponent_->SetFontWeight(theme_->GetFontWeight());
        selectOptionComponent_->SetFontFamily(theme_->GetFontFamily());
        selectOptionComponent_->SetTextDecoration(theme_->GetTextDecoration());
        selectOptionComponent_->SetAllowScale(theme_->IsAllowScale());
    }
}

bool DOMOption::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    if (attr.first != DOM_OPTION_CONTENT) {
        attrs_.push_back(attr);
    }
#endif
    if (attr.first == DOM_OPTION_SELECTED) {
        selectOptionComponent_->SetSelected(StringToBool(attr.second));
        return true;
    }

    if (attr.first == DOM_OPTION_VALUE) {
        selectOptionComponent_->SetValue(attr.second);
        return true;
    }

    if (attr.first == DOM_OPTION_ICON) {
        if (!icon_) {
            icon_ = AceType::MakeRefPtr<ImageComponent>(attr.second);
        } else {
            icon_->SetSrc(attr.second);
        }
        selectOptionComponent_->SetIcon(icon_);
        return true;
    }

    if (attr.first == DOM_OPTION_ACTION) {
        if (attr.second == MENU_OPTION_SHOW_TYPE) {
            selectOptionComponent_->SetShowInNavigationBar(ShowInNavigationBar::SHOW);
        } else if (attr.second == MENU_OPTION_POPUP_TYPE) {
            selectOptionComponent_->SetShowInNavigationBar(ShowInNavigationBar::POPUP);
        }
        return true;
    }

    if (attr.first == DOM_OPTION_CONTENT) {
        if (!content_) {
            content_ = AceType::MakeRefPtr<TextComponent>(attr.second);
        } else {
            content_->SetData(attr.second);
        }
        selectOptionComponent_->SetText(content_);
        return true;
    }

    if (attr.first == DOM_DISABLED) {
        selectOptionComponent_->SetDisabled(StringToBool(attr.second));
        return true;
    }

    if (attr.first == DOM_SHOW) {
        selectOptionComponent_->SetVisible(StringToBool(attr.second));
        return true;
    }

    if (attr.first == DOM_FOCUSABLE) {
        selectOptionComponent_->SetFocusable(StringToBool(attr.second));
        return true;
    }

    return false;
}

bool DOMOption::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
    styles_.push_back(style);
#endif
    if (style.first == DOM_TEXTAREA_COLOR) {
        selectOptionComponent_->SetFontColor(ParseColor(style.second));
        return true;
    }

    if (style.first == DOM_TEXTAREA_FONT_SIZE) {
        selectOptionComponent_->SetFontSize(ParseDimension(style.second));
        return true;
    }

    if (style.first == DOM_TEXTAREA_FONT_WEIGHT) {
        selectOptionComponent_->SetFontWeight(ConvertStrToFontWeight(style.second));
        return true;
    }

    if (style.first == DOM_TEXTAREA_FONT_FAMILY) {
        selectOptionComponent_->SetFontFamily(style.second);
        return true;
    }

    if (style.first == DOM_OPTION_TEXT_DECORATION) {
        selectOptionComponent_->SetTextDecoration(ConvertStrToTextDecoration(style.second));
        return true;
    }

    if (style.first == DOM_TEXT_ALLOW_SCALE) {
        selectOptionComponent_->SetAllowScale(StringToBool(style.second));
        return true;
    }

    return false;
}

void DOMOption::PrepareSpecializedComponent()
{
    selectOptionComponent_->SetTextDirection((IsRightToLeft() ? TextDirection::RTL : TextDirection::LTR));
}

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
void DOMOption::OnSetStyleFinished()
{
    std::reverse(std::begin(attrs_), std::end(attrs_));
    std::reverse(std::begin(styles_), std::end(styles_));
    selectOptionComponent_->SetAttr(attrs_);
    selectOptionComponent_->SetStyle(styles_);
}
#endif
} // namespace OHOS::Ace::Framework
