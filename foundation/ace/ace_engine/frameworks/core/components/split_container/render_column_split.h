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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_RENDER_COLUMN_SPLIT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_RENDER_COLUMN_SPLIT_H

#include "core/pipeline/base/render_node.h"
#include "core/components/split_container/render_split_container.h"
#include "core/gestures/drag_recognizer.h"

namespace OHOS::Ace {

class RenderColumnSplit : public RenderSplitContainer {
    DECLARE_ACE_TYPE(RenderColumnSplit, RenderSplitContainer);

public:
    RenderColumnSplit() = default;
    ~RenderColumnSplit() override = default;

    static RefPtr<RenderNode> Create();
    void LayoutChildren() override;
    void HandleDragStart(const Offset& startPoint) override;
    void HandleDragUpdate(const Offset& currentPoint) override;
    void HandleDragEnd(const Offset& endPoint, double velocity) override;

private:
    Point localPoint_;
    double startY_ = 0.0;
    std::vector<Rect> splitRects_;
    std::vector<double> dragSplitOffset_;
    std::size_t dragedSplitIndex_ = -1;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_RENDER_COLUMN_SPLIT_H