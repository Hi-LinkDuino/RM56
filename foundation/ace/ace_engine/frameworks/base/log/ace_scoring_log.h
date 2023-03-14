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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_SCORING_LOG_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_SCORING_LOG_H

#include <string>

#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"

#define ACE_SCORING_EVENT(event) AceScoringLog AceScoringLog(event)
#define ACE_SCORING_COMPONENT(page, component, proc) AceScoringLog AceScoringLog(page, component, proc)

namespace OHOS::Ace {

class ACE_EXPORT AceScoringLog final {
public:
    explicit AceScoringLog(const std::string& eventName);
    AceScoringLog(const std::string& pageName, const std::string& componentType, const std::string& procType);

    ~AceScoringLog();

private:
    AceScoringLog() = default;

    void Init();

    uint64_t startTime_ = 0;
    uint64_t endTime_ = 0;
    std::string logInfo_;
    static std::string procName_;
    static bool isDebuggingEnabled_;

    ACE_DISALLOW_COPY_AND_MOVE(AceScoringLog);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_LOG_ACE_SCORING_LOG_H
