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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_ELEMENT_H

#include "core/components/overlay/overlay_element.h"
#include "core/components/stage/stage_element.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class SemiModalElement : public SoleChildElement {
    DECLARE_ACE_TYPE(SemiModalElement, SoleChildElement);

public:
    void AnimateToFullWindow(int32_t duration);

    const Color& GetBackgroundColor() const;

    bool IsFullWindow() const;

    void SetFullWindow(bool fullWindow);

    void UpdateSystemBarHeight(double statusBar, double navigationBar);

    RefPtr<OverlayElement> GetOverlayElement() const;

    RefPtr<StageElement> GetStageElement() const;

    void AnimateToExitApp() const;

private:
    void UpdateStatusBarHeight(const RefPtr<Element>& column, double height);
    void UpdateNavigationBarHeight(const RefPtr<Element>& column, double height);
    bool fullWindow_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SEMI_MODAL_SEMI_MODAL_ELEMENT_H
