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

#include "base/log/ace_tracker.h"

#include "base/log/log.h"
#include "base/utils/time_util.h"

namespace OHOS::Ace {

std::unique_ptr<JsonValue> AceTracker::trackInfo_ = nullptr;

void AceTracker::Start()
{
    trackInfo_ = JsonUtil::Create(true);
}

std::string AceTracker::Stop()
{
    if (trackInfo_) {
        auto info = trackInfo_->ToString();
        trackInfo_.reset(nullptr);
        return info;
    }
    return "{}";
}

AceScopedTracker::AceScopedTracker(const std::string& tag) : tag_(tag)
{
    if (AceTracker::trackInfo_) {
        // micro sec.
        markTime_ = GetMicroTickCount();
    }
}
AceScopedTracker::~AceScopedTracker()
{
    if (AceTracker::trackInfo_) {
        // convert micro sec to ms with 1000.
        AceTracker::trackInfo_->Put(tag_.c_str(), (GetMicroTickCount() - markTime_) / 1000.0);
    }
}

} // namespace OHOS::Ace
