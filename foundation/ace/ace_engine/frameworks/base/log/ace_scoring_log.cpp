/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "base/log/ace_scoring_log.h"

#include <cinttypes>

#include "base/log/log.h"
#include "base/utils/system_properties.h"
#include "base/utils/time_util.h"
#include "core/common/ace_application_info.h"

namespace OHOS::Ace {

std::string AceScoringLog::procName_;
bool AceScoringLog::isDebuggingEnabled_ = false;

AceScoringLog::AceScoringLog(const std::string& eventName)
{
    Init();
    if (!AceScoringLog::isDebuggingEnabled_) {
        return;
    }
    logInfo_ = eventName;
}

AceScoringLog::AceScoringLog(const std::string& pageName, const std::string& componentType, const std::string& procType)
{
    Init();
    if (!AceScoringLog::isDebuggingEnabled_) {
        return;
    }
    logInfo_ = pageName;
    logInfo_.append(" ");
    logInfo_.append(componentType);
    logInfo_.append(" ");
    logInfo_.append(procType);
}

void AceScoringLog::Init()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, []() {
        AceScoringLog::procName_ = AceApplicationInfo::GetInstance().GetProcessName().empty()
                                       ? AceApplicationInfo::GetInstance().GetPackageName()
                                       : AceApplicationInfo::GetInstance().GetProcessName();
        AceScoringLog::isDebuggingEnabled_ = SystemProperties::IsScoringEnabled(AceScoringLog::procName_);
        LOGI("AceScoringLog enabled");
    });
    startTime_ = GetSysTimestamp();
}

AceScoringLog::~AceScoringLog()
{
    if (!AceScoringLog::isDebuggingEnabled_) {
        return;
    }
    endTime_ = GetSysTimestamp();
    LOGI("%{public}s %{public}s %{public}" PRIu64 " %{public}" PRIu64 "", procName_.c_str(), logInfo_.c_str(),
        startTime_, endTime_);
}

} // namespace OHOS::Ace
