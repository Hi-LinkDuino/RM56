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
#include "log_action.h"

#include "logger.h"
#include "string_util.h"
namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_LABEL(0xD002D01, "EventLogger-LogAction");
void LogAction::Init()
{
    logTask_ = std::make_unique<EventLogTask>(fd_, event_);
    std::string cmdStr = event_->GetValue("eventLog_action");
    StringUtil::SplitStr(cmdStr, ",", cmdList_);
}

void LogAction::CaptureAction()
{
    for (std::string cmd : cmdList_) {
        logTask_->AddLog(cmd);
    }
    auto ret = logTask_->StartCompose();
    if (ret != EventLogTask::TASK_SUCCESS) {
        HIVIEW_LOGE("capture fail %{public}d", ret);
    }
}
}
}