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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_CUSTOM_PAINT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_CUSTOM_PAINT_ELEMENT_H

#include "core/components/custom_paint/render_custom_paint.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class CustomPaintElement : public RenderElement {
    DECLARE_ACE_TYPE(CustomPaintElement, RenderElement);

public:
    CustomPaintElement() = default;
    ~CustomPaintElement() override = default;

    void Update() override
    {
        std::list<TaskFunc> tasks;
        auto paint = AceType::DynamicCast<RenderCustomPaint>(renderNode_);
        if (paint && paint->HasTask()) {
            tasks = paint->GetTasks();
        }
        customComponent_ = component_;
        RenderElement::Update();
        if (paint) {
            paint->SetTasks(tasks);
        }
    }

    bool CanUpdate(const RefPtr<Component>& newComponent) override
    {
        const auto context = renderNode_->GetContext().Upgrade();
        if (context && context->GetIsDeclarative()) {
            return Element::CanUpdate(newComponent);
        }
        return (newComponent == customComponent_) && Element::CanUpdate(newComponent);
    }

private:
    WeakPtr<Component> customComponent_;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CUSTOM_PAINT_CUSTOM_PAINT_ELEMENT_H
