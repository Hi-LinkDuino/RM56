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

#ifndef HDI_PARCEL_UTILS_V1_0_H
#define HDI_PARCEL_UTILS_V1_0_H

#include <message_parcel.h>
#include "hdf_sbuf_ipc.h"
#include "display_type.h"
#include "hdf_log.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class ParcelUtils {
public:
    static int32_t UnpackAllocInfo(MessageParcel &data, AllocInfo *pAllocInfo)
    {
        if (pAllocInfo == nullptr) {
            return DISPLAY_NULL_PTR;
        }
        pAllocInfo->width  = data.ReadUint32();
        pAllocInfo->height = data.ReadUint32();
        pAllocInfo->usage  = data.ReadUint64();
        pAllocInfo->format = static_cast<PixelFormat>(data.ReadUint32());
        pAllocInfo->expectedSize = data.ReadUint32();
        return DISPLAY_SUCCESS;
    }

    static int32_t PackAllocInfo(MessageParcel &data, const AllocInfo *pAllocInfo)
    {
        if (pAllocInfo == nullptr) {
            return DISPLAY_NULL_PTR;
        }
        if (!data.WriteUint32(pAllocInfo->width)) {
            HDF_LOGE("%{public}s: write AllocInfo width failed", __func__);
            return DISPLAY_PARAM_ERR;
        }
        if (!data.WriteUint32(pAllocInfo->height)) {
            HDF_LOGE("%{public}s: write AllocInfo height failed", __func__);
            return DISPLAY_PARAM_ERR;
        }
        if (!data.WriteUint64(pAllocInfo->usage)) {
            HDF_LOGE("%{public}s: write AllocInfo usage failed", __func__);
            return DISPLAY_PARAM_ERR;
        }
        if (!data.WriteUint32(pAllocInfo->format)) {
            HDF_LOGE("%{public}s: write AllocInfo format failed", __func__);
            return DISPLAY_PARAM_ERR;
        }
        if (!data.WriteUint32(pAllocInfo->expectedSize)) {
            HDF_LOGE("%{public}s: write AllocInfo type failed", __func__);
            return DISPLAY_PARAM_ERR;
        }
        return DISPLAY_SUCCESS;
    }
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_PARCEL_UTILS_V1_0_H
