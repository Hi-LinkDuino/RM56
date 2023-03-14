/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_SCENE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_SCENE_H

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "base/memory/ace_type.h"
#include "base/utils/event_callback.h"
#include "core/event/multimodal/multimodal_subscriber.h"

namespace OHOS::Ace {

class MultiModalScene final : public AceType {
    DECLARE_ACE_TYPE(MultiModalScene, AceType)

public:
    MultiModalScene(int32_t pageId, const RefPtr<MultimodalSubscriber>& subscriber)
        : subscriber_(subscriber), pageId_(pageId) {}
    ~MultiModalScene() override;

    std::string GetAvailableSubscriptId();
    std::string GetCurrentMaxSubscriptId();
    void RemoveSubscriptId(const std::string& subscriptId);

    bool SubscribeVoiceEvent(const VoiceEvent& voiceEvent, const MultimodalEventCallback& callback);
    void UnSubscribeVoiceEvent(const VoiceEvent& voiceEvent);
    void SubscribeSubscriptSwitchEvent(const EventCallback<void(bool)>& callback);
    void UnSubscribeSubscriptSwitchEvent(const EventCallback<void(bool)>& callback);
    void OnNotifyMultimodalEvent(const AceMultimodalEvent& event);
    void UnSubscribeAllEvents();

    void Hide();
    void Resume();

    int32_t GetPageId() const
    {
        return pageId_;
    }

    void Dump() const;

private:
    std::unordered_set<std::string> cachedIds_;
    std::vector<VoiceEvent> voiceEvents_;
    std::vector<EventCallback<void(bool)>> subscriptSwitchListeners_;
    std::unordered_map<std::string, MultimodalEventCallback> voiceEventCallbacks_;
    RefPtr<MultimodalSubscriber> subscriber_;
    int32_t currentAvailableId_ = 1;
    int32_t pageId_ = -1;
    bool badgeFlag_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_MULTIMODAL_SCENE_H
