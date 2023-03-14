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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_PLUGIN_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_PLUGIN_H
#include <cstdint>
#include <ctime>
#include <memory>
#include <string>

#include "faultlog_info.h"
#include "faultlog_query_result_inner.h"
#include "plugin.h"

namespace OHOS {
namespace HiviewDFX {
constexpr char FAULTLOGGER_PLUGIN_NAME[] = "Faultlogger";
class FaultloggerPlugin : public Plugin {
public:
    FaultloggerPlugin() = default;
    virtual ~FaultloggerPlugin(){};

    // faultlogger interfaces
    // Add log for specific fault type
    virtual void AddFaultLog(FaultLogInfo& info){};

    // Get FaultlogInfo from file
    virtual std::unique_ptr<FaultLogInfo> GetFaultLogInfo(const std::string& logPath)
    {
        return nullptr;
    };

    // Query self fault log
    virtual std::unique_ptr<FaultLogQueryResultInner> QuerySelfFaultLog(int32_t uid, int32_t pid,
        int32_t faultType, int32_t maxNum)
    {
        return nullptr;
    };
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif
