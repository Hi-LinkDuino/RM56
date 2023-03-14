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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_ELEMENT_H

#include "core/common/container.h"
#include "core/components/text/text_component.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class SelectElement : public SoleChildElement, public FocusNode {
    DECLARE_ACE_TYPE(SelectElement, SoleChildElement, FocusNode);

public:
    void PerformBuild() override;

    bool CanUpdate(const RefPtr<Component>& newComponent) override
    {
        auto context = context_.Upgrade();

        if (context && context->GetIsDeclarative()) {
            return true;
        } else {
            return false;
        }
    }

    void FlushRefresh();

protected:
    void OnClick() override;
    void OnFocus() override;
    void OnBlur() override;

private:
    void HandleClickedEvent();
    void HandleTouchEvent(bool isDown);
    void HandleOptionClickedEvent(std::size_t index);
    void HandleOptionModifiedEvent(std::size_t index);
    Corner GetCorner() const;
    void SetBackgroundColor(bool isDown, const Color& color);
    void CreateColorAnimation(RefPtr<KeyframeAnimation<Color>>& animation,
        const Color& from, const Color& to, bool isDown);
    void PlayEventEffectAnimation(bool isDown, const Color& endColor);
    RefPtr<RenderNode> GetRenderText() const;
    RefPtr<RenderNode> GetRenderBox() const;

    RefPtr<Component> dataComponent_;
    std::function<void(const std::string&)> onChangeCallback_;
    Dimension normalPadding_;
    Color eventEffectColor_ = Color::TRANSPARENT;
    RefPtr<Animator> eventEffectController_;
    std::function<void(std::size_t, std::string)> onSelected_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SELECT_SELECT_ELEMENT_H
