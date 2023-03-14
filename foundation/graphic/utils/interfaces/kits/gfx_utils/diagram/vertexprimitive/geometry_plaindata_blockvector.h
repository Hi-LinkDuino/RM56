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
 * @brief Defines PodBvector.
 * @since 1.0
 * @version 1.0
 */

#ifndef GRAPHIC_LTE_GEOMETRY_PLAINDATA_BLOCKVECTOR_H
#define GRAPHIC_LTE_GEOMETRY_PLAINDATA_BLOCKVECTOR_H

#include <cstddef>
#include <cstring>

#include "gfx_utils/diagram/common/common_basics.h"
#include "gfx_utils/heap_base.h"
#include "securec.h"

namespace OHOS {
/**
 * @brief Defines PodBvector.
 *
 * @since 1.0
 * @version 1.0
 */
template <class T, uint32_t S = BLOCK_SHIFT_SIZE>
class GeometryPlainDataBlockVector : public HeapBase {
public:
    enum BlockScale {
        BLOCK_SHIFT = S,
        BLOCK_SIZE = 1 << BLOCK_SHIFT,
        BLOCK_MASK = BLOCK_SIZE - 1
    };
    using ValueType = T;
    GeometryPlainDataBlockVector();
    /**
     * @brief Construct a podvector.
     * @param blockPtrInc Size of each block
     * @since 1.0
     * @version 1.0
     */
    GeometryPlainDataBlockVector(uint32_t blockPtrInc);
    ~GeometryPlainDataBlockVector();
    GeometryPlainDataBlockVector(const GeometryPlainDataBlockVector<T, S>& v);
    const GeometryPlainDataBlockVector<T, S>& operator=(const GeometryPlainDataBlockVector<T, S>& v);
    /**
     * @brief clean.
     * @since 1.0
     * @version 1.0
     */
    void RemoveAll()
    {
        size_ = 0;
    }
    /**
     * @brief clean.
     * @since 1.0
     * @version 1.0
     */
    void Clear()
    {
        size_ = 0;
    }
    /**
     * @brief Freeing memory blocks occupied by all elements.
     * @since 1.0
     * @version 1.0
     */
    void FreeAll()
    {
        FreeTail(0);
    }
    /**
     * @brief Modify last element.
     * @since 1.0
     * @version 1.0
     */
    void ModifyLast(const T& val);
    /**
     * @brief Delete last element.
     * @since 1.0
     * @version 1.0
     */
    void RemoveLast();

    void AllocateBlock(uint32_t blockNum);

    /**
     * @brief Release element from end.
     * @param size Number of to release
     * @since 1.0
     * @version 1.0
     */
    void FreeTail(uint32_t size);
    /**
     * @brief Add an element.
     * @param val Added elements.
     * @since 1.0
     * @version 1.0
     */
    void Add(const T& val);
    /**
     * @brief Add an element.
     * @param val val Added elements.
     * @since 1.0
     * @version 1.0
     */
    void PushBack(const T& val)
    {
        Add(val);
    }

    /**
     * @brief Add an array.
     * @param ptr The first address of the array to be added,numElem Number of elements
     * @since 1.0
     * @version 1.0
     */
    void AddArray(const T* ptr, uint32_t numElem)
    {
        while (numElem--) {
            Add(*ptr++);
        }
    }

