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

#ifndef MEDIA_DATA_STREAM_IMPL
#define MEDIA_DATA_STREAM_IMPL

#include <queue>
#include <vector>
#include "data_stream.h"
#include "foundation/osal/thread/condition_variable.h"
#include "foundation/osal/thread/mutex.h"

namespace OHOS {
namespace Media {
class DataStreamImpl : public DataStream {
public:
    DataStreamImpl(size_t size, size_t count, MemoryType type);
    bool GetDataBuffer(std::shared_ptr<DataBuffer>& buffer, int timeout) override;
    bool QueueEmptyBuffer(const std::shared_ptr<DataBuffer>& buffer) override;
    bool QueueEmptyBuffer(uint8_t* address) override;

    bool GetEmptyBuffer(std::shared_ptr<DataBuffer>& buffer, int timeout) override;
    bool QueueDataBuffer(const std::shared_ptr<DataBuffer>& buffer) override;

private:
    OSAL::Mutex emptyMutex_ {};
    OSAL::Mutex dataMutex_ {};
    OSAL::ConditionVariable emptyCondition_ {};
    OSAL::ConditionVariable dataCondition_ {};
    std::queue<std::shared_ptr<DataBuffer>> emptyBuffers_ {};
    std::queue<std::shared_ptr<DataBuffer>> dataBuffers_ {};
    std::vector<std::shared_ptr<DataBuffer>> allBuffers_ {}; // keep all buffers
};

class VirtualDataBuffer : public DataBuffer {
public:
    explicit VirtualDataBuffer(size_t capacity);
    ~VirtualDataBuffer() override;
    bool IsEos() override;
    void SetEos(bool isEos) override;
    uint8_t* GetAddress() override;
    size_t GetCapacity() override;
    size_t GetSize() override;
    void SetSize(size_t size) override;

private:
    bool isEos_ {false};
    uint8_t* address_ {nullptr};
    size_t capacity_ {0};
    size_t size_ {0};
};
} // Media
} // OHOS
#endif // MEDIA_DATA_STREAM_IMPL
