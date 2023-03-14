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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/form/resource/form_request_data.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {

class ACE_EXPORT FormComponent : public RenderComponent {
    DECLARE_ACE_TYPE(FormComponent, RenderComponent);

public:
    FormComponent() = default;
    ~FormComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetFormRequestionInfo(const RequestFormInfo& info)
    {
        info_ = info;
    }

    RequestFormInfo GetFormRequestionInfo() const
    {
        return info_;
    }

    void SetAllowUpdate(bool allow)
    {
        info_.allowUpate = allow;
    }

    bool IsAllowUpdate() const
    {
        return info_.allowUpate;
    }

    void SetDimension(int32_t dimension)
    {
        info_.dimension = dimension;
    }

    void SetModuleName(const std::string& name)
    {
        info_.moduleName = name;
    }

    void SetCardSize(const Dimension& width, const Dimension& height)
    {
        info_.width = width;
        info_.height = height;
    }

    const Dimension& GetWidth() const
    {
        return info_.width;
    }

    const Dimension& GetHeight() const
    {
        return info_.height;
    }

    void SetOnAcquireFormEventId(const EventMarker& event)
    {
        onAcquireForm_ = event;
    }

    const EventMarker& GetOnAcquireFormEventId() const
    {
        return onAcquireForm_;
    }

    void SetOnErrorEventId(const EventMarker& event)
    {
        onError_ = event;
    }

    const EventMarker& GetOnErrorEvent() const
    {
        return onError_;
    }

    void SetOnUninstallEventId(const EventMarker& event)
    {
        onUninstall_ = event;
    }

    const EventMarker& GetOnUninstallEvent() const
    {
        return onUninstall_;
    }

    void SetOnRouterEventId(const EventMarker& event)
    {
        onRouter_ = event;
    }

    const EventMarker& GetOnRouterEvent() const
    {
        return onRouter_;
    }

private:
    RequestFormInfo info_;
    EventMarker onAcquireForm_;
    EventMarker onError_;
    EventMarker onUninstall_;
    EventMarker onRouter_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FORM_FORM_COMPONENT_H
