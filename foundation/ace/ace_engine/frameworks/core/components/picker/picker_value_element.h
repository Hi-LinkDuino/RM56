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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_VALUE_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_VALUE_ELEMENT_H

#include "core/components/box/render_box.h"
#include "core/components/picker/render_picker_value.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class PickerValueElement : public SoleChildElement, public FocusNode {
    DECLARE_ACE_TYPE(PickerValueElement, SoleChildElement, FocusNode);

public:
    void PerformBuild() override
    {
        SetFocusable(true);
        SoleChildElement::PerformBuild();
        HandleSelfFocus();
    }

    void Update() override
    {
        customComponent_ = component_;
        SoleChildElement::Update();
    }

    bool CanUpdate(const RefPtr<Component>& newComponent) override
    {
        return (newComponent == customComponent_) && Element::CanUpdate(newComponent);
    }

protected:
    void OnClick() override;

    void OnFocus() override
    {
        HandleSelfFocus(true);
    }

    void OnBlur() override
    {
        HandleSelfFocus(false);
    }

private:
    RefPtr<RenderBox> GetRenderBox() const;

    void HandleSelfFocus(bool focus);

    void HandleSelfFocus();

    WeakPtr<Component> customComponent_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_PICKER_PICKER_VALUE_ELEMENT_H
