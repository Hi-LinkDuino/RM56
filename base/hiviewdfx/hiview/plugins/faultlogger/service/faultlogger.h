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
#ifndef HIVIEWDFX_HIVIEW_FAULTLOGGER_H
#define HIVIEWDFX_HIVIEW_FAULTLOGGER_H
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "event.h"
#include "plugin.h"

#include "faultlog_info.h"
#include "faultlog_manager.h"
#include "faultlog_query_result_inner.h"
#include "faultlogger_plugin.h"

namespace OHOS {
namespace HiviewDFX {
struct DumpRequest {
    bool requestDetail;
    bool requestList;
    std::string fileName;
    std::string moduleName;
    time_t time;
};
class Faultlogger : public FaultloggerPlugin, public EventListener {
public:
    Faultlogger() : mgr_(nullptr), hasInit_(false) {};
    virtual ~Faultlogger(){};

    // implementations of Plugin interfaces
    // for intercepting JavaCrash and AppFreeze from collectors pipeline
    bool OnEvent(std::shared_ptr<Event> &event) override;
    bool CanProcessEvent(std::shared_ptr<Event> event) override;
    bool ReadyToLoad() override;
    void OnLoad() override;

    // dump debug infos through cmdline
    void Dump(int fd, const std::vector<std::string> &cmds) override;

    // implementations of FaultloggerPlugin interfaces
    void AddFaultLog(FaultLogInfo& info) override;
    std::unique_ptr<FaultLogInfo> GetFaultLogInfo(const std::string& logPath) override;
    std::unique_ptr<FaultLogQueryResultInner> QuerySelfFaultLog(int32_t uid,
        int32_t pid, int32_t faultType, int32_t maxNum) override;

    // implementations of EventListener interfaces
    // for intercepting JsCrash from engine pipeline
    void OnUnorderedEvent(const Event &msg) override;
    std::string GetListenerName() override;

private:
    void AddFaultLogIfNeed(FaultLogInfo& info, std::shared_ptr<Event> event);
    void AddPublicInfo(FaultLogInfo& info);
    void Dump(int fd, const DumpRequest& request) const;
    void StartBootScan();
    std::unique_ptr<FaultLogManager> mgr_;
    volatile bool hasInit_;
};
}  // namespace HiviewDFX
}  // namespace OHOS
#endif  // HIVIEWDFX_HIVIEW_FAULTLOGGER_H