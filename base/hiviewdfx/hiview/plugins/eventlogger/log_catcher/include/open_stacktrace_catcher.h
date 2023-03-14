/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef EVENT_LOGGER_OPEN_STACKTRACE_LOG_CATCHER
#define EVENT_LOGGER_OPEN_STACKTRACE_LOG_CATCHER

#include <string>

#include "event_log_catcher.h"
namespace OHOS {
namespace HiviewDFX {
class OpenStacktraceCatcher : public EventLogCatcher {
public:
    explicit OpenStacktraceCatcher();
    ~OpenStacktraceCatcher() override{};
    bool Initialize(const std::string& packageNam, int pid, int intParam2) override;
    int Catch(int fd) override;

private:
    int32_t ForkAndDumpStackTrace(int32_t fd);
    static void WaitChildPid(pid_t pid);
    static const inline int SLEEP_TIME_US = 100000;
    static const inline int MAX_RETRY_COUNT = 10;
    int32_t pid_ = -1;
    std::string packageName_ = "";
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // EVENT_LOGGER_BACKTRACE_LOG_CATCHER
