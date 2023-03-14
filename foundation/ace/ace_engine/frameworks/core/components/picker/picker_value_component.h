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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_VALUE_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_VALUE_COMPONENT_H

#include "core/components/picker/picker_option_component.h"
#include "core/components/picker/picker_value_element.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class PickerValueComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(PickerValueComponent, SoleChildComponent);

public:
    explicit PickerValueComponent(const PickerValueCallback& clickCallback)
    {
        clickCallback_ = clickCallback;
        option_ = AceType::MakeRefPtr<PickerOptionComponent>();
        option_->SetSelected(false);
        option_->SetAutoLayout(true);
        SetChild(option_);
    }

    ~PickerValueComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return AceType::MakeRefPtr<RenderPickerValue>(clickCallback_);
    }
    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<PickerValueElement>();
    }

    const std::string& GetText() const
    {
        return option_->GetText();
    }
    void SetText(const std::string& value)
    {
        option_->SetText(value);
    }

    void SetTheme(const RefPtr<PickerTheme>& value)
    {
        if (!value) {
            LOGE("input value is null.");
            return;
        }

        value->SetOptionStyle(true, false, value->GetOptionStyle(false, false));
        value->SetOptionDecoration(false, AceType::MakeRefPtr<Decoration>());
        option_->SetTheme(value);
        option_->SetTextDirection(GetTextDirection());
    }

private:
    RefPtr<PickerOptionComponent> option_;
    PickerValueCallback clickCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_VALUE_COMPONENT_H
