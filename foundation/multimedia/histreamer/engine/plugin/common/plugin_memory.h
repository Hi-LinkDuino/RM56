/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PLUGIN_MEMORY_H
#define HISTREAMER_PLUGIN_MEMORY_H

#include <cstdint>
#include <memory>

namespace OHOS {
namespace Media {
namespace Plugin {
constexpr size_t INVALID_POSITION = -1;
/**
 * @enum MemoryType
 *
 * @since 1.0
 * @version 1.0
 */
enum struct MemoryType : uint8_t {
    VIRTUAL_ADDR = 0,  ///< Virtual address
    SURFACE_BUFFER,    ///< Surface
    SHARE_MEMORY,   ///< Share Memory fd
};

/**
* @brief Memory allocator, which is provided by the plugin implementer.
*
* @since 1.0
* @version 1.0
*/
struct Allocator {
    explicit Allocator(MemoryType type = MemoryType::VIRTUAL_ADDR) : memoryType(type) {}
    virtual ~Allocator() = default;
    /**
     * @brief Allocates a buffer using the specified size
     * .
     * @param size  Allocation parameters.
     * @return      Pointer of the allocated buffer.
     */
    virtual void* Alloc(size_t size) = 0;

    /**
     * @brief Frees a buffer.
     * Buffer handles returned by Alloc() must be freed with this function when no longer needed.
     *
     * @param ptr   Pointer of the allocated buffer.
     */
    virtual void Free(void* ptr) = 0; // NOLINT: intentionally using void* here

    MemoryType GetMemoryType()
    {
        return memoryType;
    }

private:
    MemoryType memoryType;
};

/**
* @brief Memory description. Only manager the basic memory information.
*
*  here is the memory layout.
*            |            capacity                      |
*            |------------------------------------------|
*            |       buffer size       |
*            |-------------------------|
*  addr   offset                      buffer end
*  +---------+-------------------------+----------------+
*  |*********|        used buffer      |  unused buffer |
*  +---------+-------------------------+----------------+
*
*  operate position:
*                         position
*            +----------------+
*
* @since 1.0
* @version 1.0
*/
class Memory {
public:
    /// Destructor
    virtual ~Memory() = default;

    virtual size_t GetCapacity();

    size_t GetSize();

    const uint8_t* GetReadOnlyData(size_t position = 0);

    uint8_t *GetWritableAddr(size_t estimatedWriteSize, size_t position = 0);

    // If estimatedWriteSize doesn't equal to realWriteSize, should call UpdateDataSize
    void UpdateDataSize(size_t realWriteSize, size_t position = 0);

    size_t Write(const uint8_t* in, size_t writeSize, size_t position = INVALID_POSITION);

    size_t Read(uint8_t* out, size_t readSize, size_t position = INVALID_POSITION);

    void Reset();

    MemoryType GetMemoryType();

protected:
    /**
     * Allocates memory by the specified allocator.
     * Allocation and release are the responsibility of the external allocator.
     *
     * @param capacity Allocated memory size.
     * @param allocator External allocator.
     * @param align The alignment of the memory.
     */
    explicit Memory(size_t capacity, std::shared_ptr<Allocator> allocator = nullptr,
                    size_t align = 1, MemoryType type = MemoryType::VIRTUAL_ADDR, bool allocMem = true);

    /**
     * Get real memory address, it is addr + offset, the offset is calculated according to alignment.
     */
    virtual uint8_t *GetRealAddr() const;

    /// Memory type
    MemoryType memoryType;

    /// Allocated memory size.
    size_t capacity;

    /// The alignment of the memory.
#if (defined(__GNUC__) || defined(__clang__)) && (!defined(WIN32))
    __attribute__((unused))
#endif
    size_t alignment;

    /// Offset of the buffer address to make sure access according to alignment.
    size_t offset {0};

    /// Valid data size
    size_t size;

    /// Externally specified allocator, optional.
    std::shared_ptr<Allocator> allocator;

private:
    /**
     * Create objects based on the external memory, use shared pointers,
     * the allocation and release of memory are managed externally.
     *
     * @param capacity Allocated memory size.
     * @param bufData External memory.
     * @param align The alignment of the memory.
     */
    Memory(size_t capacity, std::shared_ptr<uint8_t> bufData,
           size_t align = 1, MemoryType type = MemoryType::VIRTUAL_ADDR);

    /// Allocated virtual memory address.
    std::shared_ptr<uint8_t> addr;

    friend class Buffer;
};
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif // HISTREAMER_PLUGIN_MEMORY_H
