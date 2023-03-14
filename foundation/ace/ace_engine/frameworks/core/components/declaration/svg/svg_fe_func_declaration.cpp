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

#include "core/components/declaration/svg/svg_fe_func_declaration.h"

#include "core/components/declaration/common/declaration_constants.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgFeFuncDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_FE_FUNC_ATTR);
}

void SvgFeFuncDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgFeFuncAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgFeFuncDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFeFuncDeclaration&)> attrs[] = {
        { DOM_SVG_FE_AMPLITUDE,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetAmplitude(declaration.ParseDouble(val));
            } },
        { DOM_SVG_FE_EXPONENT,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetExponent(declaration.ParseDouble(val));
            } },
        { DOM_SVG_FE_INTERCEPT,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetIntercept(declaration.ParseDouble(val));
            } },
        { DOM_SVG_OFFSET,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetOffset(declaration.ParseDouble(val));
            } },
        { DOM_SVG_FE_SLOPE,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetSlope(declaration.ParseDouble(val));
            } },
        { DOM_SVG_FE_TABLE_VALUES,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetTableValues(val);
            } },
        { DOM_SVG_FE_TYPE,
            [](const std::string& val, SvgFeFuncDeclaration& declaration) {
                declaration.SetType(val);
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