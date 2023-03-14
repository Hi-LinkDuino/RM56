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

#include "core/components/test/json/display_creator.h"

#include "base/log/log.h"
#include "core/components/display/display_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> DisplayCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson display");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != DISPLAY_NAME || !componentJson.Contains(CHILD) || !componentJson.GetValue(CHILD)->IsObject()) {
        LOGE("Create DisplayComponent err: not a display json or not contain child.");
        return nullptr;
    }

    auto childJson = componentJson.GetValue(CHILD);
    auto child = factory.CreateComponent(*childJson);
    auto displayComponent = AceType::MakeRefPtr<DisplayComponent>(child);
    if (componentJson.Contains(DISPLAY_VISIBLE) && componentJson.GetValue(DISPLAY_VISIBLE)->IsString()) {
        auto visibleStr = componentJson.GetValue(DISPLAY_VISIBLE)->GetString();
        displayComponent->SetVisible(DisplayCreator::ConvertStrToVisible(visibleStr));
    }
    if (componentJson.Contains(DISPLAY_OPACITY) && componentJson.GetValue(DISPLAY_OPACITY)->IsNumber()) {
        auto opacity = std::max(componentJson.GetValue(DISPLAY_OPACITY)->GetDouble(), 0.0);
        opacity = std::min(opacity, 1.0);
        displayComponent->SetOpacity(opacity);
    }
    return displayComponent;
}

VisibleType DisplayCreator::ConvertStrToVisible(const std::string& visibleStr)
{
    VisibleType visible;
    if (visibleStr == "visible") {
        visible = VisibleType::VISIBLE;
    } else if (visibleStr == "invisible") {
        visible = VisibleType::INVISIBLE;
    } else if (visibleStr == "gone") {
        visible = VisibleType::GONE;
    } else {
        visible = VisibleType::VISIBLE;
    }
    return visible;
}

} // namespace OHOS::Ace