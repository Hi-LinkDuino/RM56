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

#include "mapper_adapter.h"
#include "hdf_base.h"
#include "hdf_log.h"

#define HDF_LOG_TAG HDI_DISP_MAPPER

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
MapperAdapter::MapperAdapter()
{
    if (GrallocInitialize(&mapperFuncs_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: mapperFuncs_ init failed", __func__);
    }
}

MapperAdapter::~MapperAdapter()
{
    if (GrallocUninitialize(mapperFuncs_) != HDF_SUCCESS) {
        HDF_LOGE("%{public}s: mapperFuncs_ uninit failed", __func__);
    }
}

bool MapperAdapter::IsReady()
{
    HDF_LOGI("%{public}s: entry", __func__);
    return mapperFuncs_ != nullptr;
}

int32_t MapperAdapter::MapBuffer(const BufferHandle& handle, void*& outData) const
{
    int32_t ret = 0;
    outData = mapperFuncs_->Mmap(const_cast<BufferHandle *>(&handle));
    return ret;
}

int32_t MapperAdapter::UnmapBuffer(const BufferHandle& handle) const
{
    int32_t ret = 0;
    ret = mapperFuncs_->Unmap(const_cast<BufferHandle *>(&handle));
    return ret;
}

int32_t MapperAdapter::InvalidateCache(const BufferHandle& handle) const
{
    int32_t ret = 0;
    ret = mapperFuncs_->InvalidateCache(const_cast<BufferHandle *>(&handle));
    return ret;
}

int32_t MapperAdapter::FlushCache(const BufferHandle& handle) const
{
    int32_t ret = 0;
    ret = mapperFuncs_->FlushCache(const_cast<BufferHandle *>(&handle));
    return ret;
}

void MapperAdapter::FreeBuffer(const BufferHandle& handle) const
{
    mapperFuncs_->FreeMem(const_cast<BufferHandle *>(&handle));
}
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS