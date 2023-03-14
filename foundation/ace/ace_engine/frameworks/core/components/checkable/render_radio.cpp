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

#include "core/components/checkable/render_radio.h"

#include "base/json/json_util.h"
#include "base/log/event_report.h"
#include "core/animation/curves.h"
#include "core/common/frontend.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {
namespace {

constexpr int DEFAULT_RADIO_ANIMATION_DURATION = 300;
constexpr double DEFAULT_MID_TIME_SLOT = 0.5;
constexpr double DEFAULT_END_TIME_SLOT = 1.0;
constexpr double DEFAULT_SHRINK_TIME_SLOT = 0.9;

} // namespace

void RenderRadio::Update(const RefPtr<Component>& component)
{
    RenderCheckable::Update(component);
    const auto& radio = AceType::DynamicCast<RadioComponent<std::string>>(component);
    component_ = radio;
    if (!radio) {
        LOGE("cast to radio component failed");
        EventReport::SendRenderException(RenderExcepType::RENDER_COMPONENT_ERR);
        return;
    }
    radio->SetGroupValueUpdateHandler([weak = AceType::WeakClaim(this)](const std::string& groupValue) {
        LOGD("group value changed");
        auto renderRadio = weak.Upgrade();
        if (renderRadio && renderRadio->UpdateGroupValue(groupValue)) {
            renderRadio->MarkNeedRender();
        }
    });

    radioInnerSizeRatio_ = radio->GetRadioInnerSizeRatio();

    if (radio->GetUpdateType() == UpdateType::ALL) {
        radioValue_ = radio->GetValue();
        groupValue_ = radio->GetGroupValue();
    }
    bool originChecked = radio->GetOriginChecked();
    if (!groupValue_.empty()) {
        checked_ = radioValue_ == groupValue_;
    } else {
        checked_ = originChecked;
    }
    auto theme = GetTheme<RadioTheme>();
    if (theme) {
        borderWidth_ = theme->GetBorderWidth();
    }
    UpdateUIStatus();
    // set check animation engine
    if (!onController_) {
        onController_ = AceType::MakeRefPtr<Animator>(GetContext());
        onController_->AddStartListener(Animator::StatusCallback([weak = AceType::WeakClaim(this)]() {
            auto radio = weak.Upgrade();
            if (radio) {
                radio->OnAnimationStart();
            }
        }));
    }
    // set uncheck animation engine
    if (!offController_) {
        offController_ = AceType::MakeRefPtr<Animator>(GetContext());
        offController_->AddStopListener(Animator::StatusCallback([weak = AceType::WeakClaim(this)]() {
            auto radio = weak.Upgrade();
            if (radio) {
                radio->OffAnimationEnd();
            }
        }));
    }
    groupValueChangedListener_ = radio->GetGroupValueChangedListener();
    pointScale_ = radio->GetRadioInnerSizeRatio();
    UpdateAccessibilityAttr();
}

void RenderRadio::UpdateAccessibilityAttr() const
{
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    accessibilityNode->SetCheckedState(checked_);
    if (accessibilityNode->GetClicked()) {
        accessibilityNode->SetClicked(false);
        auto context = context_.Upgrade();
        if (context) {
            AccessibilityEvent radioEvent;
            radioEvent.nodeId = accessibilityNode->GetNodeId();
            radioEvent.eventType = "click";
            context->SendEventToAccessibility(radioEvent);
        }
    }
}

bool RenderRadio::UpdateGroupValue(const std::string& groupValue)
{
    LOGD("update group value");
    groupValue_ = groupValue;

    bool needRender = false;

    if (!checked_ && isClicked_) {
        checked_ = true;
        isClicked_ = false;
        needRender = true;
        UpdateUIStatus();
    } else if (checked_) {
        checked_ = false;
        needRender = true;
        UpdateAnimation(false);
        offController_->Play();
    }
    std::string checked = checked_ ? "true" : "false";
    std::string result = std::string("\"change\",{\"checked\":")
                             .append(checked)
                             .append(",\"value\":\"")
                             .append(groupValue_)
                             .append("\"},null");
    auto context = context_.Upgrade();
    if (context && context->GetFrontend() && context->GetFrontendType() == FrontendType::JS_CARD) {
        auto json = JsonUtil::Create(true);
        json->Put("checkedItem", groupValue_.c_str());
        result = json->ToString();
    }
    OnHandleChangedResult(result);
    if (changeEvent_) {
        changeEvent_(result);
    }
    if (valueChangeEvent_) {
        valueChangeEvent_(groupValue_);
    }
    return needRender;
}

void RenderRadio::PerformLayout()
{
    RenderCheckable::PerformLayout();
    outCircleRadius_ = drawSize_.Width() / 2.0;
    innerCircleRadius_ = outCircleRadius_ * radioInnerSizeRatio_;
}

void RenderRadio::HandleClick()
{
    if (!checked_) {
        isClicked_ = true;
        if (groupValueChangedListener_) {
            LOGE("groupValueChangedListener_");
            groupValueChangedListener_(radioValue_);
        }
    }
    UpdateAnimation(true);
    onController_->Play();

    if (clickEvent_) {
        clickEvent_();
    }
    if (onClick_) {
        onClick_();
    }
    if (onChange_) {
        onChange_(checked_);
    }
}

void RenderRadio::OffAnimationEnd()
{
    // after the animation stopped, set the stage back to unchecked
    UpdateUIStatus();
    UpdateAccessibilityAttr();
}

void RenderRadio::OnAnimationStart()
{
    // set the stage to checked and then start animation
    UpdateAccessibilityAttr();
}

void RenderRadio::UpdateAnimation(bool isOn)
{
    if (!onController_ || !offController_) {
        LOGE("update animation failed due to controller is null");
        return;
    }
    RefPtr<KeyframeAnimation<double>> shrinkEngine = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    RefPtr<KeyframeAnimation<double>> selectEngine = AceType::MakeRefPtr<KeyframeAnimation<double>>();
    onController_->ClearInterpolators();
    offController_->ClearInterpolators();
    auto shrinkFrameStart = AceType::MakeRefPtr<Keyframe<double>>(0.0, 1.0);
    auto shrinkFrameMid = AceType::MakeRefPtr<Keyframe<double>>(DEFAULT_MID_TIME_SLOT, DEFAULT_SHRINK_TIME_SLOT);
    shrinkFrameMid->SetCurve(Curves::FRICTION);
    auto shrinkFrameEnd = AceType::MakeRefPtr<Keyframe<double>>(DEFAULT_END_TIME_SLOT, 1.0);
    shrinkFrameEnd->SetCurve(Curves::FRICTION);
    shrinkEngine->AddKeyframe(shrinkFrameStart);
    shrinkEngine->AddKeyframe(shrinkFrameMid);
    shrinkEngine->AddKeyframe(shrinkFrameEnd);
    shrinkEngine->AddListener(Animation<double>::ValueCallback([weak = AceType::WeakClaim(this)](double value) {
        auto radio = weak.Upgrade();
        if (radio) {
            radio->totalScale_ = value;
            radio->MarkNeedRender();
        }
    }));

    auto selectFrameStart = AceType::MakeRefPtr<Keyframe<double>>(0.0, isOn ? 1.0 : radioInnerSizeRatio_);
    auto selectFrameMid = AceType::MakeRefPtr<Keyframe<double>>(DEFAULT_MID_TIME_SLOT, 0.0);
    selectFrameMid->SetCurve(Curves::FRICTION);
    auto selectFrameEnd =
        AceType::MakeRefPtr<Keyframe<double>>(DEFAULT_END_TIME_SLOT, isOn ? radioInnerSizeRatio_ : 1.0);
    selectFrameEnd->SetCurve(Curves::FRICTION);
    selectEngine->AddKeyframe(selectFrameStart);
    selectEngine->AddKeyframe(selectFrameMid);
    selectEngine->AddKeyframe(selectFrameEnd);
    selectEngine->AddListener(Animation<double>::ValueCallback([weak = AceType::WeakClaim(this)](double value) {
        auto radio = weak.Upgrade();
        if (radio) {
            radio->pointScale_ = value;
            radio->MarkNeedRender();
        }
    }));
    if (isOn) {
        onController_->AddInterpolator(shrinkEngine);
        onController_->AddInterpolator(selectEngine);
        onController_->SetDuration(DEFAULT_RADIO_ANIMATION_DURATION);
    } else {
        offController_->AddInterpolator(shrinkEngine);
        offController_->AddInterpolator(selectEngine);
        offController_->SetDuration(DEFAULT_RADIO_ANIMATION_DURATION);
    }
}

} // namespace OHOS::Ace
