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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_ELEMENT_H

#include "base/geometry/axis.h"
#include "base/utils/macros.h"
#include "core/focus/focus_node.h"
#include "core/pipeline/base/component_group_element.h"
#include "core/pipeline/base/render_element.h"

namespace OHOS::Ace {

class ACE_EXPORT SwiperElement : public ComponentGroupElement, public FocusGroup {
    DECLARE_ACE_TYPE(SwiperElement, ComponentGroupElement, FocusGroup);

public:
    bool IsFocusable() const override;
    void PerformBuild() override;

protected:
    void OnFocus() override;
    void OnBlur() override;
    bool RequestNextFocus(bool vertical, bool reverse, const Rect& rect) override;
    bool OnKeyEvent(const KeyEvent& keyEvent) override;

private:
    void registerCallBack();
    void RequestChildFocus(int32_t index);
    void HandleIndicatorFocus(bool isFocus);
    bool RequestIndicatorFocus();
    bool RequestCurrentItemFocus();

    RefPtr<FocusNode> indicatorFocusNode_;
    bool showIndicator_ = true;
    Axis axis_ = Axis::HORIZONTAL;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SWIPER_SWIPER_ELEMENT_H
