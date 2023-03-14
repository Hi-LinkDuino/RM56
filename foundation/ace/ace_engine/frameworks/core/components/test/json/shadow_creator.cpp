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

#include "core/components/test/json/shadow_creator.h"

#include "base/log/log.h"
#include "core/components/test/json/color_creator.h"
#include "core/components/test/json/offset_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

Shadow ShadowCreator::CreateFromJson(const JsonValue& componentJson)
{
    LOGD("CreateFromJson Shadow");
    Shadow shadow;
    if (componentJson.Contains(OFFSET) && componentJson.GetValue(OFFSET)->IsObject()) {
        auto offset = OffsetCreator::CreateFromJson(*(componentJson.GetValue(OFFSET)));
        shadow.SetOffset(offset);
    }
    if (componentJson.Contains(RADIUS) && componentJson.GetValue(RADIUS)->IsNumber()) {
        auto radius = componentJson.GetValue(RADIUS)->GetDouble();
        shadow.SetBlurRadius(radius);
    }
    if (componentJson.Contains(TEXT_COLOR) && componentJson.GetValue(TEXT_COLOR)->IsObject()) {
        auto color = ColorCreator::CreateFromJson(*(componentJson.GetValue(TEXT_COLOR)));
        shadow.SetColor(color);
    }
    if (componentJson.Contains(SPREAD) && componentJson.GetValue(SPREAD)->IsNumber()) {
        auto spread = componentJson.GetValue(SPREAD)->GetDouble();
        shadow.SetSpreadRadius(spread);
    }
    if (componentJson.Contains(ELEVATION) && componentJson.GetValue(ELEVATION)->IsNumber()) {
        auto elevation = componentJson.GetValue(ELEVATION)->GetDouble();
        shadow.SetElevation(elevation);
    }
    return shadow;
}

} // namespace OHOS::Ace
