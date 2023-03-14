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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_SUBSCRIBER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_SUBSCRIBER_H

#include <functional>

#include "base/memory/ace_type.h"
#include "core/event/multimodal/ace_multimodal_event.h"
#include "core/event/multimodal/voice_event.h"

namespace OHOS::Ace {

using MultimodalEventCallback = std::function<void(const AceMultimodalEvent&)>;

class MultimodalSubscriber : public AceType {
    DECLARE_ACE_TYPE(MultimodalSubscriber, AceType)

public:
    virtual void RegisterCallback(
        const MultimodalEventCallback& callback, const std::function<void()>& successCallback) = 0;
    virtual bool SubscribeVoiceEvents(const std::vector<VoiceEvent>& voiceEvents) = 0;
    virtual void UnSubscribeVoiceEvents(const std::vector<VoiceEvent>& voiceEvents) = 0;
    virtual void UnSubscribeAllEvents() = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_SUBSCRIBER_H
