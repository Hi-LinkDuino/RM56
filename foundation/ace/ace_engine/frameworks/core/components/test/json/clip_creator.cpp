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

#include "core/components/test/json/clip_creator.h"

#include "base/log/log.h"
#include "core/components/clip/clip_component.h"
#include "core/components/test/json/radius_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

RefPtr<Component> ClipCreator::CreateFromJson(const JsonValue& json, const JsonComponentFactory& factory)
{
    auto children = factory.CreateChildComponent(json);
    auto child = children.empty() ? nullptr : children.front();
    auto clip = AceType::MakeRefPtr<ClipComponent>(child);
    clip->SetTopLeftRadius(CreateRadiusFromJson(json, BORDER_TOP_LEFT));
    clip->SetTopRightRadius(CreateRadiusFromJson(json, BORDER_TOP_RIGHT));
    clip->SetBottomLeftRadius(CreateRadiusFromJson(json, BORDER_BOTTOM_LEFT));
    clip->SetBottomRightRadius(CreateRadiusFromJson(json, BORDER_BOTTOM_RIGHT));
    return clip;
}

Radius ClipCreator::CreateRadiusFromJson(const JsonValue& json, const std::string& key)
{
    Radius radius;
    if (json.Contains(key) && json.GetValue(key)->IsObject()) {
        auto radiusJson = json.GetValue(key);
        radius = RadiusCreator::CreateFromJson(*radiusJson);
    }
    return radius;
}

} // namespace OHOS::Ace
