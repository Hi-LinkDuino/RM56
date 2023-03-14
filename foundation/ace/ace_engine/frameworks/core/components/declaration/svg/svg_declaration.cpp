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

#include "core/components/declaration/svg/svg_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {
namespace {

const char DOM_SVG_SRC_VIEW_BOX[] = "viewBox";

} // namespace

using namespace Framework;

void SvgDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_ATTR);
}

void SvgDeclaration::InitializeStyle()
{
    SetCurrentStyle(std::make_pair(DOM_OVERFLOW_STYLE, "hidden"));
    SetHasDisplayStyle(true);
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgDeclaration&)> attrs[] = {
        { DOM_SVG_MIRROR,
            [](const std::string& val, SvgDeclaration& declaration) {
                declaration.SetAutoMirror(val == "true");
            } },
        { DOM_SVG_HEIGHT,
            [](const std::string& val, SvgDeclaration& declaration) {
                declaration.SetHeight(declaration.ParseDimension(val));
            } },
        { DOM_SVG_SRC_VIEW_BOX,
            [](const std::string& val, SvgDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                std::vector<double> viewBox;
                StringUtils::StringSpliter(val, ' ', viewBox);
                if (viewBox.size() == 4) {
                    declaration.SetViewBox(Rect(viewBox[0], viewBox[1], viewBox[2], viewBox[3]));
                }
            } },
        { DOM_SVG_VIEW_BOX,
            [](const std::string& val, SvgDeclaration& declaration) {
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
            [](const std::string& val, SvgDeclaration& declaration) {
                declaration.SetWidth(declaration.ParseDimension(val));
            } },
        { DOM_SVG_X,
            [](const std::string& val, SvgDeclaration& declaration) {
                declaration.SetX(declaration.ParseDimension(val));
            } },
        { DOM_SVG_Y,
            [](const std::string& val, SvgDeclaration& declaration) {
                declaration.SetY(declaration.ParseDimension(val));
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