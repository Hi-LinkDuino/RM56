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

#include "core/components/declaration/svg/svg_line_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgLineDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_LINE_ATTR);
}

void SvgLineDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgLineAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgLineDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgLineDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgLineDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgLineDeclaration&)> attrs[] = {
        { DOM_SVG_X1,
            [](const std::string& val, SvgLineDeclaration& declaration) {
                declaration.SetX1(declaration.ParseDimension(val));
            } },
        { DOM_SVG_X2,
            [](const std::string& val, SvgLineDeclaration& declaration) {
                declaration.SetX2(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y1,
            [](const std::string& val, SvgLineDeclaration& declaration) {
                declaration.SetY1(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y2,
            [](const std::string& val, SvgLineDeclaration& declaration) {
                declaration.SetY2(declaration.ParseDimension(val));
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