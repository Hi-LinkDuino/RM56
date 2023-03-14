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

#include "core/components/declaration/common/event.h"

#include "core/event/ace_event_helper.h"
#include "core/event/back_end_event_manager.h"

namespace OHOS::Ace {

void EventParam::FireEvent(const WeakPtr<PipelineContext>& context)
{
    auto eventContext = context.Upgrade();
    if (!eventContext) {
        return;
    }
    auto event = AceAsyncEvent<void()>::Create(eventMarker, context);
    event();
}

void TouchEventParam::FireEvent(const WeakPtr<PipelineContext>& context)
{
    auto eventContext = context.Upgrade();
    if (!eventContext) {
        return;
    }
    auto event = AceAsyncEvent<void(const TouchEventInfo&)>::Create(eventMarker, context);
    event(touchEvent);
}

} // namespace OHOS::Ace