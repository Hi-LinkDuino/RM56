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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_RENDER_STAGE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_RENDER_STAGE_H

#include "core/animation/animator.h"
#include "core/components/stack/render_stack.h"
#include "core/gestures/drag_recognizer.h"

namespace OHOS::Ace {

class RenderStage : public RenderStack {
    DECLARE_ACE_TYPE(RenderStage, RenderStack);

public:
    ~RenderStage() override = default;

    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    void SetForbidSwipeToRight(bool forbidSwipeToRight)
    {
        forbidSwipeToRight_ = forbidSwipeToRight;
    }

private:
    void WatchDragToBack();

    void HandleDragUpdate(double deltaX);

    void HandleDragStart();

    void HandleDragEnd();

    void GetControllers();

    RefPtr<Animator> controllerIn_;
    RefPtr<Animator> controllerOut_;
    RefPtr<DragRecognizer> dragDetector_;
    double dragOffsetX_ = 0.0;
    double tickTime_ = 0.0;
    bool isRightToLeft_ = false;
    bool forbidSwipeToRight_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STAGE_RENDER_STAGE_H
