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

#include "core/components/test/json/radius_creator.h"

#include <algorithm>

#include "base/log/log.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

Radius RadiusCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("CreateFromJson Radius");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != RADIUS_NAME) {
        LOGE("Create Radius err: not a radius json.");
        return Radius();
    }
    if (json.Contains(RADIUS_VALUE) && json.GetValue(RADIUS_VALUE)->IsNumber()) {
        double value = std::max(json.GetValue(RADIUS_VALUE)->GetDouble(), 0.0);
        return Radius(value);
    }
    double xRadius = 0.0;
    if (json.Contains(RADIUS_X) && json.GetValue(RADIUS_X)->IsNumber()) {
        xRadius = std::max(json.GetValue(RADIUS_X)->GetDouble(), 0.0);
    }
    double yRadius = 0.0;
    if (json.Contains(RADIUS_Y) && json.GetValue(RADIUS_Y)->IsNumber()) {
        yRadius = std::max(json.GetValue(RADIUS_Y)->GetDouble(), 0.0);
    }
    return Radius(xRadius, yRadius);
}

} // namespace OHOS::Ace
