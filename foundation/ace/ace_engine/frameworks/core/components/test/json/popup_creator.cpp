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

#include "core/components/test/json/popup_creator.h"

#include "core/components/popup/popup_component.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const char BACKGROUND_COLOR_POPUP[] = "backgroundColor";
const std::unordered_map<std::string, Placement> PLACEMENT_MAP = {
    { "left", Placement::LEFT },
    { "right", Placement::RIGHT },
    { "top", Placement::TOP },
    { "bottom", Placement::BOTTOM },
    { "topLeft", Placement::TOP_LEFT },
    { "topRight", Placement::TOP_RIGHT },
    { "bottomLeft", Placement::BOTTOM_LEFT },
    { "bottomRight", Placement::BOTTOM_RIGHT },
};

} // namespace

RefPtr<Component> PopupCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson popup");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != POPUP_NAME || !componentJson.Contains(CHILD) || !componentJson.GetValue(CHILD)->IsObject()) {
        LOGE("Create PopupComponent err: not a popup json or not contain child.");
        return nullptr;
    }

    ComposeId id = componentJson.GetString(POPUP_COMPOSED_ID);
    std::string name = componentJson.GetString(POPUP_COMPOSED_NAME);
    auto childJson = componentJson.GetValue(CHILD);
    auto child = factory.CreateComponent(*childJson);
    auto popup = AceType::MakeRefPtr<PopupComponent>(id, name, child);

    // create placement.
    Placement placement = Placement::BOTTOM;
    auto placementStr = componentJson.GetString(POPUP_PLACEMENT);
    auto it = PLACEMENT_MAP.find(placementStr);
    if (it != PLACEMENT_MAP.end()) {
        placement = it->second;
    }
    popup->GetPopupParam()->SetPlacement(placement);

    // create maskColor.
    auto maskColorJson = componentJson.GetObject(POPUP_MASK_COLOR);
    auto maskColor = ColorCreator::CreateFromJson(*maskColorJson);
    popup->GetPopupParam()->SetMaskColor(maskColor);

    // create backgroundColor.
    auto backgroundColorJson = componentJson.GetObject(BACKGROUND_COLOR_POPUP);
    auto backgroundColor = ColorCreator::CreateFromJson(*backgroundColorJson);
    popup->GetPopupParam()->SetBackgroundColor(backgroundColor);

    // create onVisibilityChange.
    EventMarker onVisibilityChange(componentJson.GetString(POPUP_ON_VISIBILITY_CHANGE));
    popup->GetPopupParam()->SetOnVisibilityChange(onVisibilityChange);
    return popup;
}

} // namespace OHOS::Ace