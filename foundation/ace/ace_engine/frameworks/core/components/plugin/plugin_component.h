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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/plugin/resource/plugin_request_data.h"
#include "core/pipeline/base/element.h"

namespace OHOS::Ace {
class ACE_EXPORT PluginComponent : public RenderComponent {
    DECLARE_ACE_TYPE(PluginComponent, RenderComponent);

public:
    PluginComponent() = default;
    ~PluginComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    void SetPluginRequestionInfo(const RequestPluginInfo& info)
    {
        info_ = info;
    }

    const RequestPluginInfo& GetPluginRequestionInfo() const
    {
        return info_;
    }

    void SetDimension(int32_t dimension)
    {
        info_.dimension = dimension;
    }

    void SetModuleName(const std::string& name)
    {
        info_.moduleName = name;
    }

    void SetPluginSize(const Dimension& width, const Dimension& height)
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

    void SetOnCompleteEventId(const EventMarker& event)
    {
        onComplete_ = event;
    }

    const EventMarker& GetOnCompleteEventId() const
    {
        return onComplete_;
    }

    void SetOnErrorEventId(const EventMarker& event)
    {
        onError_ = event;
    }

    const EventMarker& GetOnErrorEvent() const
    {
        return onError_;
    }

    void SetData(const std::string& data)
    {
        data_ = data;
    }

    const std::string& GetData() const
    {
        return data_;
    }

private:
    RequestPluginInfo info_;
    std::string data_;
    EventMarker onComplete_;
    EventMarker onError_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_COMPONENT_H