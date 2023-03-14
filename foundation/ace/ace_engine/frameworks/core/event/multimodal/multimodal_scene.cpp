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

#include "core/event/multimodal/multimodal_scene.h"

#include <algorithm>

#include "base/log/dump_log.h"
#include "base/log/log.h"

namespace OHOS::Ace {

std::string MultiModalScene::GetAvailableSubscriptId()
{
    if (cachedIds_.empty()) {
        auto newId = currentAvailableId_++;
        return std::to_string(newId);
    }
    auto cachedId = *(cachedIds_.begin());
    cachedIds_.erase(cachedIds_.begin());
    return cachedId;
}

void MultiModalScene::RemoveSubscriptId(const std::string& subscriptId)
{
    cachedIds_.emplace(subscriptId);
}

std::string MultiModalScene::GetCurrentMaxSubscriptId()
{
    return std::to_string(currentAvailableId_ - 1);
}

bool MultiModalScene::SubscribeVoiceEvent(const VoiceEvent& voiceEvent, const MultimodalEventCallback& callback)
{
    if (!callback) {
        LOGW("fail to subscribe voice event due to callback is null");
        return false;
    }
    auto result = voiceEventCallbacks_.try_emplace(voiceEvent.GetVoiceContent(), callback);
    if (!result.second) {
        LOGW("subscribe duplicate voice event, voice label %{private}s", voiceEvent.GetVoiceContent().c_str());
        return false;
    }
    voiceEvents_.emplace_back(voiceEvent);
    if (!subscriber_) {
        LOGE("fail to subscribe voice event due to subscriber is null");
        return false;
    }
    if (!subscriber_->SubscribeVoiceEvents({ voiceEvent })) {
        LOGE("fail to subscribe voice event due to subscribe fail");
        return false;
    }
    return true;
}

void MultiModalScene::UnSubscribeVoiceEvent(const VoiceEvent& voiceEvent)
{
    voiceEventCallbacks_.erase(voiceEvent.GetVoiceContent());
    auto iter = std::remove(voiceEvents_.begin(), voiceEvents_.end(), voiceEvent);
    if (iter != voiceEvents_.end()) {
        voiceEvents_.erase(iter);
        if (subscriber_) {
            subscriber_->UnSubscribeVoiceEvents({ voiceEvent });
        }
    }
}

void MultiModalScene::SubscribeSubscriptSwitchEvent(const EventCallback<void(bool)>& callback)
{
    subscriptSwitchListeners_.emplace_back(callback);
}

void MultiModalScene::UnSubscribeSubscriptSwitchEvent(const EventCallback<void(bool)>& callback)
{
    auto iter = std::remove(subscriptSwitchListeners_.begin(), subscriptSwitchListeners_.end(), callback);
    if (iter != subscriptSwitchListeners_.end()) {
        subscriptSwitchListeners_.erase(iter);
    }
}

void MultiModalScene::OnNotifyMultimodalEvent(const AceMultimodalEvent& event)
{
    static const int32_t SHOW_BADGE = 6;
    static const int32_t HIDE_BADGE = 7;
    if (event.GetVoice().action == SHOW_BADGE || event.GetVoice().action == HIDE_BADGE) {
        badgeFlag_ = event.GetVoice().action == SHOW_BADGE;
        for (const auto& callback : subscriptSwitchListeners_) {
            if (callback) {
                callback(event.GetVoice().action == SHOW_BADGE);
            }
        }
    }
    auto voiceIter = voiceEventCallbacks_.find(event.GetVoice().hotWord);
    if (voiceIter != voiceEventCallbacks_.end()) {
        (voiceIter->second)(event);
        return;
    }
    auto badgeIter = voiceEventCallbacks_.find(event.GetVoice().badge);
    if (badgeIter != voiceEventCallbacks_.end()) {
        (badgeIter->second)(event);
    }
}

void MultiModalScene::UnSubscribeAllEvents()
{
    if (subscriber_ && !voiceEvents_.empty()) {
        subscriber_->UnSubscribeVoiceEvents(voiceEvents_);
    }
    voiceEvents_.clear();
    voiceEventCallbacks_.clear();
    subscriptSwitchListeners_.clear();
}

void MultiModalScene::Hide()
{
    if (subscriber_ && !voiceEvents_.empty()) {
        subscriber_->UnSubscribeVoiceEvents(voiceEvents_);
    }
}

void MultiModalScene::Resume()
{
    if (subscriber_ && !voiceEvents_.empty()) {
        subscriber_->SubscribeVoiceEvents(voiceEvents_);
    }
}

MultiModalScene::~MultiModalScene()
{
    UnSubscribeAllEvents();
}

void MultiModalScene::Dump() const
{
    if (DumpLog::GetInstance().GetDumpFile()) {
        for (const auto& event : voiceEvents_) {
            DumpLog::GetInstance().AddDesc("voice event: ", event.GetVoiceContent());
            DumpLog::GetInstance().AddDesc("badge event: ", event.IsBadge());
        }
        DumpLog::GetInstance().AddDesc("current badge id: ", currentAvailableId_);
        DumpLog::GetInstance().AddDesc("badge flag: ", badgeFlag_);
        DumpLog::GetInstance().Print(0, GetTypeName(), voiceEvents_.size());
    }
}

} // namespace OHOS::Ace