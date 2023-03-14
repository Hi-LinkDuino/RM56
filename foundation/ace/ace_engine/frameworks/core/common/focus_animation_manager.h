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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FOCUS_ANIMATION_MANAGER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FOCUS_ANIMATION_MANAGER_H

#include <stack>

#include "base/geometry/rrect.h"
#include "base/memory/ace_type.h"
#include "core/components/common/properties/color.h"

namespace OHOS::Ace {

class Element;
class RenderFocusAnimation;
class RenderShadow;

class FocusAnimationManager : public virtual AceType {
    DECLARE_ACE_TYPE(FocusAnimationManager, AceType);

public:
    FocusAnimationManager() = default;
    ~FocusAnimationManager() override = default;

    void SetFocusAnimationProperties(
        const RRect& rrect, const Color& color, const Offset& offset, bool isIndented) const;
    void SetFocusAnimationProperties(
        const RRect& rrect, const Color& color, const Offset& offset, const Rect& clipRect) const;
    void SetAvailableRect(const Rect& paintRect);
    void CancelFocusAnimation() const;
    void PushFocusAnimationElement(const RefPtr<Element>& element);
    void PopFocusAnimationElement();
    void PopRootFocusAnimationElement();
    void PushShadow(const RefPtr<Element>& element);
    void PopShadow();
    void StartFocusAnimation() const;
    void StopFocusAnimation() const;
    void SetShadowProperties(const RRect& rrect, const Offset& offset);
    void SetShadowProperties(const RRect& rrect, const Offset& offset, const Rect& clipRect);
    void CancelShadow() const;
    void SetUseRoot(bool useRoot);
    void SetIsKeyEvent(bool isKeyEvent);
    RefPtr<RenderFocusAnimation> GetRenderFocusAnimation() const;

private:
    Rect availableRect_;
    bool useRoot_ = false;
    std::stack<WeakPtr<RenderFocusAnimation>> rootFocusAnimationStack_;
    std::stack<WeakPtr<RenderFocusAnimation>> focusAnimationStack_;
    std::stack<WeakPtr<RenderShadow>> shadowStack_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_FOCUS_ANIMATION_MANAGER_H
