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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATOR_NAVIGATOR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATOR_NAVIGATOR_COMPONENT_H

#include <string>

#include "base/utils/macros.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

enum class NavigatorType {
    DEFAULT = 0,
    PUSH,
    REPLACE,
    BACK
};

class ACE_EXPORT NavigatorComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(NavigatorComponent, SoleChildComponent);

public:
    explicit NavigatorComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~NavigatorComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    std::string GetUri() const
    {
        return uri_;
    }

    void SetUri(const std::string& uri)
    {
        uri_ = uri;
    }

    NavigatorType GetType() const
    {
        return type_;
    }

    void SetType(NavigatorType type)
    {
        type_ = type;
    }

    bool GetActive() const
    {
        return active_;
    }

    void SetParams(const std::string& params)
    {
        params_ = params;
    }

    const std::string& GetParams() const
    {
        return params_;
    }

    void SetActive(bool active)
    {
        active_ = active;
    }

    bool IsDefHeight() const
    {
        return isDefHeight_;
    }

    void SetIsDefHeight(bool isDefHeight)
    {
        isDefHeight_ = isDefHeight;
    }

    bool IsDefWidth() const
    {
        return isDefWidth_;
    }

    void SetIsDefWidth(bool isDefWidth)
    {
        isDefWidth_ = isDefWidth;
    }

    const EventMarker& GetClickedEventId() const
    {
        return clickEventId_;
    }

    void SetClickedEventId(const EventMarker& eventId)
    {
        clickEventId_ = eventId;
    }

private:
    std::string uri_;
    std::string params_;
    NavigatorType type_ = NavigatorType::PUSH;
    bool active_ = false;
    bool isDefHeight_ = false;
    bool isDefWidth_ = false;
    EventMarker clickEventId_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NAVIGATOR_NAVIGATOR_COMPONENT_H
