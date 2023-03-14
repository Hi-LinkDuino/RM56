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

#include "padding_creator.h"

#include "base/log/log.h"
#include "core/components/padding/padding_component.h"
#include "core/components/test/json/edge_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> PaddingCreator::CreateFromJson(const JsonValue& jsonValue, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson padding");
    std::string classType = jsonValue.GetValue(CLASS_NAME)->GetString();
    if (classType != PADDING_NAME) {
        LOGE("Create PaddingComponent err: not a padding json.");
        return nullptr;
    }
    auto paddingComponent = AceType::MakeRefPtr<PaddingComponent>();
    if (jsonValue.Contains(PADDING) && jsonValue.GetValue(PADDING)->IsObject()) {
        auto paddingJson = jsonValue.GetValue(PADDING);
        paddingComponent->SetPadding(EdgeCreator::CreateFromJson(*paddingJson));
    }
    if (jsonValue.Contains(CHILD) && jsonValue.GetValue(CHILD)->IsObject()) {
        auto childJson = jsonValue.GetValue(CHILD);
        auto child = factory.CreateComponent(*childJson);
        paddingComponent->SetChild(child);
    }
    return paddingComponent;
}

} // namespace OHOS::Ace