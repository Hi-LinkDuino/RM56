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

#include "core/components/declaration/svg/svg_pattern_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgPatternDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_PATTERN_ATTR);
}

void SvgPatternDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgPatternAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgPatternDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgPatternDeclaration&)> attrs[] = {
        { DOM_SVG_HEIGHT,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                declaration.SetHeight(declaration.ParseDimension(val));
            } },
        { DOM_SVG_PATTERN_CONTENT_UNITS,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                declaration.SetPatternContentUnits(val);
            } },
        { DOM_SVG_PATTERN_TRANSFORM,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                declaration.SetPresentationAttr(std::make_pair(DOM_TRANSFORM, val));
            } },
        { DOM_SVG_PATTERN_UNITS,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                declaration.SetPatternUnits(val);
            } },
        { DOM_SVG_VIEW_BOX,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                std::vector<double> viewBox;
                StringUtils::StringSpliter(val, ' ', viewBox);
                if (viewBox.size() == 4) {
                    declaration.SetViewBox(Rect(viewBox[0], viewBox[1], viewBox[2], viewBox[3]));
                }
            } },
        { DOM_SVG_WIDTH,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                declaration.SetWidth(declaration.ParseDimension(val));
            } },
        { DOM_SVG_X,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
                declaration.SetX(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y,
            [](const std::string& val, SvgPatternDeclaration& declaration) {
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