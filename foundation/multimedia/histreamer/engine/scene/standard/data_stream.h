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

#ifndef MEDIA_DATA_STREAM
#define MEDIA_DATA_STREAM

#include <memory>

namespace OHOS {
namespace Media {
/**
 * @enum MemoryType
 *
 * @since 1.0
 * @version 1.0
 */
enum class MemoryType {
    VIRTUAL_ADDR = 0,  ///< Virtual address
    SURFACE_BUFFER,    ///< Surface
    SHARE_MEMORY,      ///< Share Memory fd
};

/**
 * @brief Data buffer, contains the data.
 *
 * @since 1.0
 * @version 1.0
 */
class DataBuffer {
public:
    virtual ~DataBuffer() = default;

    /**
     * @brief Get the EOS status.
     *
     * @return Returns the EOS status, true if this is the end of steam.
     * @since 1.0
     * @version 1.0
     */
    virtual bool IsEos() = 0;

    /**
    * @brief Set the EOS status.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual void SetEos(bool isEos) = 0;

    /**
    * @brief Get the buffer address.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual uint8_t* GetAddress() = 0;

    /**
    * @brief Get the buffer capacity.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual size_t GetCapacity() = 0;

    /**
    * @brief Get the size of the valid data in this data buffer.
    *
    * @since 1.0
    * @version 1.0
    */
    virtual size_t GetSize() = 0;

    /**
    * @brief Set the size of the valid data in this data buffer.
    *
    * @param size Indicates the size of the valid data.
    * @since 1.0
    * @version 1.0
    */
    virtual void SetSize(size_t size) = 0;
};

/**
 * @brief Data producer uses this interface to produce data.
 *
 * @since 1.0
 * @version 1.0
 */
class DataProducer {
public:
    virtual ~DataProducer() = default;

    /**
    * @brief Get empty buffer.
    *
    * @param buffer Out parameter to obtain the buffer.
    * @param timeout Indicates how much time (millisecond) to wait, default -1 means wait until buffer obtained.
    * @since 1.0
    * @version 1.0
    */
    virtual bool GetEmptyBuffer(std::shared_ptr<DataBuffer>& buffer, int timeout = -1) = 0;

    /**
    * @brief Queue data buffer.
    *
    * @param buffer the buffer contains data.
    * @since 1.0
    * @version 1.0
    */
    virtual bool QueueDataBuffer(const std::shared_ptr<DataBuffer>& buffer) = 0;
};

/**
 * @brief Data consumer uses this interface to consume data.
 *
 * @since 1.0
 * @version 1.0
 */
class DataConsumer {
public:
    virtual ~DataConsumer() = default;

    /**
    * @brief Get data buffer.
    *
    * @param buffer Out parameter to obtain the buffer contains data.
    * @param timeout Indicates how much time (millisecond) to wait, default -1 means wait until data buffer obtained.
    * @since 1.0
    * @version 1.0
    */
    virtual bool GetDataBuffer(std::shared_ptr<DataBuffer>& buffer, int timeout = -1) = 0;

    /**
    * @brief Use the shared_ptr of buffer to queue empty buffer to data stream.
    *
    * @param buffer Indicates the shared_ptr of the empty buffer.
    * @since 1.0
    * @version 1.0
    */
    virtual bool QueueEmptyBuffer(const std::shared_ptr<DataBuffer>& buffer) = 0;

    /**
    * @brief Use the buffer address to queue empty buffer to data stream.
    *
    * @param address Indicates the address of the empty buffer.
    * @since 1.0
    * @version 1.0
    */
    virtual bool QueueEmptyBuffer(uint8_t* address) = 0;
};

/**
 * @brief Data stream, extends DataConsumer and DataProducer.
 *
 * @since 1.0
 * @version 1.0
 */
class DataStream : public DataConsumer, public DataProducer {
};

/**
 * @brief The factory function to create {@link DataStream}.
 *
 * @param size Indicates the size of each buffer.
 * @param count Indicates the count of buffers.
 * @param type Indicates the memory type, default is virtual address.
 * @since 1.0
 * @version 1.0
 */
std::shared_ptr<DataStream> CreateDataStream(size_t size, size_t count, MemoryType type = MemoryType::VIRTUAL_ADDR);
} // Media
} // OHOS
#endif // MEDIA_DATA_STREAM
