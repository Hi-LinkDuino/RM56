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
#ifndef EVENT_LOG_EVENT_LOG_ACTION_RUN
#define EVENT_LOG_EVENT_LOG_ACTION_RUN

#include <memory>
#include <string>
#include <vector>

#include "log_action.h"
#include "sys_event.h"
namespace OHOS {
namespace HiviewDFX {
class EventLogAction : public LogAction {
public:
    EventLogAction(int fd, std::shared_ptr<SysEvent> event): LogAction(fd, event) {};
    ~EventLogAction(){};
    void CaptureAction() override;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // EVENT_LOG_LOG_ACTION_RUN