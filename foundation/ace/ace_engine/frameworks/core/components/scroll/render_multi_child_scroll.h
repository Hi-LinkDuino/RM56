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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_RENDER_MULTI_CHILD_SCROLL_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_RENDER_MULTI_CHILD_SCROLL_H

#include "core/common/vibrator/vibrator.h"
#include "core/components/common/rotation/rotation_node.h"
#include "core/components/list/list_component.h"
#include "core/components/list/render_list.h"
#include "core/components/scroll/render_scroll.h"

namespace OHOS::Ace {

class RenderMultiChildScroll : public RenderScroll, public RotationNode {
    DECLARE_ACE_TYPE(RenderMultiChildScroll, RenderScroll, RotationNode);

public:
    static RefPtr<RenderNode> Create();

    void AddChild(const RefPtr<RenderList>& child);

    void JumpToIndex(int32_t index, int32_t source = SCROLL_FROM_JUMP) override;

    void JumpToPosition(double position, int32_t source = SCROLL_FROM_JUMP) override;

    // notify start position in global main axis
    void NotifyDragStart(double startPosition) override;

    // notify drag offset in global main axis
    void NotifyDragUpdate(double dragOffset, int32_t source) override;

    void ProcessScrollOverCallback(double velocity) override;

    void MoveItemToViewPort(double position, double size, double effectOffset = 0.0);

    void ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth) override;

    bool ScrollPage(bool reverse, bool smooth, const std::function<void()>& onFinish = nullptr) override;

    void HandleRotate(double rotateValue, bool isVertical) override;

    void SetCacheExtent(double cacheExtent)
    {
        cacheExtent_ = cacheExtent;
    }

    void RefreshOffset(double offset)
    {
        (axis_ == Axis::HORIZONTAL) ? currentOffset_.SetX(offset) : currentOffset_.SetY(offset);
    }

    bool CalculateMainScrollExtent();

    bool OnRotation(const RotationEvent& event) override;

    void OnAttachContext() override
    {
        auto childList = GetChildren();
        for (const auto& child : childList) {
            if (child) {
                child->Attach(context_);
            }
        }
    }

    bool IsOutOfBottomBoundary() override;

    bool IsOutOfTopBoundary() override;

    double GetMainScrollExtent() const override;

    double GetFixPositionOnWatch(double final, double current) override;

    void SetScrollVibrate(bool scrollVibrate)
    {
        scrollVibrate_ = scrollVibrate;
    }

    bool GetScrollVibrate()
    {
        return scrollVibrate_;
    }

    bool IsUseOnly() override
    {
        return true;
    }

protected:
    void OnPredictLayout(int64_t deadline) override;
    void PerformLayout() override;
    void Update(const RefPtr<Component>& component) override;
    bool ReachMaxCount() const override;
    bool IsReadyToJump() const;

    Dimension gradientWidth_;
    Color backgroundColor_;
    double offsetBeforeLayout_ = 0.0;

private:
    LayoutParam MakeInnerLayoutParam() const;
    void UpdateGradient(const RefPtr<ListComponent>& listComponent);
    void UpdateEdgeEffect(const RefPtr<ListComponent>& listComponent);
    void ApplyGradientColor();
    void LayoutChild();
    void LayoutChild(const RefPtr<RenderNode>& child, const Offset& position, double start, double end);
    bool LayoutChild(const RefPtr<RenderNode>& curChild, int32_t curIndex, const RefPtr<RenderNode>& lastChild);
    double CalculateBeginPositionInViewPort(double position, double size, double effectOffset = 0.0);
    bool ScrollToPosition(double position, int32_t source, bool smooth);
    void ProcessScrollExtent();
    void ExtendViewPort();
    bool HandleCrashTop() override;
    bool HandleCrashBottom() override;

    double cacheExtent_ = 300.0;
    int32_t currentIndex_ = 0;
    int32_t initialIndex_ = 0;
    int32_t effectIndex_ = 0;
    double initialOffset_ = 0.0;
    double effectOffset_ = 0.0;
    double accumulatedRotationValue_ = 0.0;
    bool scrollVibrate_ = true;
    bool rotationVibrate_ = false;

    RefPtr<Animator> animateController_;
    RefPtr<Vibrator> vibrator_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_RENDER_MULTI_CHILD_SCROLL_H
