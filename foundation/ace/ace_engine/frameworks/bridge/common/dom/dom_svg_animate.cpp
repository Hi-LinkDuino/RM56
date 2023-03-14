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

#include "frameworks/bridge/common/dom/dom_svg_animate.h"

#include "frameworks/bridge/common/utils/utils.h"
#include "frameworks/core/components/declaration/svg/svg_animate_declaration.h"

namespace OHOS::Ace::Framework {
namespace {

const char ANIMATION_FILL_MODE_FREEZE[] = "freeze";
const char ANIMATION_CALC_MODE_DISCRETE[] = "discrete";
const char ANIMATION_CALC_MODE_PACED[] = "paced";
const char ANIMATION_CALC_MODE_SPLINE[] = "spline";

} // namespace

DOMSvgAnimate::DOMSvgAnimate(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}

void DOMSvgAnimate::PrepareSpecializedComponent()
{
    if (!animateComponent_) {
        animateComponent_ = AceType::MakeRefPtr<SvgAnimateComponent>(std::to_string(GetNodeId()), GetTag());
    }
    SetAnimateAttrs();
}

void DOMSvgAnimate::SetAnimateAttrs()
{
    auto declaration = AceType::DynamicCast<SvgAnimateDeclaration>(declaration_);
    if (!declaration) {
        return;
    }
    animateComponent_->SetBegin(declaration->GetBegin());
    animateComponent_->SetDur(declaration->GetDur());
    animateComponent_->SetEnd(declaration->GetEnd());
    animateComponent_->SetRepeatCount(declaration->GetRepeatCount());
    Fill fillMode = (declaration->GetFillMode() == ANIMATION_FILL_MODE_FREEZE ? Fill::FREEZE : Fill::REMOVE);
    animateComponent_->SetFillMode(fillMode);
    animateComponent_->SetCalcMode(ConvertCalcMode(declaration->GetCalcMode()));
    animateComponent_->SetValues(declaration->GetValues());
    animateComponent_->SetKeyTimes(declaration->GetKeyTimes());
    animateComponent_->SetKeySplines(declaration->GetKeySplines());
    animateComponent_->SetFrom(declaration->GetFrom());
    animateComponent_->SetTo(declaration->GetTo());
    animateComponent_->SetAttributeName(declaration->GetAttributeName());

    if (animateComponent_->GetSvgAnimateType() == SvgAnimateType::MOTION) {
        animateComponent_->SetKeyPoints(declaration->GetKeyPoints());
        animateComponent_->SetPath(declaration->GetPath());
        animateComponent_->SetRotate(declaration->GetRotate());
    }
    if (animateComponent_->GetSvgAnimateType() == SvgAnimateType::TRANSFORM) {
        animateComponent_->SetTransformType(declaration->GetTransformType());
    }
}

CalcMode DOMSvgAnimate::ConvertCalcMode(const std::string& val) const
{
    CalcMode calcMode;
    if (val == ANIMATION_CALC_MODE_DISCRETE) {
        calcMode = CalcMode::DISCRETE;
    } else if (val == ANIMATION_CALC_MODE_PACED) {
        calcMode = CalcMode::PACED;
    } else if (val == ANIMATION_CALC_MODE_SPLINE) {
        calcMode = CalcMode::SPLINE;
    } else {
        calcMode = CalcMode::LINEAR;
    }
    return calcMode;
}

} // namespace OHOS::Ace::Framework
