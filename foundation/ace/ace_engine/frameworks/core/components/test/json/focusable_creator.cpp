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

#include "focusable_creator.h"

#include "core/components/focusable/focusable_component.h"
#include "core/components/test/json/box_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> FocusableCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory& factory)
{
    auto children = factory.CreateChildComponent(json);
    auto child = children.empty() ? nullptr : children.front();

    auto boxStyleJson = json.GetValue(BOX_STYLE);
    if (!boxStyleJson || !boxStyleJson->IsObject()) {
        LOGE("Failed to get box style from json");
        return nullptr;
    }

    auto boxStyle = AceType::DynamicCast<BoxComponent>(BoxCreator::CreateFromJson(*boxStyleJson, factory));
    if (!boxStyle) {
        LOGE("Failed to create box style from json");
        return nullptr;
    }

    boxStyleJson = json.GetValue(FOCUSED_BOX_STYLE);
    if (!boxStyleJson || !boxStyleJson->IsObject()) {
        LOGE("Failed to get focused box style from json");
        return nullptr;
    }

    auto focusedBoxStyle = AceType::DynamicCast<BoxComponent>(BoxCreator::CreateFromJson(*boxStyleJson, factory));
    if (!focusedBoxStyle) {
        LOGE("Failed to create focused box style from json");
        return nullptr;
    }

    auto focusableComponent = AceType::MakeRefPtr<FocusableComponent>(child);
    focusableComponent->SetOnClickId(EventMarker(json.GetString(ON_CLICK_EVENT)));
    focusableComponent->SetOnFocusId(EventMarker(json.GetString(ON_FOCUS_EVENT)));
    focusableComponent->SetOnBlurId(EventMarker(json.GetString(ON_BLUR_EVENT)));
    focusableComponent->SetOnKeyId(EventMarker(json.GetString(ON_KEY_EVENT)));
    focusableComponent->SetBoxStyle(boxStyle);
    focusableComponent->SetFocusedBoxStyle(focusedBoxStyle);

    if (json.Contains(AUTO_FOCUSED) && json.GetValue(AUTO_FOCUSED)->IsBool()) {
        focusableComponent->SetAutoFocused(json.GetValue(AUTO_FOCUSED)->GetBool());
    }

    return focusableComponent;
}

} // namespace OHOS::Ace