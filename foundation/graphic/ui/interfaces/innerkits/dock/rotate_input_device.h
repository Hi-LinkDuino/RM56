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

#ifndef GRAPHIC_LITE_ROTATE_INPUT_DEVICE_H
#define GRAPHIC_LITE_ROTATE_INPUT_DEVICE_H

#include "dock/input_device.h"
#include "components/ui_view.h"

#if ENABLE_ROTATE_INPUT
namespace OHOS {
/** @brief A Rotate input device. */
class RotateInputDevice : public InputDevice {
public:
    /** @brief Constructor */
    RotateInputDevice();
    /** @brief Destructor */
    virtual ~RotateInputDevice(){}

    void SetRotateInputThreshold(int16_t threshold)
    {
        threshold_ = threshold;
    }

protected:
    void DispatchEvent(const DeviceData& data) override;
    bool rotateStart_;
    int16_t threshold_;
    int16_t cachedRotation_;
};
} // namespace OHOS

#endif // GRAPHIC_LITE_ROTATE_INPUT_DEVICE_H
#endif
