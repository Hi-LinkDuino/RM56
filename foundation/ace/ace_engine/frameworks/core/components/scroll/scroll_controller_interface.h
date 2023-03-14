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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_CONTROLLER_INTERFACE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_CONTROLLER_INTERFACE_H

#include "base/geometry/axis.h"
#include "base/geometry/dimension.h"
#include "base/geometry/offset.h"
#include "base/memory/ace_type.h"
#include "core/animation/curve.h"
#include "core/components/common/layout/constants.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class ScrollEdgeType : size_t {
    SCROLL_TOP = 0,
    SCROLL_BOTTOM,
    SCROLL_LEFT,
    SCROLL_RIGHT,
    SCROLL_NONE,
};

class ACE_EXPORT ScrollController : public AceType {
    DECLARE_ACE_TYPE(ScrollController, AceType);

public:
    ScrollController() = default;
    ~ScrollController() override = default;

    virtual void JumpTo(int32_t index, int32_t source = 3) {}

    virtual Axis GetScrollDirection() const
    {
        return Axis::NONE;
    }

    virtual bool AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve)
    {
        return true;
    }
    virtual void ScrollBy(double pixelX, double pixelY, bool smooth) {}
    virtual void ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth) {}
    virtual void ScrollPage(bool reverse, bool smooth) {}
    virtual Offset GetCurrentOffset() const
    {
        return Offset();
    }

    void SetScrollNode(const WeakPtr<RenderNode>& scroll)
    {
        scroll_ = scroll;
    }

protected:
    WeakPtr<RenderNode> scroll_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_SCROLL_CONTROLLER_INTERFACE_H
