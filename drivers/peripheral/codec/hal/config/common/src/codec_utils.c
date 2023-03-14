/*
 * Copyright (c) 2021 Shenzhen Kaihong DID Co., Ltd.
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

#include "codec_utils.h"
#include "hdf_log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HDF_LOG_TAG "codec_config_common"

void PrintArray(const char *where, const char *name, ResizableArray *array)
{
    uint32_t index;
    
    if (array == NULL) {
        return;
    }
    HDF_LOGI("%{public}s, %{public}s len: %{public}d", where, name, (int32_t)array->actualLen);
    for (index = 0; index < array->actualLen; index++) {
        HDF_LOGI("%{public}s, %{public}s-%{public}d: %{public}d",
            where, name, index, (int32_t)array->element[index]);
    }
}

void PrintCapability(const char *where, CodecCapbility *cap)
{
    int32_t mime = 0;
    if (cap == NULL) {
        HDF_LOGE("%{public}s, null capability!", where);
        return;
    }
    mime = (int32_t)cap->mime;
    if (mime < 0) {
        HDF_LOGE("%{public}s, print invalid capability!", where);
        return;
    }

    HDF_LOGI("%{public}s, --- start print cap ----------------------------", where);
    HDF_LOGI("%{public}s, mime: %{public}d", where, (int32_t)cap->mime);
    HDF_LOGI("%{public}s, type: %{public}d", where, (int32_t)cap->type);
    HDF_LOGI("%{public}s, widthAlignment: %{public}d", where, (int32_t)cap->whAlignment.widthAlignment);
    HDF_LOGI("%{public}s, heightAlignment: %{public}d", where, (int32_t)cap->whAlignment.heightAlignment);
    HDF_LOGI("%{public}s, minwidth: %{public}d", where, (int32_t)cap->minSize.width);
    HDF_LOGI("%{public}s, minHeight: %{public}d", where, (int32_t)cap->minSize.height);
    HDF_LOGI("%{public}s, maxwidth: %{public}d", where, (int32_t)cap->maxSize.width);
    HDF_LOGI("%{public}s, maxheight: %{public}d", where, (int32_t)cap->maxSize.height);
    HDF_LOGI("%{public}s, minBitRate: %{public}d", where, (int32_t)cap->minBitRate);
    HDF_LOGI("%{public}s, maxBitRate: %{public}d", where, (int32_t)cap->maxBitRate);
    PrintArray(where, "supportProfiles", &(cap->supportProfiles));
    PrintArray(where, "supportLevels", &(cap->supportLevels));
    PrintArray(where, "supportPixelFormats", &(cap->supportPixelFormats));
    HDF_LOGI("%{public}s, minInputBufferNum: %{public}d", where, (int32_t)cap->minInputBufferNum);
    HDF_LOGI("%{public}s, minOutputBufferNum: %{public}d", where, (int32_t)cap->minOutputBufferNum);
    HDF_LOGI("%{public}s, allocateMask: %{public}d", where, (int32_t)cap->allocateMask);
    HDF_LOGI("%{public}s, capsMask: %{public}d", where, (int32_t)cap->capsMask);
    HDF_LOGI("%{public}s, ------------------------------ end print cap ---", where);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
