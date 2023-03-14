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

#include "key_input.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qvariant.h>

#include "events/key_event.h"

namespace OHOS {
#if USE_KEY
namespace {
static uint16_t g_lastKeyId = 0;
static uint16_t g_lastKeyState = INVALID_KEY_STATE;
} // namespace

KeyInput* KeyInput::GetInstance()
{
    static KeyInput keyInput;
    return &keyInput;
}

bool KeyInput::Read(DeviceData& data)
{
    data.keyId = g_lastKeyId;
    data.state = g_lastKeyState;
    g_lastKeyState = INVALID_KEY_STATE;
    return false;
}

void KeyInput::KeyHandler(QKeyEvent* event)
{
    if (event == nullptr) {
        return;
    }
    switch (event->type()) {
        case QKeyEvent::Type::KeyPress:
            g_lastKeyState = InputDevice::STATE_PRESS;
            break;
        case QKeyEvent::Type::KeyRelease:
            g_lastKeyState = InputDevice::STATE_RELEASE;
            break;
        default:
            break;
    }
}
#endif // USE_KEY
} // namespace OHOS
