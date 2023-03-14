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

#include "core/components/test/json/flex_item_creator.h"

#include "base/log/log.h"
#include "base/utils/macros.h"
#include "base/utils/utils.h"
#include "core/components/flex/flex_item_component.h"
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

RefPtr<Component> FlexItemCreator::CreateFromJson(const JsonValue& componentJson, const JsonComponentFactory& factory)
{
    LOGD("CreateFromJson FlexItem");
    auto children = factory.CreateChildComponent(componentJson);
    auto child = children.empty() ? nullptr : children.front();
    double flexGrow = GetDoubleVal(componentJson, FLEX_GROW);
    double flexShrink = GetDoubleVal(componentJson, FLEX_SHRINK);
    double flexBasis = GetDoubleVal(componentJson, FLEX_BASIS);
    return AceType::MakeRefPtr<FlexItemComponent>(flexGrow, flexShrink, flexBasis, child);
}

} // namespace OHOS::Ace
