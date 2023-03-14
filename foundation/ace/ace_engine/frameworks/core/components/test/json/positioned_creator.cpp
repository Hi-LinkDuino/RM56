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

#include "core/components/test/json/positioned_creator.h"

#include <unordered_map>

#include "base/log/log.h"
#include "core/components/positioned/positioned_component.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {
namespace {

inline double GetDoubleVal(const JsonValue& componentJson, const std::string& key)
{
    double value = 0.0;
    auto jsonVal = componentJson.GetValue(key);
    if (jsonVal && jsonVal->IsNumber()) {
        value = (jsonVal->GetDouble() > 0.0) ? jsonVal->GetDouble() : 0.0;
    }
    return value;
}

} // namespace

RefPtr<Component> PositionedCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    auto children = factory.CreateChildComponent(componentJson);
    auto child = children.empty() ? nullptr : children.front();
    auto positioned = AceType::MakeRefPtr<PositionedComponent>(child);
    positioned->SetWidth(GetDoubleVal(componentJson, POSITIONED_WIDTH));
    positioned->SetHeight(GetDoubleVal(componentJson, POSITIONED_HEIGHT));
    positioned->SetBottom(Dimension(GetDoubleVal(componentJson, POSITIONED_BOTTOM), DimensionUnit::PX));
    positioned->SetTop(Dimension(GetDoubleVal(componentJson, POSITIONED_TOP), DimensionUnit::PX));
    positioned->SetLeft(Dimension(GetDoubleVal(componentJson, POSITIONED_LEFT), DimensionUnit::PX));
    positioned->SetRight(Dimension(GetDoubleVal(componentJson, POSITIONED_RIGHT), DimensionUnit::PX));
    return positioned;
}

} // namespace OHOS::Ace
