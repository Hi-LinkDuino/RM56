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

#ifndef HDI_MAPPER_ADAPTER_V1_0_H
#define HDI_MAPPER_ADAPTER_V1_0_H

#include "buffer_handle.h"
#include "display_gralloc.h"
#include "display_type.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class MapperAdapter {
public:
    MapperAdapter();
    virtual ~MapperAdapter();
    bool IsReady();
    int32_t MapBuffer(const BufferHandle& handle, void *&outData) const;
    int32_t UnmapBuffer(const BufferHandle& handle) const;
    int32_t InvalidateCache(const BufferHandle& handle) const;
    int32_t FlushCache(const BufferHandle& handle) const;
    void FreeBuffer(const BufferHandle& handle) const;

private:
    GrallocFuncs *mapperFuncs_ = nullptr;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_MAPPER_ADAPTER_V1_0_H
