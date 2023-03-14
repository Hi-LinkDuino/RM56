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

#include "frameworks/bridge/common/dom/dom_image_animator.h"

#include "base/log/event_report.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::Framework {
namespace {

constexpr int32_t MS_TO_S = 1000;
const char* STATE_PLAYING = "playing";
const char* STATE_PAUSED = "paused";
const char* STATE_STOPPED = "stopped";
const char* ITERATIONS_INFINITE = "infinite";

} // namespace

DOMImageAnimator::DOMImageAnimator(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName)
{
    imageAnimator_ = AceType::MakeRefPtr<ImageAnimatorComponent>(nodeName);
}

bool DOMImageAnimator::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    if (!imageAnimator_) {
        LOGE("SetSpecializedAttr failed, The imageAnimator is not created.");
        return false;
    }
    if (attr.first == DOM_ITERATION) {
        if (attr.second == ITERATIONS_INFINITE) {
            imageAnimator_->SetIteration(ANIMATION_REPEAT_INFINITE);
        } else {
            imageAnimator_->SetIteration(StringToInt(attr.second));
        }
        return true;
    }
    if (attr.first == DOM_PREDECODE) {
        imageAnimator_->SetPreDecode(StringToInt(attr.second));
        return true;
    }
    if (attr.first == DOM_DURATION) {
        auto val = attr.second;
        if (val.find("ms") != std::string::npos) {
            imageAnimator_->SetDuration(StringToInt(val));
        } else if (val.find('s') != std::string::npos) {
            imageAnimator_->SetDuration(StringToInt(val) * MS_TO_S);
        } else {
            imageAnimator_->SetDuration(StringToInt(val));
        }
        return true;
    }
    if (attr.first == DOM_FIXEDSIZE) {
        imageAnimator_->SetIsFixedSize(StringToBool(attr.second));
        return true;
    }
    if (attr.first == DOM_REVERSE) {
        imageAnimator_->SetIsReverse(StringToBool(attr.second));
        return true;
    }
    if (attr.first == DOM_FILLMODE) {
        imageAnimator_->SetFillMode(StringToFillMode(attr.second));
        return true;
    }
    return false;
}

bool DOMImageAnimator::AddSpecializedEvent(int32_t pageId, const std::string& event)
{
    if (!imageAnimator_) {
        LOGE("AddSpecializedEvent failed, The imageAnimator is not created.");
        return false;
    }
    const auto& controller = imageAnimator_->GetImageAnimatorController();
    if (!controller) {
        LOGE("AddSpecializedEvent failed, controller is null.");
        return false;
    }
    if (event == DOM_IMAGE_ANIMATOR_START) {
        auto startEvent = EventMarker(GetNodeIdForEvent(), event, pageId);
        controller->SetStartEvent(startEvent);
        return true;
    } else if (event == DOM_IMAGE_ANIMATOR_STOP) {
        auto stopEvent = EventMarker(GetNodeIdForEvent(), event, pageId);
        controller->SetStopEvent(stopEvent);
        return true;
    } else if (event == DOM_IMAGE_ANIMATOR_PAUSE) {
        auto pauseEvent = EventMarker(GetNodeIdForEvent(), event, pageId);
        controller->SetPauseEvent(pauseEvent);
        return true;
    } else if (event == DOM_IMAGE_ANIMATOR_RESUME) {
        auto resumeEvent = EventMarker(GetNodeIdForEvent(), event, pageId);
        controller->SetResumeEvent(resumeEvent);
        return true;
    } else {
        return false;
    }
}

void DOMImageAnimator::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (!imageAnimator_) {
        LOGE("CallSpecializedMethod failed, The imageAnimator is not created.");
        return;
    }
    const auto& controller = imageAnimator_->GetImageAnimatorController();
    if (!controller) {
        LOGE("CallSpecializedMethod failed, controller is null.");
        return;
    }
    controller->CallAnimationFunc(method);
}

const char* DOMImageAnimator::GetState() const
{
    if (!imageAnimator_) {
        LOGE("GetState failed, The imageAnimator is not created.");
        return STATE_STOPPED;
    }
    const auto& controller = imageAnimator_->GetImageAnimatorController();
    if (!controller) {
        LOGE("GetState failed, controller is null.");
        return STATE_STOPPED;
    }
    auto currentStatus = controller->CallAnimatorGetStatusFunc();
    if (currentStatus == Animator::Status::PAUSED) {
        return STATE_PAUSED;
    } else if (currentStatus == Animator::Status::RUNNING) {
        return STATE_PLAYING;
    } else {
        // IDLE and STOP
        return STATE_STOPPED;
    }
}

void DOMImageAnimator::PrepareSpecializedComponent()
{
    if (!imageAnimator_) {
        LOGE("The imageAnimator is not created.");
        EventReport::SendComponentException(ComponentExcepType::IMAGE_ANIMATOR_ERR);
        return;
    }
    if (declaration_) {
        auto& borderStyle = static_cast<CommonBorderStyle&>(declaration_->GetStyle(StyleTag::COMMON_BORDER_STYLE));
        if (borderStyle.IsValid() && borderStyle.border.HasRadius()) {
            imageAnimator_->SetBorder(borderStyle.border);
        }
    }
    imageAnimator_->SetImageProperties(imagesAttr_);
}

} // namespace OHOS::Ace::Framework
