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

#include "core/components/declaration/svg/svg_fe_flood_declaration.h"

#include "core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgFeFloodDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_FE_FLOOD_ATTR);
}

void SvgFeFloodDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgFeFloodAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgFeFloodDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFeFloodDeclaration&)> attrs[] = {
        { DOM_SVG_FE_FLOOD_COLOR,
            [](const std::string& val, SvgFeFloodDeclaration& declaration) {
                declaration.SetFloodColor(declaration.ParseColor(val));
            } },
        { DOM_SVG_FE_FLOOD_OPACITY,
            [](const std::string& val, SvgFeFloodDeclaration& declaration) {
                declaration.SetFloodOpacity(declaration.ParseDouble(val));
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