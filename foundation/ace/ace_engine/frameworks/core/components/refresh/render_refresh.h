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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_RENDER_REFRESH_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_RENDER_REFRESH_H

#include "base/memory/ace_type.h"
#include "base/utils/system_properties.h"
#include "core/animation/animation.h"
#include "core/animation/animator.h"
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/display/render_display.h"
#include "core/components/progress/loading_progress_component.h"
#include "core/components/progress/render_loading_progress.h"
#include "core/components/refresh/refresh_component.h"
#include "core/components/refresh/refresh_controller.h"
#include "core/components/scroll/scrollable.h"
#include "core/components/text/render_text.h"
#include "core/components/text/text_component.h"
#include "core/gestures/drag_recognizer.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class RenderRefresh : public RenderNode {
    DECLARE_ACE_TYPE(RenderRefresh, RenderNode)

public:
    RenderRefresh();
    ~RenderRefresh() override = default;

    static RefPtr<RenderNode> Create();
    void UpdateTouchRect() override;
    void Update(const RefPtr<Component>& component) override;
    void OnHiddenChanged(bool hidden) override;
    void PerformLayout() override;
    void UpdateScrollableOffset(double delta);
    void HandleDragUpdate(double delta);
    void HandleDragEnd();
    void HandleDragCancel();

    RefreshStatus GetStatus() const
    {
        return refreshStatus_;
    }

    void SetRefreshStatus(bool refreshing)
    {
        refreshing_ = refreshing;
    }

    bool GetRefreshing() const
    {
        return refreshing_;
    }

    Dimension GetInspectorOffset() const
    {
        return inspectorOffset_;
    }

    double GetFriction() const
    {
        // Percent
        return frictionRatio_ * 100.0;
    }
protected:
    void OnTouchTestHit(
        const Offset& coordinateOffset, const TouchRestrict& touchRestrict, TouchTestResult& result) override;

    const RefPtr<RenderBox>& GetTimeBox() const
    {
        return timeBox_;
    }

    const RefPtr<RenderDisplay>& GetDisplay() const
    {
        return display_;
    }

    bool IsShowLastTime() const
    {
        return showLastTime_;
    }

    bool IsWatch() const
    {
        return SystemProperties::GetDeviceType() == DeviceType::WATCH;
    }

private:
    void CalcLoadingParams(const RefPtr<Component>& component);
    void Initialize();
    void InitAccessibilityEventListener();

    void UpdateScrollOffset(double value);
    void FireRefreshEvent() const;
    void FirePullDownEvent(const std::string& state) const;
    void StartAnimation(double start, double end, bool isFinished);
    void HandleStopListener(bool isFinished);
    RefreshStatus GetNextStatus();

    double MaxScrollableHeight() const;
    double GetFriction(double percentage) const;
    double GetOffset(double offset) const;
    double GetLoadingDiameter() const;
    double GetOpacity() const;
    Offset GetShowTimeOffset() const;
    Offset GetLoadingOffset() const;
    std::string GetFormatDateTime();

    Offset scrollableOffset_;
    RefPtr<DragRecognizer> dragDetector_;
    RefPtr<Animation<double>> translate_;
    RefPtr<Animator> animator_;

    // onRefresh event
    std::function<void(const std::string&)> refreshEvent_;

    // OnPullDownStartEvent
    std::function<void(const std::string&)> pullDownEvent_;

    std::function<void(int)> onStateChange_;
    std::function<void(void)> onRefreshing_;

    bool showLastTime_ = false;
    bool refreshing_ = false;
    bool isInitialized_ = false;
    bool isRefresh_ = false;
    bool isUseOffset_ = false; // Whether use the indicator offset or default
    RefreshType refreshType_ = RefreshType::AUTO;
    Color progressColor_;
    Color backgroundColor_;

    RefPtr<RefreshController> refreshController_;
    RefreshStatus refreshStatus_ = RefreshStatus::INACTIVE;

    // Used for loading
    RefPtr<LoadingProgressComponent> loadingComponent_;
    RefPtr<BoxComponent> loadingBoxComponent_;
    RefPtr<BoxComponent> loadingBackgroundBoxComponent_;

    // This is the progress loading
    RefPtr<RenderLoadingProgress> loading_;

    // This is used for position
    RefPtr<RenderBox> loadingBox_;

    // This is used for progress background include circle and color
    RefPtr<Decoration> decoration_;
    RefPtr<RenderBox> loadingBackgroundBox_;
    RefPtr<RenderNode> columnChild_;

    // Used for loading time
    RefPtr<TextComponent> timeComponent_;
    RefPtr<BoxComponent> timeBoxComponent_;
    RefPtr<DisplayComponent> displayComponent_;
    RefPtr<RenderText> time_;
    RefPtr<RenderBox> timeBox_;
    RefPtr<RenderDisplay> display_;

    // Used for different status
    double triggerRefreshDistance_ = 0.0;
    double triggerLoadingDistance_ = 0.0;
    double triggerShowTimeDistance_ = 0.0;
    double loadingDiameter_ = 0.0;
    double maxScrollOffset_ = 0.0;
    double indicatorOffset_ = 0.0;
    double frictionRatio_ = 0.0;
    double timeDistance_ = 0.0;
    double timeOffset_ = 0.0;
    Dimension inspectorOffset_;

    // Use for update loading size when screen size changed.
    double scale_ = 0.0;
    WeakPtr<Component> refreshComponent_;

    std::string timeText_;
    std::string lastTimeText_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_RENDER_REFRESH_H
