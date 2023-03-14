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
#include "core/components_v2/inspector/image_animator_composed_element.h"

#include "base/log/dump_log.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const ImageAnimatorComposedElement&)>> CREATE_JSON_MAP {
    { "state", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetStatus(); } },
    { "duration", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetDuration(); } },
    { "reverse", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetReverse(); } },
    { "fixedSize", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetFixedSize(); } },
    { "preDecode", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetPreDecode(); } },
    { "fillMode", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetFillMode(); } },
    { "iterations", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetIteration(); } },
    { "images", [](const ImageAnimatorComposedElement& inspector) { return inspector.GetImages(); } },
};

} // namespace

void ImageAnimatorComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    for (const auto& value : CREATE_JSON_MAP) {
        DumpLog::GetInstance().AddDesc(std::string(value.first + ": ").append(value.second(*this)));
    }
}

std::unique_ptr<JsonValue> ImageAnimatorComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string ImageAnimatorComposedElement::GetStatus() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        auto statusIndex = static_cast<int32_t>(element->GetStatus());
        switch (statusIndex) {
            case 1: return "AnimationStatus.Running";
            case 2: return "AnimationStatus.Paused";
            case 3: return "AnimationStatus.Stopped";
            default: return "AnimationStatus.Initial";
        }
    }
    return "AnimationStatus.Initial";
}

std::string ImageAnimatorComposedElement::GetDuration() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        return std::to_string(element->GetDuration());
    }
    return "";
}

std::string ImageAnimatorComposedElement::GetReverse() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        return element->GetReverse() ? "true" : "false";
    }
    return "";
}

std::string ImageAnimatorComposedElement::GetFixedSize() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        return element->GetFixedSize() ? "true" : "false";
    }
    return "";
}

std::string ImageAnimatorComposedElement::GetPreDecode() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        return std::to_string(element->GetPreDecode());
    }
    return "";
}

std::string ImageAnimatorComposedElement::GetIteration() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        return std::to_string(element->GetIteration());
    }
    return "";
}

std::string ImageAnimatorComposedElement::GetFillMode() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        auto fillModeIndex = static_cast<int32_t>(element->GetFillMode());
        switch (fillModeIndex) {
            case 1: return "FillMode.Forwards";
            case 2: return "FillMode.Backwards";
            case 3: return "FillMode.Both";
            default: return "FillMode.None";
        }
    }
    return "FillMode.None";
}

std::string ImageAnimatorComposedElement::GetImages() const
{
    auto element = GetContentElement<ImageAnimatorElement>(ImageAnimatorElement::TypeId());
    if (element) {
        auto jsonArray = JsonUtil::CreateArray(false);
        for (const auto& image : element->GetImages()) {
            auto item = JsonUtil::Create(false);
            item->Put("src", image.src.c_str());
            item->Put("left", image.left.ToString().c_str());
            item->Put("top", image.top.ToString().c_str());
            item->Put("width", image.width.ToString().c_str());
            item->Put("height", image.height.ToString().c_str());
            item->Put("duration", std::to_string(image.duration).c_str());

            jsonArray->Put(item);
        }
        return jsonArray->ToString();
    }
    return "";
}

} // namespace OHOS::Ace::V2