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

#include "core/components/declaration/svg/svg_text_path_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

void SvgTextPathDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_TEXT_PATH_ATTR);
}

void SvgTextPathDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgTextPathAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgTextPathDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgTextPathDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgTextPathDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgTextPathDeclaration&)> attrs[] = {
        { DOM_SVG_ATTR_PATH,
            [](const std::string& val, SvgTextPathDeclaration& declaration) {
                declaration.SetPath(val);
            } },
        { DOM_SVG_START_OFFSET,
            [](const std::string& val, SvgTextPathDeclaration& declaration) {
                declaration.SetStartOffset(declaration.ParseDimension(val));
            } },
        { DOM_VALUE,
            [](const std::string& val, SvgTextPathDeclaration& declaration) {
                declaration.SetTextData(val);
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