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

#include "core/components/declaration/svg/svg_gradient_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

namespace {

const char DOM_SVG_SRC_GRADIENT_TRANSFORM[] = "gradientTransform";
const char DOM_SVG_SRC_SPREAD_METHOD[] = "spreadMethod";

} // namespace

using namespace Framework;

void SvgGradientDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_GRADIENT_ATTR);
}

void SvgGradientDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgGradientAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgGradientDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgGradientDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgGradientDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgGradientDeclaration&)> attrs[] = {
        { DOM_SVG_CX,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetCx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_CY,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetCy(declaration.ParseDimension(val));
            } },
        { DOM_SVG_FX,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetFx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_FY,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetFy(declaration.ParseDimension(val));
            } },
        { DOM_SVG_SRC_GRADIENT_TRANSFORM,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetTransform(val);
            } },
        { DOM_SVG_GRADIENT_TRANSFORM,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetTransform(val);
            } },
        { DOM_SVG_R,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetR(declaration.ParseDimension(val));
            } },
        { DOM_SVG_SRC_SPREAD_METHOD,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                if (val == "pad") {
                    declaration.SetSpreadMethod(SpreadMethod::PAD);
                }
                if (val == "reflect") {
                    declaration.SetSpreadMethod(SpreadMethod::REFLECT);
                }
                if (val == "repeat") {
                    declaration.SetSpreadMethod(SpreadMethod::REPEAT);
                }
            } },
        { DOM_SVG_SPREAD_METHOD,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                if (val == "pad") {
                    declaration.SetSpreadMethod(SpreadMethod::PAD);
                }
                if (val == "reflect") {
                    declaration.SetSpreadMethod(SpreadMethod::REFLECT);
                }
                if (val == "repeat") {
                    declaration.SetSpreadMethod(SpreadMethod::REPEAT);
                }
            } },
        { DOM_SVG_X1,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetX1(declaration.ParseDimension(val));
            } },
        { DOM_SVG_X2,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetX2(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y1,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetY1(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y2,
            [](const std::string& val, SvgGradientDeclaration& declaration) {
                declaration.SetY2(declaration.ParseDimension(val));
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), attr.first.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
