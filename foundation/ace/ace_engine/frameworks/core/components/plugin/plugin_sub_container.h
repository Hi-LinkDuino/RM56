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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_SUB_CONTAINER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_SUB_CONTAINER_H

#include "core/components/plugin/file_asset_provider.h"
#include "base/thread/task_executor.h"
#include "frameworks/bridge/plugin_frontend/plugin_frontend.h"
#include "frameworks/core/pipeline/pipeline_context.h"

namespace OHOS::Ace {
class ACE_EXPORT PluginSubContainer : public virtual AceType {
    DECLARE_ACE_TYPE(PluginSubContainer, AceType);

public:
    using OnPluginAcquiredCallback = std::function<void(const size_t)>;

    explicit PluginSubContainer(const WeakPtr<PipelineContext>& context) : outSidePipelineContext_(context) {}
    ~PluginSubContainer() = default;

    void Initialize();
    void RunPlugin(
        const std::string& path, const std::string& module, const std::string& source, const std::string& data);
    void UpdatePlugin(const std::string& content);
    void Destroy();

    void SetPluginElement(const WeakPtr<Element>& element)
    {
        pluginElement_ = element;
    }

    const WeakPtr<Element> GetPluginElement() const
    {
        return pluginElement_;
    }

    void SetPluginComponet(const RefPtr<Component>& mountPoint)
    {
        pluginComponent_ = mountPoint;
    }

    RefPtr<TaskExecutor> GetTaskExecutor() const
    {
        return taskExecutor_;
    }

    RefPtr<PipelineContext> GetPipelineContext() const
    {
        return pipelineContext_;
    }

    void UpdateRootElmentSize();
    void UpdateSurfaceSize();

    void SetAllowUpdate(bool update)
    {
        allowUpdate_ = update;
    }

    void SetInstanceId(int32_t instanceId)
    {
        instanceId_ = instanceId;
    }

private:
    void SetPluginComponentTheme(
        const std::string& path, const RefPtr<FlutterAssetManager>& flutterAssetManager);
    void SetActionEventHandler();
    RefPtr<FlutterAssetManager> SetAssetManager(const std::string& path, const std::string& module);

private:
    RefPtr<PluginFrontend> frontend_;
    RefPtr<TaskExecutor> taskExecutor_;
    RefPtr<PipelineContext> pipelineContext_;
    WeakPtr<PipelineContext> outSidePipelineContext_;
    RefPtr<AssetManager> assetManager_;

    int32_t instanceId_ = 0;
    bool isArkApp_ = false;

    bool allowUpdate_ = true;

    RefPtr<Component> pluginComponent_;
    WeakPtr<Element> pluginElement_;

    double surfaceWidth_ = 1.0f;
    double surfaceHeight_ = 1.0f;
    Dimension rootWidth_ = 0.0_vp;
    Dimension rootHeight_ = 0.0_vp;
    double density_ = 1.0f;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_SUB_CONTAINER_H
