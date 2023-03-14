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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_FOCUS_ANIMATION_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_FOCUS_ANIMATION_COMPONENT_H

#include "core/components/common/properties/radius.h"
#include "core/components/focus_animation/focus_animation_element.h"
#include "core/components/focus_animation/focus_animation_theme.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

class FocusAnimationComponent : public RenderComponent {
    DECLARE_ACE_TYPE(FocusAnimationComponent, RenderComponent);

public:
    explicit FocusAnimationComponent(bool isRoot = false) : isRoot_(isRoot) {}

    ~FocusAnimationComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<FocusAnimationElement>(isRoot_);
    }

    bool IsRoot() const
    {
        return isRoot_;
    }

private:
    bool isRoot_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_FOCUS_ANIMATION_FOCUS_ANIMATION_COMPONENT_H
