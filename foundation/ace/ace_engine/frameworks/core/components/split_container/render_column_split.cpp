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

#include "core/components/split_container/render_column_split.h"

#include "core/components/flex/render_flex.h"
#include "core/pipeline/base/position_layout_utils.h"

namespace OHOS::Ace {

namespace {

constexpr double DEFAULT_SPLIT_RESPOND_WIDTH = 25.0;
constexpr size_t DEFAULT_DRAG_INDEX = -1;

} // namespace

void RenderColumnSplit::LayoutChildren()
{
    splitRects_.clear();
    if (dragSplitOffset_.size() == 0) {
        dragSplitOffset_ = std::vector<double>(GetChildren().size(), 0.0);
    }
    Size maxSize = GetLayoutParam().GetMaxSize();
    layoutWidth_ = maxSize.Width();
    layoutHeight_ = 0.0;
    size_t index = 0;
    double childOffsetY = 0.0;
    for (const auto& item : GetChildren()) {
        Offset offset = Offset(0, childOffsetY);
        item->Layout(GetLayoutParam());
        item->SetPosition(offset);

        // calc drag offset
        childOffsetY += item->GetLayoutSize().Height();
        layoutHeight_ += item->GetLayoutSize().Height();
        if (dragSplitOffset_[index] > 0) {
            childOffsetY += dragSplitOffset_[index];
        }
        double posY = childOffsetY > DEFAULT_SPLIT_RESPOND_WIDTH ? (childOffsetY - DEFAULT_SPLIT_RESPOND_WIDTH) : 0.0;
        splitRects_.push_back(Rect(0, posY, maxSize.Width(), 2 * DEFAULT_SPLIT_RESPOND_WIDTH + DEFAULT_SPLIT_HEIGHT));
        childOffsetY += DEFAULT_SPLIT_HEIGHT;
        layoutHeight_ += DEFAULT_SPLIT_HEIGHT;
        index++;
    }
    layoutHeight_ -= DEFAULT_SPLIT_HEIGHT;
}

void RenderColumnSplit::HandleDragStart(const Offset& startPoint)
{
    dragedSplitIndex_ = DEFAULT_DRAG_INDEX;
    for (std::size_t i = 0; i < splitRects_.size(); i++) {
        if (splitRects_[i].IsInRegion(Point(startPoint.GetX(), startPoint.GetY()))) {
            dragedSplitIndex_ = i;
            LOGD("dragedSplitIndex_ = %zu", dragedSplitIndex_);
            break;
        }
    }
    startY_ = startPoint.GetY();
}

void RenderColumnSplit::HandleDragUpdate(const Offset& currentPoint)
{
    if (dragedSplitIndex_ == DEFAULT_DRAG_INDEX) {
        return;
    }
    if (!GetPaintRect().IsInRegion(Point(currentPoint.GetX(), currentPoint.GetY()))) {
        return;
    }
    dragSplitOffset_[dragedSplitIndex_] += (currentPoint.GetY() - startY_);
    startY_ = currentPoint.GetY();
    MarkNeedLayout();
}

void RenderColumnSplit::HandleDragEnd(const Offset& endPoint, double velocity)
{
    startY_ = 0.0;
}

} // namespace OHOS::Ace