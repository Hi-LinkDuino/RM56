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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_EDGE_EFFECT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_EDGE_EFFECT_H

#include <functional>

#include "base/memory/ace_type.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/color.h"
#include "core/components/scroll/scrollable.h"

namespace OHOS::Ace {

using EdgeCallback = std::function<double()>;

class ScrollEdgeEffect : public AceType {
    DECLARE_ACE_TYPE(ScrollEdgeEffect, AceType);

public:
    explicit ScrollEdgeEffect(EdgeEffect edgeEffect)
    {
        edgeEffect_ = edgeEffect;
    }

    ~ScrollEdgeEffect() override = default;

    EdgeEffect GetEdgeEffect() const
    {
        return edgeEffect_;
    }

    // support layout out of boundary fill with blank
    bool IsRestrictBoundary() const
    {
        return edgeEffect_ != EdgeEffect::SPRING;
    }

    bool IsFadeEffect() const
    {
        return edgeEffect_ == EdgeEffect::FADE;
    }

    bool IsSpringEffect() const
    {
        return edgeEffect_ == EdgeEffect::SPRING;
    }

    void SetScrollable(const RefPtr<Scrollable>& scrollable)
    {
        scrollable_ = scrollable;
    }

    void SetScrollNode(const WeakPtr<RenderNode>& scroll)
    {
        scroll_ = scroll;
    }

    void SetCurrentPositionCallback(const EdgeCallback& currentPositionCallback)
    {
        currentPositionCallback_ = currentPositionCallback;
    }

    void SetLeadingCallback(const EdgeCallback& leadingCallback)
    {
        leadingCallback_ = leadingCallback;
    }

    void SetTrailingCallback(const EdgeCallback& trainingCallback)
    {
        trailingCallback_ = trainingCallback;
    }

    void SetInitLeadingCallback(const EdgeCallback& initLeadingCallback)
    {
        initLeadingCallback_ = initLeadingCallback;
    }

    void SetInitTrailingCallback(const EdgeCallback& initTrainingCallback)
    {
        initTrailingCallback_ = initTrainingCallback;
    }

    // common use
    virtual void InitialEdgeEffect() {}

    // fade effect use
    virtual double CalculateOverScroll(double oldPosition, bool isReachMax)
    {
        return 0.0;
    }

    virtual void HandleOverScroll(Axis axis, double overScroll, const Size& viewPort) {}

    virtual void Paint(RenderContext& context, const Size& viewPort, const Offset& offset) {}

    // spring effect use
    virtual void RegisterSpringCallback() {}

    virtual void ProcessScrollOver(double velocity) {}

protected:
    RefPtr<Scrollable> scrollable_;
    WeakPtr<RenderNode> scroll_;
    EdgeCallback currentPositionCallback_; // get current position
    EdgeCallback leadingCallback_; // get stable min extent
    EdgeCallback trailingCallback_; // get stable max extent
    EdgeCallback initLeadingCallback_; // get current min extent
    EdgeCallback initTrailingCallback_; // get current max extent

private:
    EdgeEffect edgeEffect_ = EdgeEffect::SPRING;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_EDGE_EFFECT_H
