/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/inspector/loading_progress_composed_element.h"

#include "base/log/dump_log.h"
#include "core/components/progress/loading_progress_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string,
    std::function<std::string(const LoadingProgressComposedElement&)>> CREATE_JSON_MAP {
    { "color", [](const LoadingProgressComposedElement& inspector) { return inspector.GetProgressColor(); } }
};
}

void LoadingProgressComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("color: ").append(GetProgressColor()));
}

std::unique_ptr<JsonValue> LoadingProgressComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string LoadingProgressComposedElement::GetProgressColor() const
{
    auto render = GetRenderLoadingProgress();
    if (render) {
        return ConvertColorToString(render->GetProgressColor());
    }
    return "";
}

RefPtr<RenderLoadingProgress> LoadingProgressComposedElement::GetRenderLoadingProgress() const
{
    auto node = GetInspectorNode(LoadingProgressElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderLoadingProgress>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2