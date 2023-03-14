/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#define HST_LOG_TAG "MediaStatStub"

#include "media_stat_stub.h"

#include <algorithm>
#include "foundation/log.h"

namespace OHOS {
namespace Media {
void MediaStatStub::Reset()
{
    mediaStats_.clear();
}

void MediaStatStub::Append(const std::string& reporter)
{
    for (auto& stat : mediaStats_) {
        if (stat.reporter == reporter) {
            return;
        }
    }
    mediaStats_.emplace_back(reporter);
}

void MediaStatStub::ReceiveEvent(const Event& event)
{
    switch (event.type) {
        case EventType::EVENT_COMPLETE:
            for (auto& stat : mediaStats_) {
                if (stat.reporter == event.srcFilter) {
                    stat.completeEventReceived = true;
                    break;
                }
            }
            break;
        default:
            MEDIA_LOG_W("MediaStats::ReceiveEvent receive unexpected event " PUBLIC_LOG_D32,
                        static_cast<int>(event.type));
            break;
    }
}

bool MediaStatStub::IsEventCompleteAllReceived()
{
    return std::all_of(mediaStats_.begin(), mediaStats_.end(), [](const MediaStat& stat) {
        return stat.completeEventReceived.load();
    });
}
} // Media
} // OHOS