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

#include "core/components/test/json/edge_creator.h"

#include <algorithm>

#include "base/log/log.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

Edge EdgeCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("CreateFromJson Edge");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != EDGE_NAME) {
        LOGE("Create Edge err: not an edge json.");
        return Edge();
    }
    if (json.Contains(EDGE_VALUE) && json.GetValue(EDGE_VALUE)->IsNumber()) {
        double value = std::max(json.GetValue(EDGE_VALUE)->GetDouble(), 0.0);
        return Edge(value);
    }
    double left = 0.0;
    if (json.Contains(EDGE_LEFT) && json.GetValue(EDGE_LEFT)->IsNumber()) {
        left = std::max(json.GetValue(EDGE_LEFT)->GetDouble(), 0.0);
    }
    double top = 0.0;
    if (json.Contains(EDGE_TOP) && json.GetValue(EDGE_TOP)->IsNumber()) {
        top = std::max(json.GetValue(EDGE_TOP)->GetDouble(), 0.0);
    }
    double right = 0.0;
    if (json.Contains(EDGE_RIGHT) && json.GetValue(EDGE_RIGHT)->IsNumber()) {
        right = std::max(json.GetValue(EDGE_RIGHT)->GetDouble(), 0.0);
    }
    double bottom = 0.0;
    if (json.Contains(EDGE_BOTTOM) && json.GetValue(EDGE_BOTTOM)->IsNumber()) {
        bottom = std::max(json.GetValue(EDGE_BOTTOM)->GetDouble(), 0.0);
    }
    return Edge(left, top, right, bottom);
}

} // namespace OHOS::Ace