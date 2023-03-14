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

#include "core/event/mouse_raw_recognizer.h"

#include "base/log/log.h"

namespace OHOS::Ace {
namespace {

const char ON_MOUSE_EVENT[] = "onMouse";

} // namespace

void MouseRawRecognizer::HandleEvent(const MouseEvent& event)
{
    LOGD("raw mouse recognizer handle event, action is %{public}d, button is %{public}d", event.action, event.button);
    if (onMouse_) {
        onMouse_(CreateMouseEventInfo(event));
    }
    lastEvent_ = event;
}

void MouseRawRecognizer::HandleHoverEvent(MouseState mouseState)
{
    // Ace2.0
    if (onHover_) {
        onHover_(mouseState == MouseState::HOVER);
    }

    // Ace1.0
    if (onMouseHover_ && mouseState == MouseState::HOVER) {
        onMouseHover_();
    } else if (onMouseHoverExit_ && mouseState == MouseState::NONE) {
        onMouseHoverExit_();
    }
}

MouseEventInfo MouseRawRecognizer::CreateMouseEventInfo(const MouseEvent& event) const
{
    MouseEventInfo info(ON_MOUSE_EVENT);
    info.SetMouseGlobalInfo(event);
    info.SetMouseLocationInfo(event - coordinateOffset_);
    info.SetTimeStamp(event.time);
    info.SetSourceDevice(event.sourceType);
    info.SetDeviceId(event.deviceId);
    return info;
}

} // namespace OHOS::Ace