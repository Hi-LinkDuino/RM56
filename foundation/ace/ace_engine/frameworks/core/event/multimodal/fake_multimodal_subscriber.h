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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_FAKE_MULTIMODAL_SUBSCRIBER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_FAKE_MULTIMODAL_SUBSCRIBER_H

#include "core/event/multimodal/multimodal_subscriber.h"

namespace OHOS::Ace::Platform {

class FakeMultimodalSubscriber : public MultimodalSubscriber {
    DECLARE_ACE_TYPE(FakeMultimodalSubscriber, MultimodalSubscriber)

public:
    FakeMultimodalSubscriber() = default;
    ~FakeMultimodalSubscriber() override = default;

    void RegisterCallback(const std::function<void(const AceMultimodalEvent&)>& callback,
                          const std::function<void()>& successCallback) override {}
    bool SubscribeVoiceEvents(const std::vector<VoiceEvent>& voiceEvents) override
    {
        return false;
    }
    void UnSubscribeVoiceEvents(const std::vector<VoiceEvent>& voiceEvents) override {}
    void UnSubscribeAllEvents() override {}
};

} // namespace OHOS::Ace::Platform

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_FAKE_MULTIMODAL_SUBSCRIBER_H