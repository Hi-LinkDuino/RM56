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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_COMPONENT_H

#include "core/components/common/properties/edge.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class ToolBarComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(ToolBarComponent, ComponentGroup);

public:
    explicit ToolBarComponent(const std::list<RefPtr<Component>>& children)
        : ComponentGroup(children) {}

    ~ToolBarComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_TOOL_BAR_COMPONENT_H
