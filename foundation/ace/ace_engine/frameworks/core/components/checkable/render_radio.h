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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_RADIO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_RADIO_H

#include <functional>

#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/checkable/checkable_component.h"
#include "core/components/checkable/render_checkable.h"

namespace OHOS::Ace {

class RenderRadio : public RenderCheckable {
    DECLARE_ACE_TYPE(RenderRadio, RenderCheckable);

public:
    RenderRadio() = default;
    ~RenderRadio() override = default;

    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    bool UpdateGroupValue(const std::string& groupValue);

    RefPtr<RadioComponent<std::string>> GetRadioComponent() const
    {
        return component_;
    }

protected:
    double innerCircleRadius_ = 0.0;
    double outCircleRadius_ = 0.0;
    double radioInnerSizeRatio_ = 0.0;
    std::string radioValue_;
    std::string groupValue_;
    std::function<void(std::string)> groupValueChangedListener_;

    void UpdateAnimation(bool isOn);
    void OffAnimationEnd();
    void OnAnimationStart();
    void HandleClick() override;
    void UpdateAccessibilityAttr() const;

    RefPtr<Animator> onController_;
    RefPtr<Animator> offController_;

    double totalScale_ = 1.0;
    double pointScale_ = 1.0;
    bool isClicked_ = false;
    RefPtr<RadioComponent<std::string>> component_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHECKABLE_RENDER_RADIO_H
