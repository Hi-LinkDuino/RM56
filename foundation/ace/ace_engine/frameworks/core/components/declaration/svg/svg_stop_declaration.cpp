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

#include "core/components/declaration/svg/svg_stop_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

namespace {

const char DOM_SVG_SRC_STOP_COLOR[] = "stop-color";
const char DOM_SVG_SRC_STOP_OPACITY[] = "stop-opacity";

} // namespace

using namespace Framework;

void SvgStopDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_STOP_ATTR);
}

void SvgStopDeclaration::InitializeStyle()
{
    // self attribute must be initialized first. Otherwise, may be initialized as a base attribute.
    MaybeResetAttribute<SvgStopAttribute>(AttributeTag::SPECIALIZED_ATTR);
}

bool SvgStopDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetSpecializedValue(attr);
}

bool SvgStopDeclaration::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    return SetSpecializedValue(style);
}

bool SvgStopDeclaration::SetSpecializedValue(const std::pair<std::string, std::string>& attr)
{
    static const LinearMapNode<void (*)(const std::string&, SvgStopDeclaration&)> attrs[] = {
        { DOM_SVG_OFFSET,
            [](const std::string& val, SvgStopDeclaration& declaration) {
                declaration.SetOffset(declaration.ParseDimension(val));
            } },
        { DOM_SVG_SRC_STOP_COLOR,
            [](const std::string& val, SvgStopDeclaration& declaration) {
                Color color = (val == VALUE_NONE ? Color::TRANSPARENT : declaration.GetColor(val));
                declaration.SetColor(color);
            } },
        { DOM_SVG_SRC_STOP_OPACITY,
            [](const std::string& val, SvgStopDeclaration& declaration) {
                declaration.SetOpacity(declaration.ParseDouble(val));
            } },
        { DOM_SVG_STOP_COLOR,
            [](const std::string& val, SvgStopDeclaration& declaration) {
                Color color = (val == VALUE_NONE ? Color::TRANSPARENT : declaration.GetColor(val));
                declaration.SetColor(color);
            } },
        { DOM_SVG_STOP_OPACITY,
            [](const std::string& val, SvgStopDeclaration& declaration) {
                declaration.SetOpacity(declaration.ParseDouble(val));
            } },
    };
    auto attrIter = BinarySearchFindIndex(attrs, ArraySize(attrs), attr.first.c_str());
    if (attrIter != -1) {
        attrs[attrIter].value(attr.second, *this);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace
