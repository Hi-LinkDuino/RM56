/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components_v2/inspector/marquee_composed_element.h"

#include "base/log/dump_log.h"
#include "base/utils/string_utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components/marquee/marquee_element.h"
#include "core/components_v2/inspector/utils.h"
namespace OHOS::Ace::V2 {
namespace {
const std::unordered_map<std::string, std::function<std::string(const MarqueeComposedElement&)>> CREATE_JSON_MAP {
    { "src", [](const MarqueeComposedElement& inspector) { return inspector.GetValue(); } },
    { "step", [](const MarqueeComposedElement& inspector) { return inspector.GetStep(); } },
    { "loop", [](const MarqueeComposedElement& inspector) { return inspector.GetLoop(); } },
    { "start", [](const MarqueeComposedElement& inspector) { return inspector.GetStart(); } },
    { "fromStart", [](const MarqueeComposedElement& inspector) { return inspector.GetFromStart(); } }
};
} // namespace

void MarqueeComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("src: ").append(GetValue()));
    DumpLog::GetInstance().AddDesc(std::string("step: ").append(GetStep()));
    DumpLog::GetInstance().AddDesc(std::string("loop: ").append(GetLoop()));
    DumpLog::GetInstance().AddDesc(std::string("start: ").append(GetStart()));
    DumpLog::GetInstance().AddDesc(std::string("fromStart ").append(GetFromStart()));
}

std::unique_ptr<JsonValue> MarqueeComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string MarqueeComposedElement::GetValue() const
{
    auto render = GetRenderMarquee();
    if (!render) {
        return "";
    }
    return render->GetValue();
}

std::string MarqueeComposedElement::GetStep() const
{
    auto render = GetRenderMarquee();
    if (!render) {
        return "6";
    }
    return StringUtils::DoubleToString(render->GetStep());
}
std::string MarqueeComposedElement::GetLoop() const
{
    auto render = GetRenderMarquee();
    if (!render) {
        return "-1";
    }
    return std::to_string(render->GetLoop());
}

std::string MarqueeComposedElement::GetStart() const
{
    auto render = GetRenderMarquee();
    if (!render) {
        return "";
    }
    return ConvertBoolToString(render->GetStart());
}

std::string MarqueeComposedElement::GetFromStart() const
{
    auto render = GetRenderMarquee();
    if (!render) {
        return "true";
    }
    return ConvertBoolToString(render->GetFromStart());
}

RefPtr<RenderMarquee> MarqueeComposedElement::GetRenderMarquee() const
{
    auto node = GetInspectorNode(MarqueeElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderMarquee>(node);
    }
    return nullptr;
}
} // namespace OHOS::Ace::V2