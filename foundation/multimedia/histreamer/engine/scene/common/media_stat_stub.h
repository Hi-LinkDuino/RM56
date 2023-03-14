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

#ifndef HISTREAMER_PIPELINE_COMMON_MEDIA_STAT_STUB_H
#define HISTREAMER_PIPELINE_COMMON_MEDIA_STAT_STUB_H

#include <cstdint>
#include <atomic>
#include <vector>

#include "pipeline/core/event.h"

namespace OHOS {
namespace Media {
class MediaStatStub {
public:
    struct MediaStat {
        std::string reporter;
        std::atomic<bool> completeEventReceived {false};
        explicit MediaStat(std::string rep) : reporter(std::move(rep))
        {
        }
        MediaStat(const MediaStat& other) : reporter(other.reporter)
        {
            completeEventReceived = other.completeEventReceived.load();
        }
        MediaStat& operator=(const MediaStat& other)
        {
            completeEventReceived = other.completeEventReceived.load();
            return *this;
        }
    };

    MediaStatStub() = default;
    void Reset();
    void Append(const std::string& reporter);
    void ReceiveEvent(const Event& event);
    bool IsEventCompleteAllReceived();

private:
    std::vector<MediaStat> mediaStats_;
};
} // Media
} // OHOS
#endif // HISTREAMER_PIPELINE_COMMON_MEDIA_STAT_STUB_H
