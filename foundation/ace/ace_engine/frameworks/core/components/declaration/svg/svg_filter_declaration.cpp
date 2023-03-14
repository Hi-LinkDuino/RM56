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

#include "core/components/declaration/svg/svg_filter_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgFilterDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_FILTER_ATTR);
}

void SvgFilterDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgFilterAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgFilterDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFilterDeclaration&)> attrs[] = {
        { DOM_SVG_HEIGHT,
            [](const std::string& val, SvgFilterDeclaration& declaration) {
                declaration.SetHeight(declaration.ParseDimension(val));
            } },
        { DOM_SVG_WIDTH,
            [](const std::string& val, SvgFilterDeclaration& declaration) {
                declaration.SetWidth(declaration.ParseDimension(val));
            } },
        { DOM_SVG_X,
            [](const std::string& val, SvgFilterDeclaration& declaration) {
                declaration.SetX(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y,
            [](const std::string& val, SvgFilterDeclaration& declaration) {
                declaration.SetY(declaration.ParseDimension(val));
            } },
    };

    std::string key = attr.first;
    StringUtils::TransfromStrCase(key, StringUtils::TEXT_CASE_LOWERCASR);
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), key.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return SvgBaseDeclaration::SetPresentationAttr(attr);
}

} // namespace OHOS::Ace