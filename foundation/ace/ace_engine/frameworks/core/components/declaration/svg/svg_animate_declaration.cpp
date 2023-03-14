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

#include "core/components/declaration/svg/svg_animate_declaration.h"

#include "base/utils/string_utils.h"
#include "core/components/declaration/common/declaration_constants.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

using namespace Framework;

using SvgMap = std::unordered_map<std::string, void (*)(const std::string&, SvgAnimateDeclaration&)>;

void SvgAnimateDeclaration::InitSpecialized()
{
    AddSpecializedAttribute(DeclarationConstants::DEFAULT_SVG_ANIMATE_ATTR);
}

bool SvgAnimateDeclaration::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    return SetAnimateAttr(attr);
}

bool SvgAnimateDeclaration::SetAnimateAttr(const std::pair<std::string, std::string>& attr)
{
    static const SvgMap svgMap = {
        { DOM_SVG_ANIMATION_BEGIN,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetBegin(ConvertTimeStr(val));
            } },
        { DOM_SVG_ANIMATION_DUR,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                if (val == "indefinite") {
                    declaration.SetDur(0);
                } else {
                    declaration.SetDur(ConvertTimeStr(val));
                }
            } },
        { DOM_SVG_ANIMATION_END,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetEnd(ConvertTimeStr(val));
            } },
        { DOM_SVG_ANIMATION_REPEAT_COUNT,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                if (val == "indefinite") {
                    declaration.SetRepeatCount(-1);
                } else {
                    declaration.SetRepeatCount(StringToInt(val));
                }
            } },
        { DOM_SVG_ANIMATION_FILL,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetFillMode(val);
            } },
        { DOM_SVG_ANIMATION_CALC_MODE,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetCalcMode(val);
            } },
        { DOM_SVG_ANIMATION_VALUES,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                auto& attrs = declaration.MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
                StringUtils::SplitStr(val, ";", attrs.values);
            } },
        { DOM_SVG_ANIMATION_KEY_TIMES,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                auto& attrs = declaration.MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
                StringUtils::StringSpliter(val, ';', attrs.keyTimes);
            } },
        { DOM_SVG_ANIMATION_KEY_SPLINES,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                auto& attrs = declaration.MaybeResetAttribute<SvgAnimateAttribute>(AttributeTag::SPECIALIZED_ATTR);
                StringUtils::SplitStr(val, ";", attrs.keySplines);
            } },
        { DOM_SVG_ANIMATION_FROM,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetFrom(val);
            } },
        { DOM_SVG_ANIMATION_TO,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetTo(val);
            } },
        { DOM_SVG_ANIMATION_ATTRIBUTE_NAME,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetAttributeName(val);
            } },
        { DOM_SVG_ANIMATION_KEY_POINTS,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                if (val.empty()) {
                    return;
                }
                auto& attrs = declaration.MaybeResetAttribute<SvgAnimateAttribute>(
                    AttributeTag::SPECIALIZED_ATTR);
                StringUtils::StringSpliter(val, ';', attrs.keyPoints);
            } },
        { DOM_SVG_ANIMATION_PATH,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetPath(val);
            } },
        { DOM_SVG_ANIMATION_ROTATE,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetRotate(val);
            } },
        { DOM_SVG_ANIMATION_TYPE,
            [](const std::string& val, SvgAnimateDeclaration& declaration) {
                declaration.SetTransformType(val);
            } },
    };
    auto attrIter = svgMap.find(attr.first);
    if (attrIter != svgMap.end()) {
        attrIter->second(attr.second, *this);
        return true;
    }
    return false;
}

} // namespace OHOS::Ace