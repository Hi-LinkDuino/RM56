/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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
#ifndef GRAPHIC_LITE_TOUCH_INPUT_H
#define GRAPHIC_LITE_TOUCH_INPUT_H

#include "dock/pointer_input_device.h"
#include "touch_if.h"

namespace OHOS {
class TouchInput : public PointerInputDevice {
public:
    TouchInput(){}
    virtual ~TouchInput() {}
    static TouchInput* GetInstance();
    bool Read(DeviceData& data) override;

private:
    bool IsValidTouchMsg(struct touch_msg *msg);
    bool init = false;
    DevHandle handle = nullptr;
    struct touch_msg msg = {0};
};
} // namespace OHOS
#endif // GRAPHIC_LITE_TOUCH_INPUT_H
