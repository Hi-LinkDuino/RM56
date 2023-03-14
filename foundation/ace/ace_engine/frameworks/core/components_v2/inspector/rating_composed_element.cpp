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

#include "core/components_v2/inspector/rating_composed_element.h"

#include <unordered_map>

#include "base/log/dump_log.h"
#include "core/components/common/layout/constants.h"
#include "core/components/rating/rating_element.h"
#include "core/components_v2/inspector/utils.h"

namespace OHOS::Ace::V2 {
namespace {

const std::unordered_map<std::string, std::function<std::string(const RatingComposedElement&)>> CREATE_JSON_MAP {
    { "rating", [](const RatingComposedElement& inspector) { return inspector.GetRating(); } },
    { "indicator", [](const RatingComposedElement& inspector) { return inspector.GetIndicator(); } },
    { "stars", [](const RatingComposedElement& inspector) { return inspector.GetStars(); } },
    { "stepSize", [](const RatingComposedElement& inspector) { return inspector.GetStepSize(); } },
    { "starStyle", [](const RatingComposedElement& inspector) { return inspector.GetStarStyle(); } }
};

} // namespace

void RatingComposedElement::Dump()
{
    InspectorComposedElement::Dump();
    DumpLog::GetInstance().AddDesc(std::string("rating: ").append(GetRating()));
    DumpLog::GetInstance().AddDesc(std::string("indicator: ").append(GetIndicator()));
    DumpLog::GetInstance().AddDesc(std::string("stars: ").append(GetStars()));
    DumpLog::GetInstance().AddDesc(std::string("stepSize: ").append(GetStepSize()));
    DumpLog::GetInstance().AddDesc(std::string("starStyle: ").append(GetStarStyle()));
}

std::unique_ptr<JsonValue> RatingComposedElement::ToJsonObject() const
{
    auto resultJson = InspectorComposedElement::ToJsonObject();
    for (const auto& value : CREATE_JSON_MAP) {
        resultJson->Put(value.first.c_str(), value.second(*this).c_str());
    }
    return resultJson;
}

std::string RatingComposedElement::GetRating() const
{
    auto renderRating = GetRenderRating();
    if (renderRating) {
        return std::to_string(renderRating->GetRatingScore());
    }
    return "";
}

std::string RatingComposedElement::GetIndicator() const
{
    auto renderRating = GetRenderRating();
    auto indicator = renderRating ? renderRating->GetIndicator() : false;
    return ConvertBoolToString(indicator);
}

std::string RatingComposedElement::GetStars() const
{
    auto renderRating = GetRenderRating();
    if (renderRating) {
        return std::to_string(renderRating->GetStarNum());
    }
    return "";
}

std::string RatingComposedElement::GetStepSize() const
{
    auto renderRating = GetRenderRating();
    if (renderRating) {
        return std::to_string(renderRating->GetStepSize());
    }
    return "";
}

std::string RatingComposedElement::GetStarStyle() const
{
    auto renderRating = GetRenderRating();
    auto jsonStarStyle = JsonUtil::Create(true);
    if (!renderRating) {
        return "";
    }
    jsonStarStyle->Put("backgroundUri", renderRating->GetBackgroundSrc().c_str());
    jsonStarStyle->Put("foregroundUri", renderRating->GetForegroundSrc().c_str());
    jsonStarStyle->Put("secondaryUri", renderRating->GetSecondarySrc().c_str());

    return jsonStarStyle->ToString();
}

RefPtr<RenderRating> RatingComposedElement::GetRenderRating() const
{
    auto node = GetInspectorNode(RatingElement::TypeId());
    if (node) {
        return AceType::DynamicCast<RenderRating>(node);
    }
    return nullptr;
}

} // namespace OHOS::Ace::V2