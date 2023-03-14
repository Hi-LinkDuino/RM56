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

#ifndef GRAPHIC_LITE_KEY_INPUT_H
#define GRAPHIC_LITE_KEY_INPUT_H

#include <QKeyEvent>

#include "config.h"
#include "dock/key_input_device.h"

namespace OHOS {
#if USE_KEY
class KeyInput : public KeyInputDevice {
public:
    KeyInput() {}
    virtual ~KeyInput() {}
    static KeyInput* GetInstance();
    bool Read(DeviceData& data) override;
    void KeyHandler(QKeyEvent* event);

private:
    bool leftButtonDown_;
    int16_t lastX_;
    int16_t lastY_;
};
#endif // USE_KEY
} // namespace OHOS
#endif // GRAPHIC_LITE_KEY_INPUT_H
