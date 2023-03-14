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

#ifndef HDI_IDISPLAY_GRALLOC_V1_0_H
#define HDI_IDISPLAY_GRALLOC_V1_0_H

#include "display_type.h"
#include "buffer_handle.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class IDisplayGralloc {
public:
    virtual ~IDisplayGralloc() = default;

    /**
     * @brief Get all interfaces of display gralloc.
     *
     * @return Returns <b>IDisplayGralloc* </b> if the operation is successful; returns an Null point otherwise.
     * @since 1.0
     * @version 1.0
     */
    // static sptr<IDisplayGralloc> Get();
    static IDisplayGralloc* Get();

    /**
     * @brief Allocates memory based on the parameters passed by the GUI.
     *
     * @param info Indicates the reference to the description info of the memory to allocate.
     *
     * @param handle Indicates the reference of pointer to the buffer of the memory to allocate.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t AllocMem(const AllocInfo &info, BufferHandle *&handle) const = 0;

    /**
     * @brief Releases memory.
     *
     * @param handle Indicates the reference to the buffer of the memory to release.
     *
     * @since 1.0
     * @version 1.0
     */
    virtual void FreeMem(const BufferHandle &handle) const = 0;

    /**
     * @brief Maps memory to memory without cache in the process's address space.
     *
     * @param handle Indicates the reference to the buffer of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual void *Mmap(const BufferHandle &handle) const = 0;

    /**
     * @brief Maps memory to memory with cache in the process's address space.
     *
     * @param handle Indicates the reference to the buffer of the memory to map.
     *
     * @return Returns the pointer to a valid address if the operation is successful; returns <b>NULL</b> otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual void *MmapCache(const BufferHandle &buffer) const = 0;

    /**
     * @brief Unmaps memory, that is, removes any mappings in the process's address space.
     *
     * @param handle Indicates the reference to the buffer of the memory to unmap.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t Unmap(const BufferHandle &handle) const = 0;

    /**
     * @brief Flushes data from the cache to memory and invalidates the data in the cache.
     *
     * @param handle Indicates the reference to the buffer of the cache to flush.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t FlushCache(const BufferHandle &handle) const = 0;

    /**
     * @brief Flushes data from the cache mapped via {@link Mmap} to memory and invalidates the data in the cache.
     *
     * @param handle Indicates the reference to the buffer of the cache to flush.
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t FlushMCache(const BufferHandle &buffer) const = 0;

    /**
     * @brief Invalidate the Cache, it will update the cache from memory.
     *
     * @param handle Indicates the reference to the buffer of the cache which will been invalidated
     *
     * @return Returns <b>0</b> if the operation is successful; returns an error code defined in {@link DispErrCode}
     * otherwise.
     * @since 1.0
     * @version 1.0
     */
    virtual int32_t InvalidateCache(const BufferHandle &handle) const = 0;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_IDISPLAY_GRALLOC_V1_0_H
