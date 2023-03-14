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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_ROOT_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_ROOT_COMPONENT_H

#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class RootElement;
class PipelineContext;

// The root component of a component tree,
// Bridge the Element to RenderNode.
class RootComponent : public RenderComponent {
    DECLARE_ACE_TYPE(RootComponent, RenderComponent);

public:
    explicit RootComponent(const RefPtr<Component>& child);
    ~RootComponent() override = default;

    static RefPtr<RootComponent> Create(const RefPtr<Component>& child);

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
    RefPtr<RootElement> SetupElementTree(const RefPtr<PipelineContext>& context);
    const RefPtr<Component>& GetChild() const
    {
        return child_;
    }

    void MarkContextMenu()
    {
        isContextMenu_ = true;
    }

    bool IsContextMenu()
    {
        return isContextMenu_;
    }
private:
    RefPtr<Component> child_;
    bool isContextMenu_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_ROOT_ROOT_COMPONENT_H
