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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_CONTROLLER_H

#include "core/animation/curve.h"
#include "core/pipeline/base/constants.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class GridController : public AceType {
    DECLARE_ACE_TYPE(GridController, AceType);

public:
    GridController() = default;
    ~GridController() override = default;

    void SetInitialIndex(int32_t index)
    {
        if (index >= 0) {
            initialIndex_ = index;
        }
    }

    int32_t GetInitialIndex() const
    {
        return initialIndex_;
    }

    int32_t GetCurrentIndex() const;
    double GetCurrentPosition() const;
    void JumpTo(double position);
    void AnimateTo(double position, float duration, const RefPtr<Curve>& curve);
    void JumpTo(int32_t index);
    void AnimateTo(int32_t index, float duration, const RefPtr<Curve>& curve);
    void ScrollBy(int32_t pixelX, int32_t pixelY, bool smooth);

    // Will be called by RenderGrid when update.
    void SetGridNode(const WeakPtr<RenderNode>& grid)
    {
        grid_ = grid;
    }

private:
    int32_t initialIndex_ = DEFAULT_INITIAL_ITEM;
    WeakPtr<RenderNode> grid_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_GRID_GRID_CONTROLLER_H
