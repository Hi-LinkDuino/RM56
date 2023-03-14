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

#include "core/components/declaration/svg/svg_text_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgTextDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_TEXT_ATTR);
}

void SvgTextDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgTextAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgTextDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgTextDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgTextDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgTextDeclaration&)> attrs[] = {
        { DOM_SVG_DX,
            [](const std::string& val, SvgTextDeclaration& declaration) {
                declaration.SetDx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_DY,
            [](const std::string& val, SvgTextDeclaration& declaration) {
                declaration.SetDy(declaration.ParseDimension(val));
            } },
        { DOM_SVG_ROTATE,
            [](const std::string& val, SvgTextDeclaration& declaration) {
                declaration.SetRotate(declaration.ParseDouble(val));
                declaration.hasRotate_ = true;
            } },
        { DOM_VALUE,
            [](const std::string& val, SvgTextDeclaration& declaration) {
                declaration.SetTextData(val);
            } },
        { DOM_SVG_X,
            [](const std::string& val, SvgTextDeclaration& declaration) {
                declaration.SetX(declaration.ParseDimension(val));
                declaration.hasX_ = true;
            } },
        { DOM_SVG_Y,
            [](const std::string& val, SvgTextDeclaration& declaration) {
                declaration.SetY(declaration.ParseDimension(val));
                declaration.hasY_ = true;
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), attr.first.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return SvgBaseDeclaration::SetPresentationAttr(attr);
}

} // namespace OHOS::Ace