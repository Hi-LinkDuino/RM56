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

#ifndef LOG_BUFFER_H
#define LOG_BUFFER_H

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <shared_mutex>

#include "log_data.h"
#include "log_filter.h"

namespace OHOS {
namespace HiviewDFX {
class HilogBuffer {
public:
    using LogMsgContainer = std::list<HilogData>;
    using ReaderId = uintptr_t;
    using OnFound = std::function<void(const HilogData&)>;

    HilogBuffer();
    ~HilogBuffer();

    size_t Insert(const HilogMsg& msg);
    bool Query(const LogFilterExt& filter, const ReaderId& id, OnFound onFound);

    ReaderId CreateBufReader(std::function<void()> onNewDataCallback);
    void RemoveBufReader(const ReaderId& id);

    int32_t Delete(uint16_t logType);

    void InitBuffLen();
    int64_t GetBuffLen(uint16_t logType);
    int32_t SetBuffLen(uint16_t logType, uint64_t buffSize);
    int32_t GetStatisticInfoByLog(uint16_t logType, uint64_t& printLen, uint64_t& cacheLen, int32_t& dropped);
    int32_t GetStatisticInfoByDomain(uint32_t domain, uint64_t& printLen, uint64_t& cacheLen, int32_t& dropped);
    int32_t ClearStatisticInfoByLog(uint16_t logType);
    int32_t ClearStatisticInfoByDomain(uint32_t domain);

    static bool LogMatchFilter(const LogFilterExt& filter, const HilogData& logData);

private:
    struct BufferReader {
        LogMsgContainer::iterator m_pos;
        LogMsgContainer* m_msgList = nullptr;
        std::function<void()> m_onNewDataCallback;
    };

    void UpdateStatistics(const HilogData& logData);
    void OnDeleteItem(LogMsgContainer::iterator itemPos);
    void OnPushBackedItem(LogMsgContainer& msgList);
    void OnNewItem(LogMsgContainer& msgList);
    std::shared_ptr<BufferReader> GetReader(const ReaderId& id);

    size_t size;
    size_t sizeByType[LOG_TYPE_MAX];
    LogMsgContainer hilogDataList;
    LogMsgContainer hilogKlogList;
    std::shared_mutex hilogBufferMutex;
    std::map<uint32_t, uint64_t> cacheLenByDomain;
    std::map<uint32_t, uint64_t> printLenByDomain;
    std::map<uint32_t, uint64_t> droppedByDomain;
    uint64_t cacheLenByType[LOG_TYPE_MAX];
    uint64_t droppedByType[LOG_TYPE_MAX];
    uint64_t printLenByType[LOG_TYPE_MAX];

    std::map<ReaderId, std::shared_ptr<BufferReader>> m_logReaders;
    std::shared_mutex m_logReaderMtx;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif
