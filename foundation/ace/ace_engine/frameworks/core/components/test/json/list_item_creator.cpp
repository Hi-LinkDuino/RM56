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

#include "core/components/test/json/list_item_creator.h"

#include "base/log/log.h"
#include "core/components/list/list_item_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ListItemCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson ListItem");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != LIST_ITEM_NAME) {
        LOGE("Create ListItem err: not a list item json.");
        return nullptr;
    }

    RefPtr<Component> child;
    if (componentJson.Contains(CHILD) && componentJson.GetValue(CHILD)->IsObject()) {
        auto childJson = componentJson.GetValue(CHILD);
        child = factory.CreateComponent(*childJson);
    }

    std::string type;
    if (componentJson.Contains(LIST_ITEM_TYPE) && componentJson.GetValue(LIST_ITEM_TYPE)->IsString()) {
        type = componentJson.GetValue(LIST_ITEM_TYPE)->GetString();
        LOGD("Create ListItem type: %{public}s", type.c_str());
    }

    return AceType::MakeRefPtr<ListItemComponent>(type, child);
}

} // namespace OHOS::Ace
