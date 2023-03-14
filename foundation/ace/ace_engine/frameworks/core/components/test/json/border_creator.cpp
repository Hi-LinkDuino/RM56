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

#include "core/components/test/json/border_creator.h"

#include "base/log/log.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/border_edge.h"
#include "core/components/test/json/border_edge_creator.h"
#include "core/components/test/json/radius_creator.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

std::unique_ptr<Border> BorderCreator::CreateFromJson(const JsonValue& json)
{
    LOGD("CreateFromJson Border");
    std::string classType = json.GetValue(CLASS_NAME)->GetString();
    if (classType != BORDER_NAME) {
        LOGE("Create border err: not a border json.");
        return nullptr;
    }
    if (json.Contains(BORDER_VALUE) && json.GetValue(BORDER_VALUE)->IsObject()) {
        auto borderEdgeJson = json.GetValue(BORDER_VALUE);
        auto border = std::make_unique<Border>(BorderEdgeCreator::CreateFromJson(*borderEdgeJson));
        UpdateRadiusFromJson(border, json);
        return border;
    }
    BorderEdge left;
    if (json.Contains(BORDER_LEFT) && json.GetValue(BORDER_LEFT)->IsObject()) {
        auto leftJson = json.GetValue(BORDER_LEFT);
        left = BorderEdgeCreator::CreateFromJson(*leftJson);
    }
    BorderEdge top;
    if (json.Contains(BORDER_TOP) && json.GetValue(BORDER_TOP)->IsObject()) {
        auto topJson = json.GetValue(BORDER_TOP);
        top = BorderEdgeCreator::CreateFromJson(*topJson);
    }
    BorderEdge right;
    if (json.Contains(BORDER_RIGHT) && json.GetValue(BORDER_RIGHT)->IsObject()) {
        auto rightJson = json.GetValue(BORDER_RIGHT);
        right = BorderEdgeCreator::CreateFromJson(*rightJson);
    }
    BorderEdge bottom;
    if (json.Contains(BORDER_BOTTOM) && json.GetValue(BORDER_BOTTOM)->IsObject()) {
        auto bottomJson = json.GetValue(BORDER_BOTTOM);
        bottom = BorderEdgeCreator::CreateFromJson(*bottomJson);
    }
    auto border = std::make_unique<Border>(left, top, right, bottom);
    UpdateRadiusFromJson(border, json);
    return border;
}

void BorderCreator::UpdateRadiusFromJson(const std::unique_ptr<Border>& border, const JsonValue& json)
{
    if (json.Contains(BORDER_RADIUS_VALUE) && json.GetValue(BORDER_RADIUS_VALUE)->IsObject()) {
        auto radiusJson = json.GetValue(BORDER_RADIUS_VALUE);
        border->SetBorderRadius(RadiusCreator::CreateFromJson(*radiusJson));
        return;
    }
    if (json.Contains(BORDER_TOP_LEFT) && json.GetValue(BORDER_TOP_LEFT)->IsObject()) {
        auto topLeftJson = json.GetValue(BORDER_TOP_LEFT);
        border->SetTopLeftRadius(RadiusCreator::CreateFromJson(*topLeftJson));
    }
    if (json.Contains(BORDER_TOP_RIGHT) && json.GetValue(BORDER_TOP_RIGHT)->IsObject()) {
        auto topRightJson = json.GetValue(BORDER_TOP_RIGHT);
        border->SetTopRightRadius(RadiusCreator::CreateFromJson(*topRightJson));
    }
    if (json.Contains(BORDER_BOTTOM_LEFT) && json.GetValue(BORDER_BOTTOM_LEFT)->IsObject()) {
        auto bottomLeftJson = json.GetValue(BORDER_BOTTOM_LEFT);
        border->SetBottomLeftRadius(RadiusCreator::CreateFromJson(*bottomLeftJson));
    }
    if (json.Contains(BORDER_BOTTOM_RIGHT) && json.GetValue(BORDER_BOTTOM_RIGHT)->IsObject()) {
        auto bottomRightJson = json.GetValue(BORDER_BOTTOM_RIGHT);
        border->SetBottomRightRadius(RadiusCreator::CreateFromJson(*bottomRightJson));
    }
}

} // namespace OHOS::Ace