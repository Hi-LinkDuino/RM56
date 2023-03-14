/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CONTAINER_MODAL_CONTAINER_MODAL_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CONTAINER_MODAL_CONTAINER_MODAL_ELEMENT_H

#include "core/components/container_modal/container_modal_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/overlay/overlay_element.h"
#include "core/components/stage/stage_element.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class ContainerModalElement : public SoleChildElement {
    DECLARE_ACE_TYPE(ContainerModalElement, SoleChildElement);

public:
    RefPtr<OverlayElement> GetOverlayElement() const;
    RefPtr<StageElement> GetStageElement() const;
    void ShowTitle(bool isShow);
    void BlurWindow(bool isBlur);
    void WindowFocus(bool isFocus);
    void Update() override;
    void PerformBuild() override;

private:
    RefPtr<StackElement> GetStackElement() const;
    bool CanShowFloatingTitle();
    bool CanHideFloatingTitle();
    void ChangeFloatingTitleIcon(bool isFocus = true);
    void ChangeTitleIcon(bool isFocus = true);
    WindowMode windowMode_;
    RefPtr<Animator> controller_;
    RefPtr<RenderDisplay> floatingTitleDisplay_;
    RefPtr<BoxElement> floatingTitleBox_;
    RefPtr<BoxElement> titleBox_;
    RefPtr<ContainerModalComponent> containerModalComponent_;
    float density_ = 1.0f;
    float moveX_ = 0.0f;
    float moveY_ = 0.0f;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CONTAINER_MODAL_CONTAINER_MODAL_ELEMENT_H
