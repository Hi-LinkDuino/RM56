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

#include "core/components/test/json/compose_creator.h"

#include "base/log/log.h"
#include "core/pipeline/base/composed_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ComposeCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson compose");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != COMPOSE_NAME) {
        LOGE("Create ComposedComponent err: not a compose json.");
        return nullptr;
    }
    ComposeId id;
    if (componentJson.Contains(COMPOSE_ID) && componentJson.GetValue(COMPOSE_ID)->IsString()) {
        id = componentJson.GetValue(COMPOSE_ID)->GetString();
    }
    std::string name;
    if (componentJson.Contains(NAME) && componentJson.GetValue(NAME)->IsString()) {
        name = componentJson.GetValue(NAME)->GetString();
    }
    RefPtr<Component> child;
    if (componentJson.Contains(COMPOSE_DATA)) {
        const auto& composeData = componentJson.GetValue(COMPOSE_DATA);
        child = factory.CreateComponent(*composeData);
    }
    return AceType::MakeRefPtr<ComposedComponent>(id, name, child);
}

} // namespace OHOS::Ace
