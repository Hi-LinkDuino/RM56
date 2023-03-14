/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
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

#ifndef HISTREAMER_PIPELINE_CORE_EVENT_H
#define HISTREAMER_PIPELINE_CORE_EVENT_H

#include <string>
#include <map>
#include "plugin/common/any.h"

namespace OHOS {
namespace Media {
// 各个组件向Pipeline报告的事件类型
enum struct EventType : uint32_t {
    EVENT_READY = 0,
    EVENT_AUDIO_PROGRESS, // unit is HST_TIME_BASE
    EVENT_VIDEO_PROGRESS, // unit is HST_TIME_BASE
    EVENT_COMPLETE,
    EVENT_ERROR,
    EVENT_PLUGIN_ERROR,
    EVENT_PLUGIN_EVENT,
    EVENT_BUFFERING,
    EVENT_BUFFER_PROGRESS,
    EVENT_DECODER_ERROR,
};

struct Event {
    std::string srcFilter;
    EventType type;
    Plugin::Any param;
};

const char* GetEventName(EventType type);
} // namespace Media
} // namespace OHOS
#endif
