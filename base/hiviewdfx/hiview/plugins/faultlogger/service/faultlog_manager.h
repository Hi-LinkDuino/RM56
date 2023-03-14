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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULTLOG_MANAGER_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULTLOG_MANAGER_H
#include <cstdint>
#include <ctime>
#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "event_loop.h"
#include "log_store_ex.h"

#include "faultlog_info.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLogDatabase;
class FaultLogManager {
public:
    // use rawdb manage fault log infos
    explicit FaultLogManager(std::shared_ptr<EventLoop> looper) : looper_(looper) {};
    virtual ~FaultLogManager();
    void Init();

    bool GetFaultLogContent(const std::string& name, std::string& content) const;
    int32_t CreateTempFaultLogFile(time_t time, int32_t id, int32_t faultType, const std::string& module) const;
    std::list<std::string> GetFaultLogFileList(const std::string& module, time_t time, int32_t id, int32_t faultType,
                                               int32_t maxNum) const;
    std::string SaveFaultLogToFile(FaultLogInfo& info) const;
    void SaveFaultInfoToRawDb(FaultLogInfo& info) const;
    std::list<FaultLogInfo> GetFaultInfoList(
        const std::string& module, int32_t id, int32_t faultType, int32_t maxNum) const;
    bool IsProcessedFault(int32_t pid, int32_t uid, int32_t faultType);

private:
    void ReduceLogFileListSize(std::list<std::string>& infoVec, int32_t maxNum) const;
    std::shared_ptr<EventLoop> looper_ = nullptr;
    std::unique_ptr<LogStoreEx> store_ = nullptr;
    FaultLogDatabase* faultLogDb_ = nullptr;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULTLOG_MANAGER_H