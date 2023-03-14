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

#include "frameworks/core/components/svg/parse/svg_node.h"

#include "include/core/SkString.h"
#include "include/utils/SkParsePath.h"

#include "frameworks/core/components/svg/parse/svg_gradient.h"

namespace OHOS::Ace {

double SvgNode::ConvertDimensionToPx(const Dimension& value, const Size& viewPort, SvgLengthType type) const
{
    switch (value.Unit()) {
        case DimensionUnit::PERCENT: {
            if (type == SvgLengthType::HORIZONTAL) {
                return value.Value() * viewPort.Width();
            }
            if (type == SvgLengthType::VERTICAL) {
                return value.Value() * viewPort.Height();
            }
            if (type == SvgLengthType::OTHER) {
                return value.Value() * sqrt(viewPort.Width() * viewPort.Height());
            }
            return 0.0;
        }
        case DimensionUnit::PX:
            return value.Value();
        default:
            auto context = context_.Upgrade();
            if (context) {
                return context->NormalizeToPx(value);
            }
            return 0.0;
    }
}

std::string SvgNode::AsClipPathCommands(const Size& viewPort,
    const std::string& href) const
{
    auto svgContext = svgContext_.Upgrade();
    if (!svgContext) {
        LOGE("AsClipPathCommands failed, svgContext is null");
        return "";
    }
    if (href.empty()) {
        return "";
    }
    auto refSvgNode = svgContext->GetSvgNodeById(href);
    if (!refSvgNode) {
        LOGE("refSvgNode is null");
        return "";
    }
    auto skPath = refSvgNode->AsPath(viewPort);
    SkString outString;
    SkParsePath::ToSVGString(skPath, &outString);
    return outString.c_str();
}

std::optional<Gradient> SvgNode::GetGradient(const std::string& href)
{
    auto svgContext = svgContext_.Upgrade();
    if (!svgContext) {
        LOGE("Gradient failed, svgContext is null");
        return std::nullopt;
    }
    if (href.empty()) {
        return std::nullopt;
    }
    auto refSvgNode = svgContext->GetSvgNodeById(href);
    if (!refSvgNode) {
        LOGE("refSvgNode is null");
        return std::nullopt;
    }
    auto svgGradient = DynamicCast<SvgGradient>(refSvgNode);
    if (svgGradient) {
        return std::make_optional(svgGradient->GetGradient());
    }
    return std::nullopt;
}

RefPtr<BoxComponent> SvgNode::CreateBoxComponent(const LayoutParam& layoutParam, const std::string& clipPathHref)
{
    auto boxComponent = AceType::MakeRefPtr<BoxComponent>();
    if (!clipPathHref.empty()) {
        auto pathCommands = AsClipPathCommands(layoutParam.GetMaxSize(), clipPathHref);
        if (!pathCommands.empty()) {
            auto clipPath = AceType::MakeRefPtr<ClipPath>();
            auto path = AceType::MakeRefPtr<Path>();
            path->SetValue(pathCommands);
            clipPath->SetBasicShape(path);
            boxComponent->SetClipPath(clipPath);
        }
    }
    return boxComponent;
}

} // namespace OHOS::Ace
