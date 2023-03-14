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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_RENDER_STEPPER_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_RENDER_STEPPER_ITEM_H

#include <functional>

#include "base/memory/ace_type.h"
#include "core/components/stepper/stepper_item_component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

using OnEventFunc = std::function<void(const std::string&)>;

class RenderStepperItem : public RenderNode {
    DECLARE_ACE_TYPE(RenderStepperItem, RenderNode)

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void FireAppearEvent() const;
    void FireDisappearEvent() const;
    const StepperLabels& GetLabel() const
    {
        return label_;
    }

private:
    OnEventFunc appearEvent_;
    OnEventFunc disappearEvent_;
    StepperLabels label_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_RENDER_STEPPER_ITEM_H
