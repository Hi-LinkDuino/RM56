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

#ifndef LOG_QUERIER_H
#define LOG_QUERIER_H

#include <array>
#include <atomic>
#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>

#include <hilog_common.h>
#include <socket.h>

#include "log_buffer.h"

namespace OHOS {
namespace HiviewDFX {
class ServiceController  {
public:
    static constexpr int MAX_DATA_LEN = 2048;
    using PacketBuf = std::array<char, MAX_DATA_LEN>;

    ServiceController(std::unique_ptr<Socket> communicationSocket, HilogBuffer& buffer);
    ~ServiceController();

    void CommunicationLoop(const std::atomic<bool>& stopLoop);

private:
    void SetFilters(const PacketBuf& rawData);

    void HandleLogQueryRequest();
    void HandleNextRequest(const PacketBuf& rawData, const std::atomic<bool>& stopLoop);

    // persist storage
    void HandlePersistStartRequest(const PacketBuf& rawData);
    void HandlePersistStopRequest(const PacketBuf& rawData);
    void HandlePersistQueryRequest(const PacketBuf& rawData);

    // buffer size
    void HandleBufferResizeRequest(const PacketBuf& rawData);
    void HandleBufferSizeRequest(const PacketBuf& rawData);

    // statistics
    void HandleInfoQueryRequest(const PacketBuf& rawData);
    void HandleInfoClearRequest(const PacketBuf& rawData);
    void HandleBufferClearRequest(const PacketBuf& rawData);

    int WriteData(LogQueryResponse& rsp, OptCRef<HilogData> pData);
    int WriteV(const iovec* vec, size_t len);
    int WriteLogQueryRespond(unsigned int sendId, uint32_t respondCmd, OptCRef<HilogData> pData);
    void NotifyForNewData();

    std::unique_ptr<Socket> m_communicationSocket;
    HilogBuffer& m_hilogBuffer;
    HilogBuffer::ReaderId m_bufReader;

    std::condition_variable m_notifyNewDataCv;
    std::mutex m_notifyNewDataMtx;

    LogFilterExt m_filters;
};

int RestorePersistJobs(HilogBuffer& _buffer);
} // namespace HiviewDFX
} // namespace OHOS
#endif
