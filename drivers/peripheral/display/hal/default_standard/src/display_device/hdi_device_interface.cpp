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

#include "hdi_device_interface.h"
#include <vector>
#include "display_common.h"
#include "drm_device.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
std::vector<std::shared_ptr<HdiDeviceInterface>> HdiDeviceInterface::DiscoveryDevice()
{
    DISPLAY_LOGD();
    int ret;
    std::vector<std::shared_ptr<HdiDeviceInterface>> devices;
    std::shared_ptr<HdiDeviceInterface> drmDevice = DrmDevice::Create();
    if (!drmDevice) {
        DISPLAY_LOGE("can not create drm device");
        return devices;
    }
    ret = drmDevice->Init();
    if (ret == DISPLAY_SUCCESS) {
        DISPLAY_LOGD("drm device init success");
        devices.push_back(std::move(drmDevice));
    } else {
        DISPLAY_LOGE("drm device init failed");
    }
    return devices;
}
}
}
}