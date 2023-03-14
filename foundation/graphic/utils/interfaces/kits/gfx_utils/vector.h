/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
 * @addtogroup Graphic
 * @{
 *
 * @brief Defines a lightweight graphics system that provides basic UI and container views,
 *        including buttons, images, labels, lists, animators, scroll views, swipe views, and layouts.
 *        This system also provides the Design for X (DFX) capability to implement features such as
 *        view rendering, animation, and input event distribution.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LITE_VECTOR_H
#define GRAPHIC_LITE_VECTOR_H
#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/heap_base.h"

namespace OHOS {
namespace Graphic {
template<typename T> class Vector : public HeapBase {
public:
    Vector(uint16_t capacity = 1) : capacity_(capacity)
    {
        array_ = new T[capacity];
    }

    Vector(const Vector<T>& value)
    {
        capacity_ = value.Capacity();
        array_ = new T[capacity_];
        size_ = value.size_;
        if (value.array_ != nullptr) {
            for (uint16_t i = 0; i < value.size_; i++) {
                array_[i] = value.array_[i];
            }
        }
    }

    virtual ~Vector()
    {
        delete[] array_;
    }

    T& Front()
    {
        return array_[0]; // undefined operation while vector is empty
    }

    T& Back()
    {
        return array_[size_ - 1]; // undefined operation while vector is empty
    }

    void PushBack(const T& data)
    {
        if (size_ == capacity_) {
            capacity_ <<= 1;
            T* array = new T[capacity_];
            for (uint16_t i = 0; i < size_; i++) {
                array[i] = array_[i];
            }
            delete[] array_;
            array_ = array;
        }
        array_[size_++] = data;
    }

    void PopBack()
    {
        if (IsEmpty()) {
            return;
        }
        --size_;
    }

    void Clear()
    {
        size_ = 0;
    }

    T* Begin() const
    {
        return array_;
    }

    const T* End() const
    {
        return (array_ + size_);
    }

    bool IsEmpty() const
    {
        return (size_ == 0);
    }

    uint16_t Size() const
    {
        return size_;
    }

    uint16_t Capacity() const
    {
        return capacity_;
    }

    uint16_t ReSize(uint16_t size)
    {
        if (size <= capacity_) {
            size_ = size;
        }
        return size_;
    }

    void Erase(uint16_t index)
    {
        if (index >= size_) {
            return;
        }
        size_--;
        for (; index < size_; index++) {
            array_[index] = array_[index + 1];
        }
    }

    void Swap(Vector<T>& other)
    {
        uint16_t size = size_;
        size_ = other.size_;
        other.size_ = size;

        uint16_t capacity = capacity_;
        capacity_ = other.capacity_;
        other.capacity_ = capacity;

        T* array = array_;
        array_ = other.array_;
        other.array_ = array;
    }

    T& operator[](uint16_t index)
    {
        return array_[index];
    }

    void operator=(const Vector<T>& value)
    {
        if (capacity_ < value.Size()) {
            delete[] array_;
            capacity_ = value.capacity_;
            array_ = new T[capacity_];
        }
        if (value.array_ != nullptr) {
            for (uint16_t i = 0; i < value.size_; i++) {
                array_[i] = value.array_[i];
            }
            size_ = value.size_;
        }
    }

protected:
    uint16_t size_ = 0;
    uint16_t capacity_ = 0;
    T* array_ = nullptr;
    uint16_t head = 0;
    uint16_t tail = 0;
};
} // namespace Graphic
} // namespace OHOS
#endif // GRAPHIC_LITE_VECTOR_H