    /**
     * @brief Reduce array capacity.
     * @since 1.0
     * @version 1.0
     */
    void CutAt(uint32_t size)
    {
        if (size < size_) {
            size_ = size;
        }
    }
    /**
     * @brief Add data.
     * @since 1.0
     * @version 1.0
     */
    template <class DataAccessor>
    void AddData(DataAccessor& data)
    {
        while (data.size()) {
            Add(*data);
            ++data;
        }
    }
    /**
     * @brief Get array capacity.
     * @since 1.0
     * @version 1.0
     */
    uint32_t GetSize() const
    {
        return size_;
    }
    /**
     * @brief Gets the specified index data block.
     * @since 1.0
     * @version 1.0
     */
    const T& operator[](uint32_t index) const
    {
        return blocks_[index >> BLOCK_SHIFT][index & BLOCK_MASK];
    }
    /**
     * @brief Gets the specified index data block.
     * @since 1.0
     * @version 1.0
     */
    T& operator[](uint32_t index)
    {
        return blocks_[index >> BLOCK_SHIFT][index & BLOCK_MASK];
    }
    /**
     * @brief Gets the specified index data block.
     * @since 1.0
     * @version 1.0
     */
    T& IndexAt(uint32_t index)
    {
        return blocks_[index >> BLOCK_SHIFT][index & BLOCK_MASK];
    }
    /**
     * @brief Gets the specified index data block.
     * @since 1.0
     * @version 1.0
     */
    T ValueAt(uint32_t index) const
    {
        return blocks_[index >> BLOCK_SHIFT][index & BLOCK_MASK];
    }
    /**
     * @brief Gets the current index data block.
     * @since 1.0
     * @version 1.0
     */
    const T& IndexAt(uint32_t index) const
    {
        return blocks_[index >> BLOCK_SHIFT][index & BLOCK_MASK];
    }
    /**
     * @brief Gets the current index data block.
     * @since 1.0
     * @version 1.0
     */
    T& Curr(uint32_t index)
    {
        return (*this)[index];
    }
    /**
     * @brief Gets the previous data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    const T& Prev(uint32_t index) const
    {
        return (*this)[(index + size_ - 1) % size_];
    }
    /**
     * @brief Gets the previous data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    T& Prev(uint32_t index)
    {
        return (*this)[(index + size_ - 1) % size_];
    }
    /**
     * @brief Gets the data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    const T& Curr(uint32_t index) const
    {
        return (*this)[index];
    }

    /**
     * @brief Gets the next data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    const T& Next(uint32_t index) const
    {
        return (*this)[(index + 1) % size_];
    }

    /**
     * @brief Gets the next data block on the current index.
     * @since 1.0
     * @version 1.0
     */
    T& Next(uint32_t index)
    {
        return (*this)[(index + 1) % size_];
    }
    /**
     * @brief Get end data.
     * @since 1.0
     * @version 1.0
     */
    const T& Last() const
    {
        return (*this)[size_ - 1];
    }

    /**
     * @brief Get end data.
     * @since 1.0
     * @version 1.0
     */
    T& Last()
    {
        return (*this)[size_ - 1];
    }

    /**
     * @brief Gets the specified block address.
     *
     * @since 1.0
     * @version 1.0
     */
    const T* Block(uint32_t nb) const
    {
        return blocks_[nb];
    }

private:
    /**
     * @brief Request a memory block of the specified size.
     *
     * @since 1.0
     * @version 1.0
     */
    void Allocatelock(uint32_t nb);
    T* DataPtr();

