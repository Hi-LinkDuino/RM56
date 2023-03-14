/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "thermal_callback_impl.h"
#include <hdf_base.h>
#include <hdf_log.h>

#define HDF_LOG_TAG ThermalCallbackImpl

namespace OHOS {
namespace HDI {
namespace Thermal {
namespace V1_0 {
ThermalCallbackImpl::ThermalEventCallback ThermalCallbackImpl::eventCb_ = nullptr;
int32_t ThermalCallbackImpl::OnThermalDataEvent(const HdfThermalCallbackInfo& event)
{
    int size = static_cast<int>(event.info.size());
    HDF_LOGI("%{public}s event.info.size=%{public}d", __func__, size);
    if (!event.info.empty()) {
        for (auto iter = event.info.begin(); iter != event.info.end(); iter++) {
            HDF_LOGI("%{public}s: type: %{public}s", __func__, iter->type.c_str());
            HDF_LOGI("%{public}s: temp: %{public}d", __func__, iter->temp);
        }
    }
    if (eventCb_ == nullptr) {
        return HDF_FAILURE;
    }
    return eventCb_(event);
}

int32_t ThermalCallbackImpl::RegisterThermalEvent(const ThermalEventCallback &eventCb)
{
    eventCb_ = eventCb;
    return HDF_SUCCESS;
}
} // V1_0
} // Thermal
} // HDI
} // OHOS