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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WATCH_SLIDER_RENDER_WATCH_SLIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WATCH_SLIDER_RENDER_WATCH_SLIDER_H

#include <functional>

#include "core/components/common/rotation/rotation_node.h"
#include "core/components/watch_slider/watch_slider_component.h"
#include "core/gestures/click_recognizer.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderWatchSlider : public RenderNode, public RotationNode {
    DECLARE_ACE_TYPE(RenderWatchSlider, RenderNode, RotationNode);

public:
    static RefPtr<RenderNode> Create();

    void Update(const RefPtr<Component>& component) override;

    void PerformLayout() override;

    void Initialize();

    bool OnRotation(const RotationEvent& event) override;

    void UpdatePosition(const Offset& offset);

protected:
    Color backgroundColor_;
    Color selectColor_;
    std::string maxIconUrl_;
    std::string minIconUrl_;
    Rect topIconRect_;
    Rect bottomIconRect_;
    double value_ = 0.0;
    double max_ = 100.0;
    double min_ = 0.0;
    double step_ = 1.0;
    bool disable_ = false;
    bool isContinuous_ = false;

    RefPtr<RenderNode> topIcon_;
    RefPtr<RenderNode> bottomIcon_;
    Offset topIconPosition_;
    Offset bottomIconPosition_;

    double accumulatedValue_ = 0.0;

private:
    void HandleDragUpdate(double delta);
    void HandleDragEnd(double delta);
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;
    void HandleClick(const Offset& clickPosition);

    void OnPaintFinish() override;
    void UpdateAccessibilityAttr();
    void InitAccessibilityEventListener();

    std::function<void(const std::string&)> onMoveEnd_;
    std::function<void(const std::string&)> onMoving_;

    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<ClickRecognizer> clickDetector_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_WATCH_SLIDER_RENDER_WATCH_SLIDER_H
