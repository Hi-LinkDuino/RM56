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

#include "core/components/test/json/tween_creator.h"

#include "core/components/test/json/tween_option_creator.h"
#include "core/components/tween/tween_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> TweenCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Tween");
    TweenOption option;
    if (json.Contains(TWEEN_OPTION) && json.GetValue(TWEEN_OPTION)->IsObject()) {
        option = TweenOptionCreator::CreateFromJson(*(json.GetValue(TWEEN_OPTION)));
    }
    ComposeId id;
    if (json.Contains(COMPOSE_ID) && json.GetValue(COMPOSE_ID)->IsString()) {
        id = json.GetValue(COMPOSE_ID)->GetString();
    }
    std::string name;
    if (json.Contains(NAME) && json.GetValue(NAME)->IsString()) {
        name = json.GetValue(NAME)->GetString();
    }
    RefPtr<Component> child;
    if (json.Contains(COMPOSE_DATA)) {
        const auto& composeData = json.GetValue(COMPOSE_DATA);
        child = factory.CreateComponent(*composeData);
    }
    auto tweenComponent = AceType::MakeRefPtr<TweenComponent>(id, name, child);
    tweenComponent->SetTweenOption(option);
    return tweenComponent;
}

} // namespace OHOS::Ace