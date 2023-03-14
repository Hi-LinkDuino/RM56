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
 * @brief Defines PodVector.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_PLAINDATA_VECTOR_H
#define GRAPHIC_LTE_GEOMETRY_PLAINDATA_VECTOR_H

#include <cstddef>
#include <cstring>

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/graphic_log.h"
#include "securec.h"

namespace OHOS {
/**
 * @file geometry_plain_data_vector.h
 *
 * @brief Defines Podvector, which provides methods of serialization and deserialization.
 *
 * @since 1.0
 * @version 1.0
 */
template <class T>
class GeometryPlaindataVector : public HeapBase {
public:
    GeometryPlaindataVector() : size_(0), capacity_(0), data_(0) {}

    /**
     *
     * @brief Construct a podvector.
     * @param Cap capacity, extratail
     * @since 1.0
     * @version 1.0
     */
    GeometryPlaindataVector(uint32_t cap, uint32_t extraTail = 0);

    GeometryPlaindataVector(const GeometryPlaindataVector<T>&);

    ~GeometryPlaindataVector()
    {
        GeometryArrayAllocator<T>::Deallocate(data_, capacity_);
    }

    const GeometryPlaindataVector<T>& operator=(const GeometryPlaindataVector<T>&);

    /**
     *
     * @brief Set the capacity.
     * @since 1.0
     * @version 1.0
     */
    void Capacity(uint32_t cap, uint32_t extraTail = 0);

    /**
     *
     * @brief Get current capacity.
     * @since 1.0
     * @version 1.0
     */
    uint32_t Capacity() const
    {
        return capacity_;
    }
    /**
     *
     * @brief Request memory.
     * @param Size capacity, extratail expansion capacity.
     * @since 1.0
     * @version 1.0
     */
    void Allocate(uint32_t size, uint32_t extraTail = 0);
    /**
     *
     * @brief Array expansion.
     * @param Specify new capacity.
     * @since 1.0
     * @version 1.0
     */
    void Resize(uint32_t newSize);

    /**
     *
     * @brief Set all data to 0.
     *
     * @since 1.0
     * @version 1.0
     */
    void CleanData()
    {
        if (memset_s(data_, sizeof(T) * size_, 0, sizeof(T) * size_) != EOK) {
            GRAPHIC_LOGE("CleanData faile");
        }
    }

    /**
     *
     * @brief Insert element at specified location.
     *
     * @since 1.0
     * @version 1.0
     */
    void InsertAt(uint32_t pos, const T& val);

    /**
     *
     * @brief Get capacity.
     *
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetSize() const
    {
        return size_;
    }

    /**
     *
     * @brief Get data bytes.
     *
     * @since 1.0
     * @version 1.0
     */
    uint32_t ByteSize() const
    {
        return size_ * sizeof(T);
    }

    /**
     *
     * @brief Serialize the data pointed to by PTR into an array.
     *
     * @since 1.0
     * @version 1.0
     */
    void Serialize(uint8_t* ptr) const;

    /**
     *
     * @brief Deserialize array to data.
     *
     * @since 1.0
     * @version 1.0
     */
    void Deserialize(const uint8_t* data, uint32_t byteSize);
    /**
     *
     * @brief Gets the specified index element.
     *
     * @since 1.0
     * @version 1.0
     */
    const T& operator[](uint32_t index) const
    {
        return data_[index];
    }
    /**
     *
     * @brief Gets the specified index element.
     *
     * @since 1.0
     * @version 1.0
     */
    T& operator[](uint32_t index)
    {
        return data_[index];
    }
    /**
     *
     * @brief Gets the specified index element.
     *
     * @since 1.0
     * @version 1.0
     */
    const T& IndexAt(uint32_t index) const
    {
        return data_[index];
    }
    /**
     *
     * @brief Gets the specified index element.
     *
     * @since 1.0
     * @version 1.0
     */
    T& IndexAt(uint32_t index)
    {
        return data_[index];
    }
    /**
     *
     * @brief Gets the specified index element.
     *
     * @since 1.0
     * @version 1.0
     */
    T ValueAt(uint32_t index) const
    {
        return data_[index];
    }
    /**
     *
     * @brief Get array pointer.
     *
     * @since 1.0
     * @version 1.0
     */
    const T* Data() const
    {
        return data_;
    }

