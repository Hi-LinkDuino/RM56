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

#include "core/components/stepper/render_stepper_item.h"

#include "core/components/stepper/stepper_item_component.h"
#include "core/event/ace_event_helper.h"

namespace OHOS::Ace {

void RenderStepperItem::Update(const RefPtr<Component>& component)
{
    auto stepperItem = AceType::DynamicCast<StepperItemComponent>(component);
    if (!stepperItem) {
        LOGW("stepper item component is null");
        return;
    }
    label_ = stepperItem->GetLabel();
    auto context = context_.Upgrade();
    if (!context) {
        LOGE("context is null");
        return;
    }
    appearEvent_ = AceAsyncEvent<void(const std::string&)>::Create(stepperItem->GetAppearEventId(), context_);
    disappearEvent_ = AceAsyncEvent<void(const std::string&)>::Create(stepperItem->GetDisappearEventId(), context_);
    MarkNeedLayout();
}

void RenderStepperItem::PerformLayout()
{
    if (!GetChildren().empty()) {
        auto child = GetChildren().front();
        child->Layout(GetLayoutParam());
        auto size = GetLayoutParam().Constrain(Size(child->GetLayoutSize().Width(), child->GetLayoutSize().Height()));
        SetLayoutSize(size);
    }
}

void RenderStepperItem::FireAppearEvent() const
{
    if (appearEvent_) {
        std::string param = std::string("\"appear\",null");
        appearEvent_(param);
    }
}

void RenderStepperItem::FireDisappearEvent() const
{
    if (disappearEvent_) {
        std::string param = std::string("\"disappear\",null");
        disappearEvent_(param);
    }
}

} // namespace OHOS::Ace
