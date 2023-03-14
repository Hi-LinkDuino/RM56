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

#include "core/components/test/json/align_creator.h"

#include <map>

#include "base/log/log.h"
#include "core/components/align/align_component.h"
#include "core/components/common/properties/alignment.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

const std::map<std::string, double> ALIGN_VALUE_MAP_HORIZONTAL = { { HORIZONTAL_VALUE_LEFT, -1.0 },
    { HORIZONTAL_VALUE_CENTER, 0.0 }, { HORIZONTAL_VALUE_RIGHT, 1.0 } };
const std::map<std::string, double> ALIGN_VALUE_MAP_VERTICAL = { { VERTICAL_VALUE_TOP, -1.0 },
    { VERTICAL_VALUE_CENTER, 0.0 }, { VERTICAL_VALUE_BOTTOM, 1.0 } };

} // namespace

RefPtr<Component> AlignCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson Align");
    std::string classType = componentJson.GetValue(CLASS_NAME)->GetString();
    if (classType != ALIGN_NAME) {
        LOGE("Create Align err: not an align json.");
        return nullptr;
    }
    auto children = factory.CreateChildComponent(componentJson);
    auto alignment = GetAlignmentFromJson(componentJson);
    return AceType::MakeRefPtr<AlignComponent>(children, alignment);
}

Alignment AlignCreator::GetAlignmentFromJson(const JsonValue& componentJson)
{
    std::string horizontal = HORIZONTAL_VALUE_CENTER;
    std::string vertical = VERTICAL_VALUE_CENTER;
    horizontal = componentJson.GetString(HORIZONTAL);
    vertical = componentJson.GetString(VERTICAL);
    auto horizontalIter = ALIGN_VALUE_MAP_HORIZONTAL.find(horizontal);
    auto verticalIter = ALIGN_VALUE_MAP_VERTICAL.find(vertical);
    double dx = (horizontalIter == ALIGN_VALUE_MAP_HORIZONTAL.end()) ? 0.0 : horizontalIter->second;
    double dy = (verticalIter == ALIGN_VALUE_MAP_VERTICAL.end()) ? 0.0 : verticalIter->second;
    return Alignment(dx, dy);
}

} // namespace OHOS::Ace
