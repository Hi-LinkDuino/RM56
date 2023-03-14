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

#include "light_interface_impl.h"
#include <hdf_base.h>
#include <hdf_log.h>
#include "light_if.h"

#define HDF_LOG_TAG           hdf_light_dal

namespace OHOS {
namespace HDI {
namespace Light {
namespace V1_0 {
int32_t LightInterfaceImpl::GetLightInfo(std::vector<HdfLightInfo>& info)
{
    const struct LightInterface *lightInterface = NewLightInterfaceInstance();
    if (lightInterface == nullptr || lightInterface->GetLightInfo == nullptr) {
        HDF_LOGE("%{public}s: get light Module instance failed", __func__);
        return HDF_FAILURE;
    }

    struct LightInfo *lightInfo = nullptr;
    uint32_t count = 0;
    int32_t ret = lightInterface->GetLightInfo(&lightInfo, &count);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %d", __func__, ret);
        return ret;
    }

    while (count--) {
        HdfLightInfo hdfLightInfo;
        hdfLightInfo.lightId = lightInfo->lightType;
        hdfLightInfo.reserved = lightInfo->reserved;
        info.push_back(hdfLightInfo);
        lightInfo++;
    }
    return HDF_SUCCESS;
}

int32_t LightInterfaceImpl::TurnOnLight(int32_t lightId, const HdfLightEffect& effect)
{
    const struct LightInterface *lightInterface = NewLightInterfaceInstance();
    if (lightInterface == nullptr || lightInterface->TurnOnLight == nullptr) {
        HDF_LOGE("%{public}s: get light Module instance failed", __func__);
        return HDF_FAILURE;
    }

    LightEffect lightEffect;
    lightEffect.lightBrightness = effect.lightBrightness;
    lightEffect.flashEffect.flashMode = effect.flashEffect.flashMode;
    lightEffect.flashEffect.onTime = effect.flashEffect.onTime;
    lightEffect.flashEffect.offTime = effect.flashEffect.offTime;
    int32_t ret = lightInterface->TurnOnLight(lightId, &lightEffect);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %d", __func__, ret);
    }

    return ret;
}

int32_t LightInterfaceImpl::TurnOffLight(int32_t lightId)
{
    const struct LightInterface *lightInterface = NewLightInterfaceInstance();
    if (lightInterface == nullptr || lightInterface->TurnOffLight == nullptr) {
        HDF_LOGE("%{public}s: get light Module instance failed", __func__);
        return HDF_FAILURE;
    }
    int32_t ret = lightInterface->TurnOffLight(lightId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%{public}s failed, error code is %d", __func__, ret);
    }

    return ret;
}
} // V1_0
} // Light
} // HDI
} // OHOS