    uint32_t size_;        // Number of elements
    uint32_t numBlocks_;   // Number of blocks
    uint32_t maxBlocks_;   // Maximum number of blocks
    T** blocks_;           // First address of all blocks
    uint32_t blockPtrInc_; // Size of each block
};

template <class T, uint32_t S>
GeometryPlainDataBlockVector<T, S>::~GeometryPlainDataBlockVector()
{
    if (numBlocks_) {
        T** blk = blocks_ + numBlocks_ - 1;
        while (numBlocks_--) {
            GeometryArrayAllocator<T>::Deallocate(*blk, BLOCK_SIZE);
            --blk;
        }
    }
    GeometryArrayAllocator<T*>::Deallocate(blocks_, maxBlocks_);
}

template <class T, uint32_t S>
void GeometryPlainDataBlockVector<T, S>::FreeTail(uint32_t size)
{
    if (size < size_) {
        uint32_t nb = (size + BLOCK_MASK) >> BLOCK_SHIFT;
        while (numBlocks_ > nb) {
            GeometryArrayAllocator<T>::Deallocate(blocks_[--numBlocks_], BLOCK_SIZE);
        }
        if (numBlocks_ == 0) {
            GeometryArrayAllocator<T*>::Deallocate(blocks_, maxBlocks_);
            blocks_ = 0;
            maxBlocks_ = 0;
        }
        size_ = size;
    }
}

template <class T, uint32_t S>
GeometryPlainDataBlockVector<T, S>::GeometryPlainDataBlockVector()
    : size_(0), numBlocks_(0), maxBlocks_(0), blocks_(0), blockPtrInc_(BLOCK_SIZE) {}

template <class T, uint32_t S>
GeometryPlainDataBlockVector<T, S>::GeometryPlainDataBlockVector(uint32_t blockPtrInc)
    : size_(0), numBlocks_(0), maxBlocks_(0), blocks_(0), blockPtrInc_(blockPtrInc) {}

template <class T, uint32_t S>
GeometryPlainDataBlockVector<T, S>::GeometryPlainDataBlockVector(
    const GeometryPlainDataBlockVector<T, S>& v)
    : size_(v.size_),
      numBlocks_(v.numBlocks_),
      maxBlocks_(v.maxBlocks_),
      blocks_(v.maxBlocks_ ? GeometryArrayAllocator<T*>::Allocate(v.maxBlocks_) : 0),
      blockPtrInc_(v.blockPtrInc_)
{
    for (uint32_t i = 0; i < v.numBlocks_; ++i) {
        blocks_[i] = GeometryArrayAllocator<T>::Allocate(BLOCK_SIZE);
        if (memcpy_s(blocks_[i], BLOCK_SIZE * sizeof(T), v.blocks_[i], BLOCK_SIZE * sizeof(T)) != EOK) {
            free(blocks_[i]);
            continue;
        }
    }
}

template <class T, uint32_t S>
const GeometryPlainDataBlockVector<T, S>& GeometryPlainDataBlockVector<T, S>::operator=
(const GeometryPlainDataBlockVector<T, S>& v)
{
    uint32_t i;
    for (i = numBlocks_; i < v.numBlocks_; ++i) {
        AllocateBlock(i);
    }
    for (i = 0; i < v.numBlocks_; ++i) {
        if (memcpy_s(blocks_[i], BLOCK_SIZE * sizeof(T), v.blocks_[i], BLOCK_SIZE * sizeof(T)) != EOK) {
            free(blocks_[i]);
            continue;
        }
    }
    size_ = v.size_;
    return *this;
}

template <class T, uint32_t S>
void GeometryPlainDataBlockVector<T, S>::AllocateBlock(uint32_t blockNum)
{
    if (blockNum >= maxBlocks_) {
        T** newBlocks = GeometryArrayAllocator<T*>::Allocate(maxBlocks_ + blockPtrInc_);

        if (blocks_) {
            if (memcpy_s(newBlocks, (maxBlocks_ + blockPtrInc_), blocks_, numBlocks_ * sizeof(T*)) != EOK) {
                free(newBlocks);
                return;
            }
            GeometryArrayAllocator<T*>::Deallocate(blocks_, maxBlocks_);
        }
        blocks_ = newBlocks;
        maxBlocks_ += blockPtrInc_;
    }
    blocks_[blockNum] = GeometryArrayAllocator<T>::Allocate(BLOCK_SIZE);
    numBlocks_++;
}

template <class T, uint32_t S>
inline T* GeometryPlainDataBlockVector<T, S>::DataPtr()
{
    uint32_t blockNum = size_ >> BLOCK_SHIFT;
    if (blockNum >= numBlocks_) {
        AllocateBlock(blockNum);
    }
    return blocks_[blockNum] + (size_ & BLOCK_MASK);
}

template <class T, uint32_t S>
inline void GeometryPlainDataBlockVector<T, S>::Add(const T& val)
{
    *DataPtr() = val;
    ++size_;
}

template <class T, uint32_t S>
inline void GeometryPlainDataBlockVector<T, S>::RemoveLast()
{
    if (size_) {
        --size_;
    }
}

template <class T, uint32_t S>
void GeometryPlainDataBlockVector<T, S>::ModifyLast(const T& val)
{
    RemoveLast();
    Add(val);
}
} // namespace OHOS
#endif
