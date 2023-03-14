/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "click_spring_effect.h"

#include <unordered_map>

namespace OHOS::Ace {
namespace {

struct SpringEffectProperty {
    double scale_;
    double velocity_;
    RefPtr<SpringProperty> springProperty_;
};

const static std::unordered_map<ClickSpringEffectType, SpringEffectProperty> CLICK_SPRING_EFFECT_PROPERTIES = {
    { ClickSpringEffectType::SMALL, { 0.9, 1, AceType::MakeRefPtr<SpringProperty>(0.5, 410.0, 38.0) } },
    { ClickSpringEffectType::MEDIUM, { 0.95, 0.5, AceType::MakeRefPtr<SpringProperty>(0.5, 350.0, 35.0) } },
    { ClickSpringEffectType::LARGE, { 0.95, 0, AceType::MakeRefPtr<SpringProperty>(0.5, 240.0, 28.0) } },
};

} // namespace

ClickSpringEffect::ClickSpringEffect(const WeakPtr<PipelineContext>& context)
{
    controller_ = AceType::MakeRefPtr<Animator>(context);
}

void ClickSpringEffect::FinishPreviousAnimation()
{
    if (controller_ && !controller_->IsStopped()) {
        controller_->Finish();
    }
}

void ClickSpringEffect::ShowAnimation(TouchType touchType, ClickSpringEffectType type)
{
    FinishPreviousAnimation();
    auto propertyPos = CLICK_SPRING_EFFECT_PROPERTIES.find(type);
    if (propertyPos == CLICK_SPRING_EFFECT_PROPERTIES.end()) {
        LOGE("can't find type %{public}d", type);
        return;
    }

    auto& effectProperty = propertyPos->second;
    RefPtr<SpringMotion> springMotion;
    switch (touchType) {
        case TouchType::DOWN:
            springMotion = AceType::MakeRefPtr<SpringMotion>(
                GetScale(), effectProperty.scale_, effectProperty.velocity_, effectProperty.springProperty_);
            break;
        case TouchType::UP:
        case TouchType::CANCEL:
            springMotion = AceType::MakeRefPtr<SpringMotion>(
                GetScale(), 1.0, effectProperty.velocity_, effectProperty.springProperty_);
            break;
        default:
            return;
    }
    if (!springMotion) {
        return;
    }

    springMotion->AddListener([weakEffect = AceType::WeakClaim(this)](double value) {
        auto effect = weakEffect.Upgrade();
        if (effect) {
            effect->SetScale(value);
            effect->MarkRender();
        }
    });

    if (controller_) {
        controller_->PlayMotion(springMotion);
    }
}

void ClickSpringEffect::MarkRender()
{
    auto node = renderNode_.Upgrade();
    if (node) {
        node->MarkNeedLayout();
    }
}

} // namespace OHOS::Ace