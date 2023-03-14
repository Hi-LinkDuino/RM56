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

#include "core/components/declaration/svg/svg_fe_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

bool SvgFeDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgFeDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgFeDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgFeDeclaration&)> attrs[] = {
        { DOM_SVG_FE_COLOR_INTERPOLATION_FILTERS,
            [](const std::string& val, SvgFeDeclaration& declaration) {
                declaration.SetColorInterpolationType(val);
            } },
        { DOM_SVG_HEIGHT,
            [](const std::string& val, SvgFeDeclaration& declaration) {
                declaration.SetHeight(declaration.ParseDimension(val));
            } },
        { DOM_SVG_FE_IN,
            [](const std::string& val, SvgFeDeclaration& declaration) {
                declaration.SetIn(val);
            } },
        { DOM_SVG_FE_RESULT,
            [](const std::string& val, SvgFeDeclaration& declaration) {
                declaration.SetResult(val);
            } },
        { DOM_SVG_WIDTH,
            [](const std::string& val, SvgFeDeclaration& declaration) {
                declaration.SetWidth(declaration.ParseDimension(val));
            } },
        { DOM_SVG_X,
            [](const std::string& val, SvgFeDeclaration& declaration) {
                declaration.SetX(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y,
            [](const std::string& val, SvgFeDeclaration& declaration) {
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