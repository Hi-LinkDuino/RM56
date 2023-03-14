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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_SLIDER_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_SLIDER_ELEMENT_H

#include "core/components/slider/render_slider.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class SliderElement : public RenderElement, public FocusNode {
    DECLARE_ACE_TYPE(SliderElement, RenderElement, FocusNode);

public:
    bool OnKeyEvent(const KeyEvent& keyEvent) override
    {
        if (keyEvent.action != KeyAction::UP) {
            return false;
        }
        auto renderSlider = AceType::DynamicCast<RenderSlider>(renderNode_);
        return renderSlider && renderSlider->HandleFocusEvent(keyEvent);
    }

    void OnFocus() override
    {
        slider_ = AceType::DynamicCast<RenderSlider>(renderNode_);
        if (!slider_) {
            LOGE("Render slider create error!");
            return;
        }
        slider_->ChangeStatus(RenderStatus::FOCUS);
        slider_->SetFocus(true);
    }

    void OnBlur() override
    {
        if (!slider_) {
            return;
        }
        slider_->ChangeStatus(RenderStatus::BLUR);
        slider_->SetFocus(false);
    }

private:
    RefPtr<RenderSlider> slider_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SLIDER_SLIDER_ELEMENT_H
