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

#ifndef GRAPHIC_LITE_POINT_EVENT_INJECTOR_H
#define GRAPHIC_LITE_POINT_EVENT_INJECTOR_H

#include "graphic_config.h"
#if ENABLE_DEBUG
#include "dock/pointer_input_device.h"
#include "gfx_utils/list.h"

namespace OHOS {
class PointEventInjector : public PointerInputDevice {
public:
    PointEventInjector() : lastX_(0), lastY_(0), lastState_(0) {}
    ~PointEventInjector();
    bool SetPointEvent(const DeviceData& data);
    bool Read(DeviceData& data) override;
    uint16_t GetLeftSize() const;
#if ENABLE_WINDOW
    void SetWindowId(int32_t windowId);
#endif

private:
    List<DeviceData*> dataList_;
    int16_t lastX_;
    int16_t lastY_;
    uint16_t lastState_;
#if ENABLE_WINDOW
    int32_t windowId_ = -1;
#endif
};
}
#endif // ENABLE_DEBUG
#endif // GRAPHIC_LITE_POINT_EVENT_INJECTOR_H