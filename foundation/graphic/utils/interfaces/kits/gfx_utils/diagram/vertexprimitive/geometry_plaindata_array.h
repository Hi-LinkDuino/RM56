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

/**
 * @addtogroup GraphicGeometry
 * @{
 *
 * @brief Defines Arc.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef GRAPHIC_LTE_GEOMETRY_PLAINDATA_ARRAY_H
#define GRAPHIC_LTE_GEOMETRY_PLAINDATA_ARRAY_H

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/heap_base.h"
#include "securec.h"
namespace OHOS {
/**
 * @file geometry_plaindata_array.h
 *
 * @brief Defines GeometryPlainDataArray,Variable capacity.
 *
 * @since 1.0
 * @version 1.0
 */
template <class T>
class GeometryPlainDataArray : public HeapBase {
public:
    using SelfType = GeometryPlainDataArray<T>;

    ~GeometryPlainDataArray()
    {
        GeometryArrayAllocator<T>::Deallocate(data_, size_);
    }

    GeometryPlainDataArray() : data_(0), size_(0) {}
    /**
     *
     * @brief Construct definitions podarray array.
     * @param size Initial capacity.
     * @since 1.0
     * @version 1.0
     */
    GeometryPlainDataArray(uint32_t size)
        : data_(GeometryArrayAllocator<T>::Allocate(size)), size_(size) {}

    GeometryPlainDataArray(const SelfType& podArray)
        : data_(GeometryArrayAllocator<T>::Allocate(podArray.size_)), size_(podArray.size_)
    {
        if (memcpy_s(data_, sizeof(T) * size_, podArray.data_, sizeof(T) * size_) != EOK) {
            GRAPHIC_LOGE("GeometryPlainDataArray fail");
        }
    }

    const SelfType& operator=(const SelfType& podArray)
    {
        Resize(podArray.GetSize());
        if (memcpy_s(data_, sizeof(T) * size_, podArray.data_, sizeof(T) * size_) != EOK) {
            GRAPHIC_LOGE("GeometryPlainDataArray fail");
        }
        return *this;
    }
    /**
     * @brief Gets the element of the specified index.
     *
     * @since 1.0
     * @version 1.0
     */
    const T& operator[](uint32_t index) const
    {
        return data_[index];
    }
    /**
     * @brief Gets the element of the specified index.
     *
     * @since 1.0
     * @version 1.0
     */
    T& operator[](uint32_t index)
    {
        return data_[index];
    }
    /**
     * @brief Gets the element of the specified index.
     *
     * @since 1.0
     * @version 1.0
     */
    T ValueAt(uint32_t index) const
    {
        return data_[index];
    }
    /**
     * @brief Gets the element of the specified index.
     *
     * @since 1.0
     * @version 1.0
     */
    const T& IndexAt(uint32_t index) const
    {
        return data_[index];
    }
    /**
     * @brief Gets the element of the specified index.
     *
     * @since 1.0
     * @version 1.0
     */
    T& IndexAt(uint32_t index)
    {
        return data_[index];
    }
    /**
     * @brief Get element header address.
     *
     * @since 1.0
     * @version 1.0
     */
    const T* Data() const
    {
        return data_;
    }
    /**
     * @brief Get element header address.
     *
     * @since 1.0
     * @version 1.0
     */
    T* Data()
    {
        return data_;
    }

    /**
     *
     * @brief Modify the capacity of the definitions podarray array.
     * @param size capacity
     * @since 1.0
     * @version 1.0
     */
    void Resize(uint32_t size)
    {
        if (size != size_) {
            GeometryArrayAllocator<T>::Deallocate(data_, size_);
            data_ = GeometryArrayAllocator<T>::Allocate(size_ = size);
        }
    }
    /**
     * @brief Get the number of elements.
     *
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetSize() const
    {
        return size_;
    }

private:
    T* data_;
    uint32_t size_;
};
} // namespace OHOS
#endif
