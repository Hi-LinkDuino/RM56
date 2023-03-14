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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OVERLAY_OVERLAY_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OVERLAY_OVERLAY_COMPONENT_H

#include "core/components/stack/stack_component.h"

namespace OHOS::Ace {

class OverlayComponent : public StackComponent {
    DECLARE_ACE_TYPE(OverlayComponent, StackComponent);

public:
    explicit OverlayComponent(const std::list<RefPtr<Component>>& children)
        : StackComponent(Alignment::TOP_LEFT, StackFit::INHERIT, Overflow::OBSERVABLE, children)
    {}
    ~OverlayComponent() override = default;

    RefPtr<Element> CreateElement() override;

    RefPtr<RenderNode> CreateRenderNode() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_OVERLAY_OVERLAY_COMPONENT_H
