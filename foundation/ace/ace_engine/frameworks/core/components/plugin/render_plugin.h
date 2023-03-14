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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RENDER_PLUGIN_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RENDER_PLUGIN_H

#include "core/components/plugin/plugin_component.h"
#include "core/components/plugin/plugin_sub_container.h"
#include "core/pipeline/base/render_sub_container.h"

namespace OHOS::Ace {
class RenderPlugin : public RenderSubContainer {
DECLARE_ACE_TYPE(RenderPlugin, RenderSubContainer);

public:
    static RefPtr<RenderNode> Create();

    ~RenderPlugin() override = default;

    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

    virtual std::unique_ptr<DrawDelegate> GetDrawDelegate()
    {
        return nullptr;
    }
    virtual void RemoveChildren() {}

    void SetPluginSubContainer(const WeakPtr<PluginSubContainer>& container)
    {
        pluginContainer_ = container;
    }

    RefPtr<PipelineContext> GetSubPipelineContext() override
    {
        auto context = pluginContainer_.Upgrade();
        if (context) {
            return context->GetPipelineContext();
        }
        return nullptr;
    }

    bool TouchTest(const Point& globalPoint,
        const Point& parentLocalPoint, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
private:
    Dimension rootWidth_ = 0.0_vp;
    Dimension rootHeight_ = 0.0_vp;
    Size drawSize_;
    WeakPtr<PluginSubContainer> pluginContainer_;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PLUGIN_RENDER_PLUGIN_H
