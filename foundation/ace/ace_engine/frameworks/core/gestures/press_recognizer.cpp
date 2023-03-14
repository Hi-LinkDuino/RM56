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

#include "core/gestures/press_recognizer.h"

namespace OHOS::Ace {
namespace {

constexpr int32_t PRESS_TIMEOUT = 30;
constexpr double MAX_THRESHOLD = 2.0;

} // namespace

void PressRecognizer::OnAccepted(size_t touchId)
{
    LOGD("press gesture has been accepted! the touch id is %{public}zu", touchId);
    state_ = DetectState::DETECTED;
    if (onPress_) {
        PressInfo info(touchId);
        info.SetTimeStamp(trackPoint_.time);
        info.SetGlobalLocation(trackPoint_.GetOffset()).SetLocalLocation(trackPoint_.GetOffset() - coordinateOffset_);
        onPress_(info);
    }
}

void PressRecognizer::OnRejected(size_t touchId)
{
    LOGD("press gesture has been rejected! the touch id is %{public}zu", touchId);
    deadlineTimer_.Cancel();
    state_ = DetectState::READY;
}

void PressRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    LOGD("press recognizer receives touch down event, begin to detect press event");
    if (state_ == DetectState::READY) {
        auto context = context_.Upgrade();
        if (!context) {
            LOGE("fail to detect press gesture due to context is nullptr");
            return;
        }
        trackPoint_ = event;
        state_ = DetectState::DETECTING;
        deadlineTimer_.Reset([weakPtr = AceType::WeakClaim(this)] {
            auto refPtr = weakPtr.Upgrade();
            if (refPtr) {
                refPtr->HandleOverdueDeadline();
            } else {
                LOGE("fail to handle overdue deadline due to context is nullptr");
            }
        });
        auto taskExecutor = SingleTaskExecutor::Make(context->GetTaskExecutor(), TaskExecutor::TaskType::UI);
        taskExecutor.PostDelayedTask(deadlineTimer_, PRESS_TIMEOUT);
    } else {
        LOGW("the state is not ready for detecting press gesture");
    }
}

void PressRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    LOGD("press recognizer receives touch up event");
    if (state_ == DetectState::DETECTING) {
        LOGD("this gesture is not press, try to reject it");
        OnRejected(trackPoint_.id);
    } else if (state_ == DetectState::DETECTED) {
        if (onPressCancel_) {
            onPressCancel_();
        }
    }
    state_ = DetectState::READY;
}

void PressRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    LOGD("press recognizer receives touch move event");
    if (state_ == DetectState::DETECTING) {
        Offset offset = event.GetOffset() - trackPoint_.GetOffset();
        if (offset.GetDistance() > MAX_THRESHOLD) {
            LOGD("this gesture is not press, try to reject it");
            OnRejected(event.id);
        }
    } else if (state_ == DetectState::DETECTED) {
        Offset offset = event.GetOffset() - trackPoint_.GetOffset();
        if (offset.GetDistance() > MAX_THRESHOLD && onPressCancel_) {
            onPressCancel_();
        }
    }
}

void PressRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    LOGD("press recognizer receives touch cancel event");
    if (state_ == DetectState::DETECTING) {
        LOGD("cancel press gesture detect, try to reject it");
        OnRejected(event.id);
    }
    state_ = DetectState::READY;
}

void PressRecognizer::HandleOverdueDeadline()
{
    if (state_ == DetectState::DETECTING) {
        LOGD("this gesture is press, try to accept it");
        OnAccepted(trackPoint_.id);
    } else {
        LOGW("the state is not detecting for accept press gesture");
    }
}

} // namespace OHOS::Ace
