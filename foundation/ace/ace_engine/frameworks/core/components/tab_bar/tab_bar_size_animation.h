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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_SIZE_ANIMATION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_SIZE_ANIMATION_H

#include "base/memory/ace_type.h"
#include "core/animation/curve_animation.h"
#include "core/components/text/render_text.h"

namespace OHOS::Ace {

class TabBarSizeAnimation : public virtual AceType {
public:
    void Initialize(const WeakPtr<PipelineContext>& context);
    void Start(const WeakPtr<RenderNode>& tabBar, int32_t from, int32_t to);

private:
    struct ItemAnimationProp {
        constexpr ItemAnimationProp() = default;
        constexpr ItemAnimationProp(Dimension fontSize, double opacity) : fontSize(fontSize), opacity(opacity) {}
        constexpr ItemAnimationProp operator+(const ItemAnimationProp& prop) const
        {
            return ItemAnimationProp(fontSize + prop.fontSize, opacity + prop.opacity);
        }
        constexpr ItemAnimationProp operator-(const ItemAnimationProp& prop) const
        {
            return ItemAnimationProp(fontSize - prop.fontSize, opacity - prop.opacity);
        }
        constexpr ItemAnimationProp operator*(double value) const
        {
            return ItemAnimationProp(fontSize * value, opacity * value);
        }

        Dimension fontSize;
        double opacity = 0;
    };

    void ChangeItemProp(const RefPtr<RenderText>& layoutCallback, const ItemAnimationProp& animationProp);

    // animation control
    RefPtr<Animator> controller_;
    RefPtr<CurveAnimation<ItemAnimationProp>> onFocusTranslate_;
    RefPtr<CurveAnimation<ItemAnimationProp>> onBlurTranslate_;

    RefPtr<RenderText> onFocusItemText_;
    RefPtr<RenderText> onBlurItemText_;

    std::function<void()> layoutCallback_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_SIZE_ANIMATION_H
