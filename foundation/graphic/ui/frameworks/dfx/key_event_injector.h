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

#ifndef GRAPHIC_LITE_KEY_EVENT_INJECTOR
#define GRAPHIC_LITE_KEY_EVENT_INJECTOR

#include "graphic_config.h"
#if ENABLE_DEBUG
#include "dock/key_input_device.h"
#include "events/key_event.h"
#include "gfx_utils/list.h"

namespace OHOS {
class KeyEventInjector : public KeyInputDevice {
public:
    KeyEventInjector() : lastKeyId_(0), lastState_(INVALID_KEY_STATE) {}
    ~KeyEventInjector();
    bool Read(DeviceData& data) override;
    bool SetKey(const DeviceData& data);
    uint8_t GetLeftSize() const;
private:
    List<DeviceData*> dataList_;
    uint16_t lastKeyId_;
    uint16_t lastState_;
};
}

#endif  // ENABLE_DEBUG
#endif  // GRAPHIC_LITE_KEY_EVENT_INJECTOR
