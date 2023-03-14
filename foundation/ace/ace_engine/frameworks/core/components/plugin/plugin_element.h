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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_ELEMENT_H

#include "core/pipeline/base/render_element.h"
#include "core/components/plugin/plugin_component.h"
#include "core/components/plugin/plugin_sub_container.h"
#include "core/components/plugin/resource/plugin_manager_delegate.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {
class PluginElement : public RenderElement {
    DECLARE_ACE_TYPE(PluginElement, RenderElement);

public:
    using EventCallback = std::function<void(const std::string&)>;

    PluginElement() = default;
    ~PluginElement() override;

    void Prepare(const WeakPtr<Element>& parent) override;
    void Update()  override;
    void PerformBuild() override;

    RefPtr<RenderNode> CreateRenderNode() override;

    const RefPtr<PluginSubContainer> GetPluginSubContainer() const
    {
        return pluginSubContainer_;
    }

    void HandleOnCompleteEvent() const;
    void HandleOnErrorEvent(const std::string code, const std::string msg) const;
    void OnActionEvent(const std::string& action) const;

private:
    void RunPluginContainer();
    void InitEvent(const RefPtr<PluginComponent>& component);
    void SplitString(const std::string& str, char tag, std::vector<std::string>& strList);
    std::string GetPackagePath(const WeakPtr<PluginElement>& weak, RequestPluginInfo& info) const;
    std::string GetPackagePathByWant(const WeakPtr<PluginElement>& weak, RequestPluginInfo& info) const;
    std::string GetPackagePathByAbsolutePath(const WeakPtr<PluginElement>& weak, RequestPluginInfo& info) const;
    void GetModuleNameByWant(const WeakPtr<PluginElement>& weak, RequestPluginInfo& info) const;
    void RunPluginTask(const WeakPtr<PluginElement>& weak, const RefPtr<PluginComponent>& component);

private:
    RefPtr<PluginSubContainer> pluginSubContainer_;
    RefPtr<PluginManagerDelegate> pluginManagerBridge_;

    EventCallback onCompleteEvent_;
    EventCallback onErrorEvent_;
    RequestPluginInfo pluginInfo_;
    int64_t pluginSubContainerId_ = 0;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_PLUGIN_ELEMENT_H