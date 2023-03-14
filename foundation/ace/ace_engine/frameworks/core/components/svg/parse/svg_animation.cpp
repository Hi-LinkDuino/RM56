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

#include "frameworks/core/components/svg/parse/svg_animation.h"

#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace {

namespace {

const char DOM_SVG_SRC_REPEAT_COUNT[] = "repeatCount";
const char DOM_SVG_SRC_ATTRIBUTE_NAME[] = "attributeName";

} // namespace

using namespace Framework;
using SvgMap = std::unordered_map<std::string, void (*)(const std::string&, const RefPtr<SvgAnimateComponent>&)>;

SvgAnimation::SvgAnimation(SvgAnimateType svgAnimateType) : SvgNode()
{
    component_ = AceType::MakeRefPtr<SvgAnimateComponent>("svg_animate_nodeId", "animate", svgAnimateType);
}

RefPtr<SvgNode> SvgAnimation::CreateAnimate()
{
    return AceType::MakeRefPtr<SvgAnimation>(SvgAnimateType::ANIMATE);
}

RefPtr<SvgNode> SvgAnimation::CreateAnimateTransform()
{
    return AceType::MakeRefPtr<SvgAnimation>(SvgAnimateType::TRANSFORM);
}

RefPtr<SvgNode> SvgAnimation::CreateAnimateMotion()
{
    return AceType::MakeRefPtr<SvgAnimation>(SvgAnimateType::MOTION);
}

void SvgAnimation::SetAttr(const std::string& name, const std::string& value)
{
    static const SvgMap svgMap = {
        { DOM_SVG_ANIMATION_BEGIN,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetBegin(ConvertTimeStr(val));
            } },
        { DOM_SVG_ANIMATION_DUR,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                if (val == "indefinite") {
                    animateComponent->SetDur(0);
                } else {
                    animateComponent->SetDur(ConvertTimeStr(val));
                }
            } },
        { DOM_SVG_ANIMATION_END,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetEnd(ConvertTimeStr(val));
            } },
        { DOM_SVG_SRC_REPEAT_COUNT,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                if (val == "indefinite") {
                    animateComponent->SetRepeatCount(-1);
                } else {
                    animateComponent->SetRepeatCount(StringToInt(val));
                }
            } },
        { DOM_SVG_ANIMATION_FILL,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                Fill fillMode = (val == "freeze" ? Fill::FREEZE : Fill::REMOVE);
                animateComponent->SetFillMode(fillMode);
            } },
        { DOM_SVG_ANIMATION_CALC_MODE,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                CalcMode calcMode;
                if (val == "discrete") {
                    calcMode = CalcMode::DISCRETE;
                } else if (val == "paced") {
                    calcMode = CalcMode::PACED;
                } else if (val == "spline") {
                    calcMode = CalcMode::SPLINE;
                } else {
                    calcMode = CalcMode::LINEAR;
                }
                animateComponent->SetCalcMode(calcMode);
            } },
        { DOM_SVG_ANIMATION_VALUES,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                std::vector<std::string> values;
                StringUtils::SplitStr(val, ";", values);
                animateComponent->SetValues(values);
            } },
        { DOM_SVG_ANIMATION_KEY_TIMES,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                if (val.empty()) {
                    return;
                }
                std::vector<double> keyTimes;
                StringUtils::StringSpliter(val, ';', keyTimes);
                animateComponent->SetKeyTimes(keyTimes);
            } },
        { DOM_SVG_ANIMATION_KEY_SPLINES,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                if (val.empty()) {
                    return;
                }
                std::vector<std::string> keySplines;
                StringUtils::SplitStr(val, ";", keySplines);
                animateComponent->SetKeySplines(keySplines);
            } },
        { DOM_SVG_ANIMATION_FROM,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetFrom(val);
            } },
        { DOM_SVG_ANIMATION_TO,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetTo(val);
            } },
        { DOM_SVG_SRC_ATTRIBUTE_NAME,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetAttributeName(val);
            } },
        { DOM_SVG_ANIMATION_KEY_POINTS,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                if (val.empty()) {
                    return;
                }
                std::vector<std::string> keyPoints;
                StringUtils::StringSpliter(val, ';', keyPoints);
                animateComponent->SetKeyPoints(keyPoints);
            } },
        { DOM_SVG_ANIMATION_PATH,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetPath(val);
            } },
        { DOM_SVG_ANIMATION_ROTATE,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetRotate(val);
            } },
        { DOM_SVG_ANIMATION_TYPE,
            [](const std::string& val, const RefPtr<SvgAnimateComponent>& animateComponent) {
                animateComponent->SetTransformType(val);
            } },
    };
    auto attrIter = svgMap.find(name);
    if (attrIter != svgMap.end()) {
        attrIter->second(value, component_);
    }
}

} // namespace OHOS::Ace
