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

#include "dock/virtual_input_device.h"
#include "components/root_view.h"
#include "events/virtual_device_event.h"

#if USE_VIRTUAL_DEVICE
namespace OHOS {
void VirtualInputDevice::DispatchEvent(const DeviceData& data)
{
    if (data.state != INVALID_VIRTUAL_INPUT_STATE) {
        VirtualDeviceEvent event(data.type, data.state);
        RootView::GetInstance()->OnVirtualDeviceEvent(event);
    }
}
} // namespace OHOS
#endif
