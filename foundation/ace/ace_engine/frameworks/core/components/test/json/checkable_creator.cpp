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

#include "core/components/test/json/checkable_creator.h"

#include "core/components/checkable/checkable_component.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

void ParseProperty(const JsonValue& componentJson, const RefPtr<CheckableComponent>& component)
{
    if (componentJson.Contains(CHECKABLE_ON_CHANGED_EVENT) &&
        componentJson.GetValue(CHECKABLE_ON_CHANGED_EVENT)->IsString()) {
        auto onChanged = componentJson.GetValue(CHECKABLE_VALUE)->GetString();
        component->SetChangeEvent(EventMarker(onChanged));
    }

    if (componentJson.Contains(WIDTH) && componentJson.GetValue(WIDTH)->IsNumber()) {
        component->SetWidth(Dimension(componentJson.GetValue(WIDTH)->GetDouble(), DimensionUnit::PX));
    }
    if (componentJson.Contains(HEIGHT) && componentJson.GetValue(HEIGHT)->IsNumber()) {
        component->SetHeight(Dimension(componentJson.GetValue(HEIGHT)->GetDouble(), DimensionUnit::PX));
    }
    if (componentJson.Contains(DISABLED) && componentJson.GetValue(DISABLED)->IsBool()) {
        component->SetDisabled(componentJson.GetValue(DISABLED)->GetBool());
    }
    auto pointColorJson = componentJson.GetValue(CHECKABLE_POINT_COLOR);
    if (pointColorJson && pointColorJson->IsObject()) {
        component->SetPointColor(ColorCreator::CreateFromJson(*pointColorJson));
    }
    auto activeColorJson = componentJson.GetValue(CHECKABLE_ACTIVE_COLOR);
    if (activeColorJson && activeColorJson->IsObject()) {
        component->SetActiveColor(ColorCreator::CreateFromJson(*activeColorJson));
    }
    auto inactiveColorJson = componentJson.GetValue(CHECKABLE_INACTIVE_COLOR);
    if (inactiveColorJson && inactiveColorJson->IsObject()) {
        component->SetInactiveColor(ColorCreator::CreateFromJson(*inactiveColorJson));
    }
    auto focusColorJson = componentJson.GetValue(CHECKABLE_FOCUS_COLOR);
    if (focusColorJson && focusColorJson->IsObject()) {
        component->SetFocusColor(ColorCreator::CreateFromJson(*focusColorJson));
    }
    if (componentJson.Contains(CHECKABLE_ON_CHANGED_EVENT) &&
        componentJson.GetValue(CHECKABLE_ON_CHANGED_EVENT)->IsString()) {
        EventMarker eventMarker(componentJson.GetValue(CHECKABLE_ON_CHANGED_EVENT)->GetString());
        component->SetChangeEvent(eventMarker);
    }
}

} // namespace

RefPtr<Component> CheckableCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson checkable");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    auto themeManager = AceType::MakeRefPtr<ThemeManager>();
    if (classType == CHECKBOX_NAME) {
        RefPtr<CheckboxTheme> theme =
            AceType::DynamicCast<CheckboxTheme>(themeManager->GetTheme(CheckboxTheme::TypeId()));
        RefPtr<CheckboxComponent> component = AceType::MakeRefPtr<CheckboxComponent>(theme);
        if (componentJson.Contains(CHECKABLE_VALUE) && componentJson.GetValue(CHECKABLE_VALUE)->IsBool()) {
            component->SetValue(componentJson.GetValue(CHECKABLE_VALUE)->GetBool());
        }
        ParseProperty(componentJson, component);
        return component;
    } else if (classType == SWITCH_NAME) {
        RefPtr<SwitchTheme> theme =
            AceType::DynamicCast<SwitchTheme>(themeManager->GetTheme(SwitchTheme::TypeId()));
        RefPtr<SwitchComponent> component = AceType::MakeRefPtr<SwitchComponent>(theme);
        if (componentJson.Contains(CHECKABLE_VALUE) && componentJson.GetValue(CHECKABLE_VALUE)->IsBool()) {
            component->SetValue(componentJson.GetValue(CHECKABLE_VALUE)->GetBool());
        }
        ParseProperty(componentJson, AceType::DynamicCast<CheckableComponent>(component));
        return component;
    } else if (classType == RADIO_NAME) {
        RefPtr<RadioTheme> theme =
            AceType::DynamicCast<RadioTheme>(themeManager->GetTheme(RadioTheme::TypeId()));
        RefPtr<RadioComponent<std::string>> component = AceType::MakeRefPtr<RadioComponent<std::string>>(theme);
        if (componentJson.Contains(CHECKABLE_VALUE) && componentJson.GetValue(CHECKABLE_VALUE)->IsString()) {
            component->SetValue(componentJson.GetValue(CHECKABLE_VALUE)->GetString());
        }
        if (componentJson.Contains(RADIO_GROUP_VALUE) && componentJson.GetValue(RADIO_GROUP_VALUE)->IsString()) {
            component->SetGroupValue(componentJson.GetValue(RADIO_GROUP_VALUE)->GetString());
        }
        ParseProperty(componentJson, component);
        return component;
    } else {
        LOGD("unknown class name: %{public}s", classType.c_str());
        return nullptr;
    }
}

} // namespace OHOS::Ace
