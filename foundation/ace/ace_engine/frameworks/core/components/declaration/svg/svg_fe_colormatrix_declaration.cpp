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

#include "core/components/declaration/svg/svg_fe_colormatrix_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgFeColorMatrixDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_FE_COLORMATRIX_ATTR);
}

void SvgFeColorMatrixDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgFeColorMatrixAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgFeColorMatrixDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFeColorMatrixDeclaration&)> attrs[] = {
        { DOM_SVG_FE_TYPE,
            [](const std::string& val, SvgFeColorMatrixDeclaration& declaration) {
                declaration.SetType(val);
            } },
        { DOM_SVG_FE_VALUES,
            [](const std::string& val, SvgFeColorMatrixDeclaration& declaration) {
                declaration.SetValues(val);
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