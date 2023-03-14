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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_MODAL_DIALOG_MODAL_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_MODAL_DIALOG_MODAL_ELEMENT_H

#include "core/components/overlay/overlay_element.h"
#include "core/components/stage/stage_element.h"
#include "core/components/tween/tween_element.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class DialogModalElement : public SoleChildElement {
    DECLARE_ACE_TYPE(DialogModalElement, SoleChildElement);

public:
    DialogModalElement() = default;
    ~DialogModalElement() override = default;

    RefPtr<OverlayElement> GetOverlayElement() const;

    RefPtr<StageElement> GetStageElement() const;

    void UpdateSystemBarHeight(double statusBar, double navigationBar);

    void AnimateToEnterApp();

    void AnimateToExitApp();

    void RegisterTransitionListener();

    void PerformBuild() override;

private:
    void CreateOriginAnimation();

    TweenOption option_;
    RefPtr<Animator> controller_;
    BaseId::IdType stopCallbackId_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DIALOG_MODAL_DIALOG_MODAL_ELEMENT_H
