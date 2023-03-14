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

#include "core/components/declaration/svg/svg_fe_gaussianblur_declaration.h"

#include "core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgFeGaussianBlurDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_FE_GAUSSIAN_BLUR_ATTR);
}

void SvgFeGaussianBlurDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgFeGaussianBlurAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgFeGaussianBlurDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFeGaussianBlurDeclaration&)> attrs[] = {
        { DOM_SVG_FE_EDGE_MODE,
            [](const std::string& val, SvgFeGaussianBlurDeclaration& declaration) {
                declaration.SetEdgeMode(val);
            } },
        { DOM_SVG_FE_STD_DEVIATION,
            [](const std::string& val, SvgFeGaussianBlurDeclaration& declaration) {
                declaration.SetStdDeviation(declaration.ParseDouble(val));
            } },
    };
    std::string key = attr.first;
    StringUtils::TransfromStrCase(key, StringUtils::TEXT_CASE_LOWERCASR);
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), key.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return SvgFeDeclaration::SetSpecializedValue(attr);
}

} // namespace OHOS::Ace