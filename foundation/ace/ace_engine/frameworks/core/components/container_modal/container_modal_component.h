/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CONTAINER_MODAL_CONTAINER_MODAL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CONTAINER_MODAL_CONTAINER_MODAL_COMPONENT_H

#include "core/components/button/button_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/image/image_component.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ContainerModalComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ContainerModalComponent, SoleChildComponent);

public:
    explicit ContainerModalComponent(const WeakPtr<PipelineContext>& context)
    {
        context_ = context;
    }

    ~ContainerModalComponent() override = default;
    static RefPtr<Component> Create(const WeakPtr<PipelineContext>& context, const RefPtr<Component>& child);
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;
    void BuildInnerChild();
    std::list<RefPtr<Component>> BuildTitleChildren(bool isFloating, bool isFocus = true);

    RefPtr<ImageComponent> GetTitleIcon() const
    {
        return titleIcon_;
    }

    RefPtr<TextComponent> GetTitleLabel() const
    {
        return titleLabel_;
    }

private:
    RefPtr<Component> BuildTitle();
    RefPtr<Component> BuildFloatingTitle();
    RefPtr<Component> BuildContent();
    RefPtr<ButtonComponent> BuildControlButton(
        InternalResource::ResourceId icon, std::function<void()>&& clickCallback, bool isFocus);
    static RefPtr<Component> SetPadding(
        const RefPtr<Component>& component, const Dimension& leftPadding, const Dimension& rightPadding);

    WeakPtr<PipelineContext> context_;
    RefPtr<ImageComponent> titleIcon_;
    RefPtr<TextComponent> titleLabel_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CONTAINER_MODAL_CONTAINER_MODAL_COMPONENT_H
