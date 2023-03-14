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

#include "core/components/declaration/svg/svg_fe_offset_declaration.h"

#include "core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgFeOffsetDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_FE_OFFSET_ATTR);
}

void SvgFeOffsetDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgFeOffsetAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgFeOffsetDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFeOffsetDeclaration&)> attrs[] = {
        { DOM_SVG_DX,
            [](const std::string& val, SvgFeOffsetDeclaration& declaration) {
                declaration.SetDx(declaration.ParseDimension(val));
            } },
        { DOM_SVG_DY,
            [](const std::string& val, SvgFeOffsetDeclaration& declaration) {
                declaration.SetDy(declaration.ParseDimension(val));
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), attr.first.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return SvgFeDeclaration::SetSpecializedValue(attr);
}

} // namespace OHOS::Ace