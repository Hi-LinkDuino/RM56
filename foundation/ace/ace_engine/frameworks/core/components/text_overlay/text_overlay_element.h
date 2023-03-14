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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_TEXT_OVERLAY_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_TEXT_OVERLAY_ELEMENT_H

#include "core/components/box/box_component.h"
#include "core/components/text_overlay/text_overlay_component.h"
#include "core/components/tween/tween_element.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class TextOverlayElement : public SoleChildElement, public FocusGroup {
    DECLARE_ACE_TYPE(TextOverlayElement, SoleChildElement, FocusGroup)

public:
    void PerformBuild() override;
    void OnFocus() override;
    void OnBlur() override;

    void RebuildChild(bool isSingleHandle, bool hasToolBar, bool hasMenu, bool hasIcon, bool hasAnimation,
        bool buildToolBarOnly = false);

private:
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;

    void OnStartAnimation(
        const TweenOption& outerOption, const TweenOption& innerOption, bool isSingleHandle, bool isIn);
    // Init outer and inner tween element.
    void InitTween(const RefPtr<Element>& element);
    void AddListener(bool isSingleHandle, bool isIn);

    bool hasFocusAnimation_ = false;
    RefPtr<TextOverlayComponent> overlayComponent_;
    RefPtr<TweenElement> outerTween_;
    RefPtr<TweenElement> innerTween_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEXT_OVERLAY_TEXT_OVERLAY_ELEMENT_H
