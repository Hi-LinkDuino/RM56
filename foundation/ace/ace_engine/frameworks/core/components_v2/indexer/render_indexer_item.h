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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_INDEXER_ITEM_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_INDEXER_ITEM_H

#include "base/utils/string_utils.h"
#include "core/animation/animator.h"
#include "core/animation/keyframe_animation.h"
#include "core/components/box/render_box.h"
#include "core/components/text/text_component.h"
#include "core/components/transform/render_transform.h"

namespace OHOS::Ace::V2 {
class RenderIndexerItem : public RenderNode {
    DECLARE_ACE_TYPE(RenderIndexerItem, RenderNode);

public:
    static RefPtr<RenderNode> Create();
    void Update(const RefPtr<Component>& component) override;
    void PerformLayout() override;
    void UpdateItemStyle();

    void MarkItemInAnimation(bool inAnimation = true)
    {
        isAnimating_ = inAnimation;
    }

    int32_t GetItemType() const
    {
        return itemType_;
    }

    bool IsItemPrimary() const
    {
        return isPrimary_;
    }

    const std::string GetSectionText() const
    {
        return StringUtils::Str16ToStr8(strText_);
    }

    int32_t GetSectionIndex() const
    {
        return sectionIndex_;
    }

    void SetSectionIndex(int32_t index)
    {
        sectionIndex_ = index;
    }

    bool GetClicked() const
    {
        return clicked_;
    }

    void SetFocused(bool getFocus)
    {
        focused_ = getFocus;
        UpdateItemStyle();
    }

    void SetClicked(bool clicked)
    {
        bool preClicked = clicked_;
        clicked_ = clicked;
        if (clicked_ != preClicked) {
            UpdateItemStyle();
        }
    }

    void SetRotate(double angle)
    {
        RefPtr<RenderTransform> rotate = AceType::DynamicCast<RenderTransform>(GetChildren().front());
        if (rotate) {
            rotateAngle_ = angle;
            rotate->ResetTransform();
            rotate->RotateZ(angle);
        }
    }

    const TextStyle& GetNormalTextStyle() const
    {
        return normalStyle_;
    }

    const TextStyle& GetActiveTextStyle() const
    {
        return activeStyle_;
    }

    Color GetSelectedBackgroundColor() const
    {
        return selectedBgColor_;
    }

    const Dimension& GetItemSize() const
    {
        return itemSize_;
    }

protected:
    bool MouseHoverTest(const Point& parentLocalPoint) override;
    void OnMouseHoverEnterTest() override;
    void OnMouseHoverExitTest() override;

private:
    void StartHoverAnimation(RefPtr<Animator> controller,
        RefPtr<KeyframeAnimation<Color>>& colorAnimation);
    void ResetController(RefPtr<Animator>& controller);
    void CreateColorAnimation(RefPtr<KeyframeAnimation<Color>>& colorAnimation, const Color& beginValue,
        const Color& endValue);

    bool clicked_ = false;
    bool focused_ = false;
    int32_t itemType_ = 0;
    int32_t sectionIndex_ = 0;
    RefPtr<TextComponent> textComponent_;
    RefPtr<Animator> controllerEnter_;
    RefPtr<Animator> controllerExit_;
    RefPtr<KeyframeAnimation<Color>> colorAnimationEnter_;
    RefPtr<KeyframeAnimation<Color>> colorAnimationExit_;
    TextStyle normalStyle_;
    TextStyle activeStyle_;
    Color selectedBgColor_;
    Dimension itemSize_;
    bool rotate_ = false;
    bool isPrimary_ = false;
    bool isAnimating_ = false;
    double rotateAngle_ = 0.0;
    std::u16string strText_;
}; // class RenderIndexerItem
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_INDEXER_RENDER_INDEXER_ITEM_H