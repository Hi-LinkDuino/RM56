/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "mouse_input.h"
#include <QtCore/qcoreevent.h>
#include <QtCore/qvariant.h>

namespace OHOS {
#if USE_MOUSE
MouseInput* MouseInput::GetInstance()
{
    static MouseInput mouseInput;
    return &mouseInput;
}

bool MouseInput::Read(DeviceData& data)
{
    data.point.x = lastX_;
    data.point.y = lastY_;
    data.state = leftButtonDown_ ? STATE_PRESS : STATE_RELEASE;
    return false;
}

void MouseInput::MouseHandler(QMouseEvent* event)
{
    if (event == nullptr) {
        return;
    }
    switch (event->type()) {
        case QMouseEvent::Type::MouseButtonRelease:
            if (event->button() == Qt::LeftButton) {
                leftButtonDown_ = false;
            }
            break;
        case QMouseEvent::Type::MouseButtonPress:
            if (event->button() == Qt::LeftButton) {
                leftButtonDown_ = true;
                lastX_ = event->x();
                lastY_ = event->y();
            }
            break;
        case QMouseEvent::Type::MouseMove:
            lastX_ = event->x();
            lastY_ = event->y();
            break;
        default:
            break;
    }
}
#endif // USE_MOUSE
} // namespace OHOS
