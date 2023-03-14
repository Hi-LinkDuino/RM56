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

#include "core/components/checkable/checkable_component.h"

#include "core/components/checkable/checkable_element.h"
#include "core/components/checkable/render_checkbox.h"
#include "core/components/checkable/render_radio.h"
#include "core/components/checkable/render_switch.h"

namespace OHOS::Ace {
namespace {

constexpr bool DEFAULT_SWITCH_VALUE = false;
constexpr bool DEFAULT_CHECKBOX_VALUE = false;

} // namespace

CheckableComponent::CheckableComponent(CheckableType type, const RefPtr<CheckableTheme>& theme) : checkableType_(type)
{
    ApplyTheme(theme);
}

void CheckableComponent::ApplyTheme(const RefPtr<CheckableTheme>& theme)
{
    if (theme) {
        width_ = theme->GetWidth();
        height_ = theme->GetHeight();
        hotZoneHorizontalPadding_ = theme->GetHotZoneHorizontalPadding();
        hotZoneVerticalPadding_ = theme->GetHotZoneVerticalPadding();
        aspectRatio_ = theme->GetAspectRatio();
        pointColor_ = theme->GetPointColor();
        activeColor_ = theme->GetActiveColor();
        inactiveColor_ = theme->GetInactiveColor();
        focusColor_ = theme->GetFocusColor();
        defaultWidth_ = theme->GetDefaultWidth();
        defaultHeight_ = theme->GetDefaultHeight();
        radioInnerSizeRatio_ = theme->GetRadioInnerSizeRatio();
        needFocus_ = theme->GetNeedFocus();
        backgroundSolid_ = theme->IsBackgroundSolid();
        hoverColor_ = theme->GetHoverColor();
        inactivePointColor_ = theme->GetInactivePointColor();
        shadowColor_ = theme->GetShadowColor();
        shadowWidth_ = theme->GetShadowWidth();
        hoverRadius_ = theme->GetHoverRadius();
        borderWidth_ = theme->GetBorderWidth();
    }
}

RefPtr<RenderNode> CheckableComponent::CreateRenderNode()
{
    if (checkableType_ == CheckableType::CHECKBOX) {
        return RenderCheckbox::Create();
    } else if (checkableType_ == CheckableType::SWITCH) {
        return RenderSwitch::Create();
    } else if (checkableType_ == CheckableType::RADIO) {
        return RenderRadio::Create();
    } else {
        LOGW("Unknown checkable type!");
        return nullptr;
    }
}

RefPtr<Element> CheckableComponent::CreateElement()
{
    return AceType::MakeRefPtr<CheckableElement>();
}

CheckboxComponent::CheckboxComponent(const RefPtr<CheckboxTheme>& theme)
    : CheckableComponent(CheckableType::CHECKBOX, theme), CheckableValue<bool>(DEFAULT_CHECKBOX_VALUE)
{}

SwitchComponent::SwitchComponent(const RefPtr<SwitchTheme>& theme)
    : CheckableComponent(CheckableType::SWITCH, theme), CheckableValue<bool>(DEFAULT_SWITCH_VALUE)
{
    if (theme) {
        backgroundSolid_ = theme->IsBackgroundSolid();
    }
}

} // namespace OHOS::Ace
