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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_EVENT_TRIGGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_EVENT_TRIGGER_H

#include "base/log/log.h"
#include "core/event/ace_event_handler.h"
#include "core/event/ace_events.h"
#include "core/event/back_end_event_manager.h"

namespace OHOS::Ace {

class EventTrigger final {
public:
    void RegisterEventHandler(const RefPtr<AceEventHandler>& handler)
    {
        handler_ = handler;
    }

    template<class... Args>
    void TriggerAsyncEvent(const EventMarker& marker, Args&&... args)
    {
        if (!marker.GetData().isFront) {
            LOGD("begin to use back end event manager to fire async event");
            BackEndEventManager<void(Args...)>::GetInstance().FireBackEndEvent(marker, std::forward<Args>(args)...);
            LOGD("finish to use back end event manager to fire async event");
            return;
        }
        LOGD("begin to use event handler to fire async event");
        auto handler = handler_.Upgrade();
        if (!handler) {
            LOGE("fail to trigger async event due to event handler is nullptr");
            return;
        }
        handler->HandleAsyncEvent(marker, std::forward<Args>(args)...);
        LOGD("finish to use event handler to fire async event");
    }

    template<class... Args>
    void TriggerSyncEvent(const EventMarker& marker, Args&&... args)
    {
        if (!marker.GetData().isFront) {
            LOGD("begin to use back end event manager to fire sync event");
            BackEndEventManager<void(Args...)>::GetInstance().FireBackEndEvent(marker, std::forward<Args>(args)...);
            LOGD("finish to use back end event manager to fire sync event");
            return;
        }
        LOGD("begin to use event handler to fire sync event");
        auto handler = handler_.Upgrade();
        if (!handler) {
            LOGE("fail to trigger sync event due to event handler is nullptr");
            return;
        }
        handler->HandleSyncEvent(marker, std::forward<Args>(args)...);
        LOGD("finish to use event handler to fire sync event");
    }

private:
    WeakPtr<AceEventHandler> handler_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_EVENT_TRIGGER_H
