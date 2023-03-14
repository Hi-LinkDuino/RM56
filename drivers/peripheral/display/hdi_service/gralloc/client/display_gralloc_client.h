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

#ifndef HDI_DISPLAY_GRALLOC_CLIENT_V1_0_H
#define HDI_DISPLAY_GRALLOC_CLIENT_V1_0_H

#include "iservmgr_hdi.h"
#include "buffer_handle.h"
#include "display_type.h"
#include "idisplay_gralloc.h"
#include "allocator_proxy.h"
#include "mapper_adapter.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class DisplayGrallocClient : public IDisplayGralloc {
public:
    DisplayGrallocClient();
    virtual ~DisplayGrallocClient() {}
    virtual int32_t AllocMem(const AllocInfo& info, BufferHandle*& handle) const override;
    virtual void FreeMem(const BufferHandle& handle) const override;
    virtual void *Mmap(const BufferHandle& handle) const override;
    virtual void *MmapCache(const BufferHandle &buffer) const override;
    virtual int32_t Unmap(const BufferHandle& handle) const override;
    virtual int32_t FlushCache(const BufferHandle &buffer) const override;
    virtual int32_t FlushMCache(const BufferHandle &buffer) const override;
    virtual int32_t InvalidateCache(const BufferHandle& handle) const override;

private:
    std::shared_ptr<MapperAdapter> mapperAdapter_;
    sptr<IDisplayAllocator> allocatorProxy_;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_DISPLAY_GRALLOC_CLIENT_V1_0_H
