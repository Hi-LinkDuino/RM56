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
#ifndef EVENT_LOG_LOG_ACTION
#define EVENT_LOG_LOG_ACTION

#include <memory>
#include <string>
#include <vector>

#include "sys_event.h"

#include "event_log_task.h"
namespace OHOS {
namespace HiviewDFX {
class LogAction {
public:
    LogAction(int fd, std::shared_ptr<SysEvent> event)
        : fd_(fd),
          event_(event) {};

    virtual ~LogAction(){};
    virtual void CaptureAction();
    void Init();
protected:
    int fd_;
    std::shared_ptr<SysEvent> event_;
    std::vector<std::string> cmdList_;
private:
    std::unique_ptr<EventLogTask> logTask_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // EVENT_LOG_LOG_ACTION