    /**
     *
     * @brief Get array pointer.
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
     * @brief Reset capacity.
     *
     * @since 1.0
     * @version 1.0
     */
    void CutAt(uint32_t num)
    {
        if (num < size_) {
            size_ = num;
        }
    }
    /**
     *
     * @brief Delete all elements.
     *
     * @since 1.0
     * @version 1.0
     */
    void RemoveAll()
    {
        size_ = 0;
    }
    /**
     *
     * @brief Delete all elements.
     *
     * @since 1.0
     * @version 1.0
     */
    void Clear()
    {
        size_ = 0;
    }

private:
    uint32_t size_;
    uint32_t capacity_;
    T* data_;
};

template <class T>
void GeometryPlaindataVector<T>::Capacity(uint32_t cap, uint32_t extraTail)
{
    size_ = 0;
    if (cap > capacity_) {
        GeometryArrayAllocator<T>::Deallocate(data_, capacity_);
        capacity_ = cap + extraTail;
        data_ = capacity_ ? GeometryArrayAllocator<T>::Allocate(capacity_) : 0;
    }
}

template <class T>
void GeometryPlaindataVector<T>::Allocate(uint32_t size, uint32_t extraTail)
{
    Capacity(size, extraTail);
    size_ = size;
}

template <class T>
void GeometryPlaindataVector<T>::Resize(uint32_t newSize)
{
    if (newSize > size_) {
        if (newSize > capacity_) {
            T* data = GeometryArrayAllocator<T>::Allocate(newSize);
            if (memcpy_s(data, newSize, data_, size_ * sizeof(T)) != EOK) {
                GRAPHIC_LOGE("GeometryPlaindataVector Resize fail");
                return;
            }
            GeometryArrayAllocator<T>::Deallocate(data_, capacity_);
            data_ = data;
        }
    } else {
        size_ = newSize;
    }
}

template <class T>
GeometryPlaindataVector<T>::GeometryPlaindataVector(uint32_t cap, uint32_t extraTail)
    : size_(0), capacity_(cap + extraTail),
      data_(GeometryArrayAllocator<T>::Allocate(capacity_)) {}

template <class T>
GeometryPlaindataVector<T>::GeometryPlaindataVector(const GeometryPlaindataVector<T>& v)
    : size_(v.size_), capacity_(v.capacity_),
      data_(v.capacity_ ? GeometryArrayAllocator<T>::Allocate(v.capacity_) : 0)
{
    if (memcpy_s(data_, sizeof(T) * v.size_, v.data_, sizeof(T) * v.size_) != EOK) {
        GRAPHIC_LOGE("GeometryPlaindataVector fail");
    }
}

template <class T>
const GeometryPlaindataVector<T>& GeometryPlaindataVector<T>::operator=
(const GeometryPlaindataVector<T>& val)
{
    Allocate(val.size_);
    if (val.size_) {
        if (memcpy_s(data_, sizeof(T) * val.size_, val.data_, sizeof(T) * val.size_) != EOK) {
            GRAPHIC_LOGE("GeometryPlaindataVector::operator=fail");
        }
    }
    return *this;
}

template <class T>
void GeometryPlaindataVector<T>::InsertAt(uint32_t pos, const T& val)
{
    if (pos >= size_) {
        data_[size_] = val;
    } else {
        if (memmove_s(data_ + pos + 1, (size_ - pos) * sizeof(T),
                      data_ + pos, (size_ - pos) * sizeof(T)) != EOK) {
            return;
        }
        data_[pos] = val;
    }
    ++size_;
}
} // namespace OHOS
#endif
