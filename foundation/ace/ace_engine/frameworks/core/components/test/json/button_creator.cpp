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

#include "core/components/test/json/button_creator.h"

#include "core/components/button/button_component.h"
#include "core/components/test/json/border_edge_creator.h"
#include "core/components/test/json/color_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const ButtonType TYPES[] = { ButtonType::NORMAL, ButtonType::CAPSULE, ButtonType::CIRCLE, ButtonType::TEXT,
    ButtonType::ARC, ButtonType::DOWNLOAD, ButtonType::ICON };

Color GetColorFromJson(const JsonValue& componentJson, std::string color)
{
    if (componentJson.Contains(color) && componentJson.GetValue(color)->IsObject()) {
        auto colorJson = componentJson.GetValue(color);
        auto defColor = ColorCreator::CreateFromJson(*colorJson);
        return defColor;
    }
    return Color();
}

} // namespace

RefPtr<Component> ButtonCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson button");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != BUTTON_NAME) {
        LOGE("Create button err: not a button json.");
        return nullptr;
    }
    auto children = factory.CreateChildComponent(componentJson);
    auto button = AceType::MakeRefPtr<ButtonComponent>(children);
    if (componentJson.Contains(BUTTON_TYPE) && componentJson.GetValue(BUTTON_TYPE)->IsNumber()) {
        button->SetType(ConvertIntToEnum(componentJson.GetValue(BUTTON_TYPE)->GetInt(), TYPES, ButtonType::NORMAL));
    }
    if (componentJson.Contains(BUTTON_DISABLED_STATE) && componentJson.GetValue(BUTTON_DISABLED_STATE)->IsNumber()) {
        button->SetDisabledState(componentJson.GetValue(BUTTON_DISABLED_STATE)->GetInt());
    }
    if (componentJson.Contains(BUTTON_AUTO_FOCUS) && componentJson.GetValue(BUTTON_AUTO_FOCUS)->IsNumber()) {
        button->SetAutoFocusState(componentJson.GetValue(BUTTON_AUTO_FOCUS)->GetInt());
    }
    if (componentJson.Contains(BUTTON_HEIGHT) && componentJson.GetValue(BUTTON_HEIGHT)->IsNumber()) {
        button->SetHeight(Dimension(componentJson.GetValue(BUTTON_HEIGHT)->GetDouble()));
    }
    if (componentJson.Contains(BUTTON_WIDTH) && componentJson.GetValue(BUTTON_WIDTH)->IsNumber()) {
        button->SetWidth(Dimension(componentJson.GetValue(BUTTON_WIDTH)->GetDouble()));
    }
    if (componentJson.Contains(BUTTON_RECT_RADIUS) && componentJson.GetValue(BUTTON_RECT_RADIUS)->IsNumber()) {
        button->SetRectRadius(Dimension(componentJson.GetValue(BUTTON_RECT_RADIUS)->GetDouble()));
    }
    if (componentJson.Contains(BUTTON_CLICKED_EVENT_ID) &&
        componentJson.GetValue(BUTTON_CLICKED_EVENT_ID)->IsString()) {
        auto eventId = componentJson.GetValue(BUTTON_CLICKED_EVENT_ID)->GetString();
        button->SetClickedEventId(EventMarker(eventId));
    }
    if (componentJson.Contains(BUTTON_BORDER) && componentJson.GetValue(BUTTON_BORDER)->IsObject()) {
        auto borderJson = componentJson.GetValue(BUTTON_BORDER);
        auto borderEdge = BorderEdgeCreator::CreateFromJson(*borderJson);
        button->SetBorderEdge(borderEdge);
    }
    button->SetBackgroundColor(GetColorFromJson(componentJson, BUTTON_BACKGROUND_COLOR));
    button->SetClickedColor(GetColorFromJson(componentJson, BUTTON_CLICKED_COLOR));
    button->SetDisabledColor(GetColorFromJson(componentJson, BUTTON_DISABLED_COLOR));
    button->SetFocusColor(GetColorFromJson(componentJson, BUTTON_FOCUS_COLOR));
    return button;
}

} // namespace OHOS::Ace