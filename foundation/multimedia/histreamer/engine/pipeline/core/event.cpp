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
#include "event.h"

namespace OHOS {
namespace Media {
static const std::map<EventType, const char*> g_EventTypeMap = {
    {EventType::EVENT_READY, "EVENT_READY"},
    {EventType::EVENT_AUDIO_PROGRESS, "EVENT_AUDIO_PROGRESS"},
    {EventType::EVENT_VIDEO_PROGRESS, "EVENT_VIDEO_PROGRESS"},
    {EventType::EVENT_COMPLETE, "EVENT_COMPLETE"},
    {EventType::EVENT_ERROR, "EVENT_ERROR"},
    {EventType::EVENT_PLUGIN_ERROR, "EVENT_PLUGIN_ERROR"},
    {EventType::EVENT_PLUGIN_EVENT, "EVENT_PLUGIN_EVENT"},
    {EventType::EVENT_BUFFERING, "EVENT_BUFFERING"},
    {EventType::EVENT_BUFFER_PROGRESS, "EVENT_BUFFER_PROGRESS"},
    {EventType::EVENT_DECODER_ERROR, "EVENT_DECODER_ERROR"}
};

const char* GetEventName(EventType type)
{
    auto it = g_EventTypeMap.find(type);
    if( it!= g_EventTypeMap.end()) {
        return it->second;
    }
    return "Unknow event";
}
}
}