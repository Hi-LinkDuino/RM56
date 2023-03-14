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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_CHECKBOX_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_CHECKBOX_H

#include "core/animation/animator.h"
#include "core/animation/curve_animation.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/checkable/render_checkable.h"

namespace OHOS::Ace {

class RenderCheckbox : public RenderCheckable {
    DECLARE_ACE_TYPE(RenderCheckbox, RenderCheckable);

public:
    RenderCheckbox() = default;
    ~RenderCheckbox() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void HandleClick() override;
    bool UpdateItemValue(const bool itemValue);
    bool UpdateGroupValue(const CheckableStatus groupValue);
    void SetChecked(bool checked)
    {
        checked_ = checked;
    }

    bool GetCheckBoxValue()
    {
        if (component_) {
            return component_->GetValue();
        }
        return false;
    }

    const std::string& GetBelongGroup() const
    {
        if (component_) {
            return component_->GetBelongGroup();
        }
        return "";
    }

    const std::string& GetCheckboxName() const
    {
        if (component_) {
            return component_->GetCheckboxName();
        }
        return "";
    }

    const std::string& GetGroupName() const
    {
        if (component_) {
            return component_->GetGroupName();
        }
        return "";
    }

    const Color& GetSelectedColor() const
    {
        if (component_) {
            return component_->GetActiveColor();
        }
        return Color::TRANSPARENT;
    }

    void UpdateUIStatus() override
    {
        if (!(component_->GetGroupName().empty())) {
            auto value = component_->GetGroupValue();
            if (value == CheckableStatus::ALL) {
                uiStatus_ = UIStatus::SELECTED;
            } else if (value == CheckableStatus::PART) {
                uiStatus_ = UIStatus::PART;
            } else if (value == CheckableStatus::NONE) {
                uiStatus_ = UIStatus::UNSELECTED;
            }
        } else if (!(component_->GetCheckboxName().empty())) {
            bool isCheck = component_->GetValue();
            uiStatus_ = isCheck ? UIStatus::SELECTED :
                ((onFocus_ && needFocus_) ? UIStatus::FOCUS : UIStatus::UNSELECTED);
        } else {
            uiStatus_ = checked_ ? UIStatus::SELECTED : UIStatus::UNSELECTED;
        }
    }

protected:
    void UpdateAnimation();
    void OnAnimationStop();
    void UpdateCheckBoxShape(double value);
    void UpdateAccessibilityAttr();
    void UpdateGroupStatus();

    enum class SelectStatus {
        ALL = 0,
        PART,
        NONE,
    };

    // animation control
    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<double>> translate_;

    SelectStatus status_ = SelectStatus::ALL;
    SelectStatus lastStatus_ = SelectStatus::ALL;
    double shapeScale_ = 1.0;
    RefPtr<CheckboxComponent> component_;
    bool isGroup_ = false;
    bool selectAll_ = false;
    std::function<void(bool)> groupValueChangedListener_;
    std::function<void(const std::shared_ptr<BaseEventInfo>&)> onGroupChange_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_CHECKBOX_H
