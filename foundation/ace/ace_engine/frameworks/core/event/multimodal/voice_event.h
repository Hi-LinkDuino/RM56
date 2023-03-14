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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_VOICE_EVENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_VOICE_EVENT_H

#include <string>
#include <utility>
#include <vector>

namespace OHOS::Ace {

enum class SceneLabel {
    COMMON = 0,
    VIDEO,
    AUDIO,
    PAGE,
    SWITCH,
};

class VoiceEvent final {
public:
    VoiceEvent() = default;
    ~VoiceEvent() = default;
    VoiceEvent(const std::string& voice, SceneLabel scene, bool isBadge = false)
        : voice_(voice), scene_(scene), isBadge_(isBadge)
    {}

    void SetBadgeList(const std::vector<std::pair<std::string, std::string>>& badgeList)
    {
        badges_ = badgeList;
    }

    const std::string& GetVoiceContent() const
    {
        return voice_;
    }

    SceneLabel GetVoiceScene() const
    {
        return scene_;
    }

    bool IsBadge() const
    {
        return isBadge_;
    }

    const std::vector<std::pair<std::string, std::string>>& GetBadgeList() const
    {
        return badges_;
    }

    bool operator==(const VoiceEvent& rhs) const
    {
        return voice_ == rhs.GetVoiceContent() && scene_ == rhs.GetVoiceScene();
    }

    bool operator!=(const VoiceEvent& rhs) const
    {
        return voice_ != rhs.GetVoiceContent() || scene_ != rhs.GetVoiceScene();
    }

private:
    std::string voice_;
    SceneLabel scene_ = SceneLabel::COMMON;
    bool isBadge_ = false;
    std::vector<std::pair<std::string, std::string>> badges_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_EVENT_MULTIMODAL_VOICE_EVENT_H
