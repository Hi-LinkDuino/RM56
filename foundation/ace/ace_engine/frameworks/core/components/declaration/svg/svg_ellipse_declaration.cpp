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

#include "core/components/declaration/svg/svg_ellipse_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgEllipseDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_ELLIPSE_ATTR);
}

void SvgEllipseDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgEllipseAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgEllipseDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgEllipseDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgEllipseDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgEllipseDeclaration&)> attrs[] = {
        { DOM_SVG_CX,
            [](const std::string& val, SvgEllipseDeclaration& declaration) {
                declaration.SetCx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_CY,
            [](const std::string& val, SvgEllipseDeclaration& declaration) {
                declaration.SetCy(declaration.ParseDimension(val));
            } },
        { DOM_SVG_RX,
            [](const std::string& val, SvgEllipseDeclaration& declaration) {
                declaration.SetRx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_RY,
            [](const std::string& val, SvgEllipseDeclaration& declaration) {
                declaration.SetRy(declaration.ParseDimension(val));
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), attr.first.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return SetPresentationAttr(attr);
}

} // namespace OHOS::Ace