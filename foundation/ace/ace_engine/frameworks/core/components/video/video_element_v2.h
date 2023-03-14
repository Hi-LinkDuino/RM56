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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_ELEMENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_ELEMENT_V2_H

#include "core/components/video/video_component_v2.h"
#include "core/components/video/video_element.h"

namespace OHOS::Ace {

class VideoElementV2 : public VideoElement {
    DECLARE_ACE_TYPE(VideoElementV2, VideoElement);

public:
    VideoElementV2() = default;
    ~VideoElementV2() override = default;

    void SetNewComponent(const RefPtr<Component>& newComponent) override;

protected:
    void InitStatus(const RefPtr<VideoComponent>& videoComponent) override;
    void OnPreFullScreen(bool isPortrait) override;
    RefPtr<Component> GetRootComponent();
    void UpdateVideoComponent(const RefPtr<VideoComponentV2>& videoComponent);
    RefPtr<Component> FireFullscreen(
        bool isFullScreen, const WeakPtr<Player>& player, const WeakPtr<Texture>& texture);
    RefPtr<Component> GetEventComponents(const RefPtr<Component>& videoChild);

private:
    bool isFullscreen_ = false;
    RefPtr<VideoComponentV2> videoComponent_;
    RefPtr<Component> originComponent_;
    std::unordered_map<std::string, RefPtr<Component>> map_;

};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_VIDEO_VIDEO_ELEMENT_V2_H
