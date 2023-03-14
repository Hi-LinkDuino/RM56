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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_VIDEO_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_VIDEO_H

#include "core/components/video/video_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"
#include "frameworks/core/components/common/properties/decoration.h"

namespace OHOS::Ace::Framework {

class DOMVideo final : public DOMNode {
    DECLARE_ACE_TYPE(DOMVideo, DOMNode);

public:
    DOMVideo(NodeId nodeId, const std::string& nodeName);
    ~DOMVideo() override = default;

    RefPtr<Component> GetSpecializedComponent() override
    {
        if (videoChild_->GetSrc().empty()) {
            return nullptr;
        }
        return videoChild_;
    }

    RefPtr<Component> FireFullscreen(
        bool isFullScreen, const WeakPtr<Player>& player, const WeakPtr<Texture>& texture);

    bool OnFullScreen() const
    {
        return isFullscreen_;
    }
    RefPtr<Component> GetEventComponents(const RefPtr<Component>& videoChild);

    bool IsLeafNode() const override
    {
        return true;
    }

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    bool AddSpecializedEvent(int32_t pageId, const std::string& event) override;
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

private:
    static ImageFit ConvertStrToFit(const std::string& fit);
    static ImageObjectPosition ConvertStrToPosition(const std::string& position);
    static std::unique_ptr<JsonValue> GetParamFromJson(const std::string& json, const std::string& key);

    RefPtr<VideoComponent> videoChild_;
    RefPtr<Component> originComponent_;
    bool isFullscreen_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_VIDEO_H
