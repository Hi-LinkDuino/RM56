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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_ACE_EVENT_HELPER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_ACE_EVENT_HELPER_H

#include <functional>

#include "base/log/log.h"
#include "base/memory/referenced.h"
#include "core/event/ace_event_handler.h"
#include "core/event/back_end_event_manager.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

template<class>
class AceSyncEvent;

template<class... Args>
class AceSyncEvent<void(Args...)> final {
public:
    AceSyncEvent() = delete;
    ~AceSyncEvent() = delete;

    // Uses eventId and context to create Ace back-end event.
    // For sync event, the result needs to be referenced by out parameters.
    // When the event id starts with "Backend_" flag, then uses backend event manager processing.
    static std::function<void(Args...)> Create(const EventMarker& marker, const WeakPtr<PipelineContext>& context)
    {
        if (marker.IsEmpty()) {
            return nullptr;
        }
        return [marker, context](Args... args) {
            auto refContext = context.Upgrade();
            if (!refContext) {
                LOGE("fail to fire sync event due to context is nullptr");
                return;
            }
            refContext->FireSyncEvent(marker, std::forward<Args>(args)...);
        };
    }
};

template<class>
class AceAsyncEvent;

template<class... Args>
class AceAsyncEvent<void(Args...)> final {
public:
    AceAsyncEvent() = delete;
    ~AceAsyncEvent() = delete;

    // Uses eventId and context to create Ace back-end event.
    // When the event id starts with "Backend_" flag, then uses backend event manager processing.
    static std::function<void(Args...)> Create(const EventMarker& marker, const WeakPtr<PipelineContext>& context)
    {
        if ((marker.IsEmpty()) && (!marker.HasPreFunction())) {
            return nullptr;
        }
        return [marker, context](Args... args) {
            auto refContext = context.Upgrade();
            if (!refContext) {
                LOGE("fail to fire async event due to context is nullptr");
                return;
            }
            marker.CallPreFunction();
            if (marker.IsEmpty()) {
                LOGI("just call pre function, fail to fire async event due to marker is empty");
                return;
            }
            refContext->FireAsyncEvent(marker, std::forward<Args>(args)...);
        };
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_ACE_EVENT_HELPER_H
