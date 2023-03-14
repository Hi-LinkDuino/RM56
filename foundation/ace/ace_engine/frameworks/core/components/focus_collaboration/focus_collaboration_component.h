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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_COLLABORATION_FOCUS_COLLABORATION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_COLLABORATION_FOCUS_COLLABORATION_COMPONENT_H

#include "core/components/focus_collaboration/focus_collaboration_element.h"
#include "core/components/focus_collaboration/render_focus_collaboration.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class FocusCollaborationComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(FocusCollaborationComponent, ComponentGroup);

public:
    explicit FocusCollaborationComponent(bool isRoot = false) : isRoot_(isRoot) {}
    ~FocusCollaborationComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return RenderFocusCollaboration::Create();
    }

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<FocusCollaborationElement>(isRoot_);
    }

private:
    bool isRoot_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_COLLABORATION_FOCUS_COLLABORATION_COMPONENT_H
