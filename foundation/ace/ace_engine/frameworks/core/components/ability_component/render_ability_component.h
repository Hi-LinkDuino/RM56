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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RENDER_ABILITY_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RENDER_ABILITY_COMPONENT_H

#include "core/components/ability_component/ability_component.h"
#include "core/components/ability_component/resource/ability_component_delegate.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderAbilityComponent : public RenderNode {
    DECLARE_ACE_TYPE(RenderAbilityComponent, RenderNode);

public:
    ~RenderAbilityComponent() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;

protected:
    void OnGlobalPositionChanged() override;
    void OnSizeChanged() override;
    void ClearRenderObject() override;

    RefPtr<AbilityComponentDelegate> delegate_;
    bool hasPositionInited_ = false;

private:
    void UpdateRenderRect(bool isPositionChanged);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ABILITY_COMPONENT_RENDER_ABILITY_COMPONENT_H
