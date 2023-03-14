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

#include "core/components/test/json/tab_content_creator.h"

#include "base/log/log.h"
#include "core/components/tab_bar/tab_content_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TabContentCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Tabcontent");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != TAB_CONTENT_NAME) {
        LOGE("Create Tabcontent err: not a tabcontent json.");
        return nullptr;
    }

    auto children = factory.CreateChildComponent(componentJson);
    int32_t controllerId = 0;
    if (componentJson.Contains(TAB_CONTROLLER_ID) && componentJson.GetValue(TAB_CONTROLLER_ID)->IsNumber()) {
        controllerId = componentJson.GetValue(TAB_CONTROLLER_ID)->GetInt();
    }

    RefPtr<TabController> controller = TabController::GetController(controllerId);
    auto contents = AceType::MakeRefPtr<TabContentComponent>(children, controller);

    if (componentJson.Contains(TAB_CONTENT_SCROLLABEL) && componentJson.GetValue(TAB_CONTENT_SCROLLABEL)->IsBool()) {
        contents->SetScrollable(componentJson.GetValue(TAB_CONTENT_SCROLLABEL)->GetBool());
    }

    if (componentJson.Contains(TAB_IS_VERTICAL) && componentJson.GetValue(TAB_IS_VERTICAL)->IsBool()) {
        contents->SetVertical(componentJson.GetValue(TAB_IS_VERTICAL)->GetBool());
    }

    if (componentJson.Contains(TAB_CONTENT_ON_CHANGE) && componentJson.GetValue(TAB_CONTENT_ON_CHANGE)->IsString()) {
        auto onChanged = componentJson.GetValue(TAB_CONTENT_ON_CHANGE)->GetString();
        contents->SetChangeEventId(EventMarker(onChanged));
    }
    return contents;
}

} // namespace OHOS::Ace
