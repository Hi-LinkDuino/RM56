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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULTLOG_DATABASE_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULTLOG_DATABASE_H

#include <cstdint>
#include <ctime>
#include <list>
#include <memory>
#include <mutex>
#include <string>

#include "faultlog_info.h"
#include "sys_event.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLogDatabase {
public:
    FaultLogDatabase() {};
    ~FaultLogDatabase(){};
    void SaveFaultLogInfo(FaultLogInfo& info);
    std::list<FaultLogInfo> GetFaultInfoList(
        const std::string& module, int32_t id, int32_t faultType, int32_t maxNum);
    bool IsFaultExist(int32_t pid, int32_t uid, int32_t faultType);
private:
    std::mutex mutex_;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEWDFX_HIVIEW_FAULTLOGGER_FAULTLOG_DATABASE_H