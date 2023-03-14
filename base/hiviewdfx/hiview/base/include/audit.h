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
#ifndef HIVIEW_BASE_AUDIT_H
#define HIVIEW_BASE_AUDIT_H
#include <atomic>
#include <fstream>
#include <list>
#include <mutex>
#include <string>

#include "public_defines.h"
#include "singleton.h"

#include "unique_fd.h"
namespace OHOS {
namespace HiviewDFX {
class Audit : public Singleton<Audit> {
public:
    Audit() : init_(false), useBak_(false), enabled_(false) {};
    ~Audit();
    enum StatsEvent {
        QUEUE_EVENT_IN = 0,
        QUEUE_EVENT_OUT,
        PIPELINE_EVENT_CREATE,
        PIPELINE_EVENT_HANDLE_IN,
        PIPELINE_EVENT_HANDLE_OUT,
        PIPELINE_EVENT_DONE,
        PRIVATE_AUDIT_EVENT_TYPE
    };

    // the format of the normal event digestion should be 1.sender 2.processor 3.thread 4.detailed event info
    static bool WriteAuditEvent(StatsEvent eventType, uint64_t eventId, const std::string& digest = "");

    // get a copy of the audit log
    // if loadFromFile is enabled, log store in the disk will be inserted into the front of current logs
    static bool GetAuditLog(bool loadFromFile, std::list<std::string>& ret);

    static bool IsEnabled();

    // clear all logs
    void Clear();

    // judge the version and do some initialization
    void Init(bool isBeta);

    static constexpr char DOMAIN_DELIMITER = '|';
private:
    bool IsActiveLogFileSizeReachTheashold();
    bool ReadLogFromFile(const std::string& path, std::list<std::string>& ret);
    bool IsBackupFileActive();
    const std::string GetLogFilePath(bool active);
    void GetAuditLogInner(bool loadFromFile, std::list<std::string>& ret);
    void SaveToFileLocked(const std::string& content);
    void SwitchActiveFile();
    void WriteEvent(const std::string& content);
    bool init_;
    bool useBak_;
    bool enabled_;
    UniqueFd writeFd_;
    std::mutex mutex_;
    std::once_flag initFlag_;
    std::atomic<uint32_t> writeLogCount_;
    static constexpr uint32_t MAX_MEMORY_LOG_COUNT = 1024; // 1024 lines
    static constexpr uint32_t MAX_DISK_LOG_SIZE = 1024 * 512; // 512KB
    static constexpr uint32_t MAX_AUDIT_LOG_SIZE = 1024; // 1024 byte
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIVIEW_BASE_AUDIT_H