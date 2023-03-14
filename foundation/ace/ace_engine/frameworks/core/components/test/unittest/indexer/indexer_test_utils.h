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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_INDEXER_INDEXER_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_INDEXER_INDEXER_TEST_UTILS_H

#include "core/components/arc/arc_component.h"
#include "core/components/indexer/render_indexer.h"
#include "core/components/indexer/render_indexer_circle.h"
#include "core/components/indexer/render_indexer_list.h"
#include "core/components/test/unittest/list/list_test_utils.h"

namespace OHOS::Ace {
namespace {

class MockRenderIndexList final : public RenderIndexerList {
DECLARE_ACE_TYPE(MockRenderIndexList, RenderIndexerList);

public:
    MockRenderIndexList() = default;
    ~MockRenderIndexList() override = default;
};

} // namespace

class MockRenderArc final : public RenderArc {
DECLARE_ACE_TYPE(MockRenderArc, RenderArc);

public:
    MockRenderArc() = default;
    ~MockRenderArc() override = default;

    bool IsInRange(double position, double radius = 0)
    {
        double start = GetStartAngle();
        double sweep = GetSweepAngle();
        return GreatOrEqual(position, start - radius) && LessNotEqual(position, start + sweep + radius);
    }
};

class MockRenderIndexer final : public RenderIndexer {
DECLARE_ACE_TYPE(MockRenderIndexer, RenderIndexer);

public:
    MockRenderIndexer() = default;
    ~MockRenderIndexer() override = default;

    int32_t GetFocusItemIndex() const
    {
        return focusedItem_;
    }

    void MoveSection(int32_t indexInlist)
    {
        RenderIndexer::MoveSection(indexInlist);
    }
};

class MockRenderIndexerCircle final : public RenderIndexerCircle {
DECLARE_ACE_TYPE(MockRenderIndexerCircle, RenderIndexerCircle);

public:
    MockRenderIndexerCircle() = default;
    ~MockRenderIndexerCircle() override = default;

    RefPtr<RenderIndexerItem> GetSpecificItem(int32_t index)
    {
        return RenderIndexerCircle::GetSpecificItem(index);
    }

    bool NeedChangeLanguage(int32_t index)
    {
        return RenderIndexerCircle::NeedChangeIndexer(index);
    }

    bool ChangeLanguage(int32_t index)
    {
        bool res = RenderIndexerCircle::ChangeLanguage(index);
        if (res) {
            PerformLayout();
        }
        return res;
    }

    int32_t GetFocusItemIndex() const
    {
        return focusedItem_;
    }

    int32_t GetMaxShowItemCount() const
    {
        return itemMaxCount_;
    }

    int32_t GetCurrentItemType() const
    {
        return curItemType_;
    }

    int32_t GetFocusItemIndex()
    {
        return focusedItem_;
    }

    double GetArcHeadOffset()
    {
        return arcHeadOffset_;
    }

    IndexerItemStatus GetIndexerStatus() const
    {
        return curStatus_;
    }

    IndexerItemStatus GetIndexerNextStatus() const
    {
        return nextStatus_;
    }

    int32_t GetActualItemIndex(int32_t index)
    {
        return RenderIndexerCircle::GetActualItemIndex(index);
    }

    void HandleRotation(double value)
    {
        RenderIndexerCircle::HandleRotation(value);
    }

    void CollapseItems()
    {
        RenderIndexerCircle::CollapseItems();
    }

    void ExpandItems()
    {
        RenderIndexerCircle::ExpandItems();
    }

    void FinishCollapseAnimation()
    {
        if (collapseController_ && collapseController_->IsRunning()) {
            if (GetIndexerNextStatus() == IndexerItemStatus::COLLAPSE) {
                collapseController_->TriggerFrame(0);
                collapseController_->Stop();
            } else if (GetIndexerNextStatus() == IndexerItemStatus::EXPAND) {
                collapseController_->Finish();
            }
        }
    }

    void FinishFocusAnimation()
    {
        if (focusController_ && focusController_->IsRunning()) {
            focusController_->TriggerFrame(INDEXER_ANIMATION_DURATION);
            focusController_->Finish();
        }
    }

    void MoveSection(int32_t indexInlist)
    {
        RenderIndexer::MoveSection(indexInlist);
    }

    void SetItemsFocused(int32_t index)
    {
        RenderIndexerCircle::SetItemsFocused(index);
    }
};

class MockRenderDisplay final : public RenderDisplay {
DECLARE_ACE_TYPE(MockRenderDisplay, RenderDisplay);

public:
    MockRenderDisplay() = default;
    ~MockRenderDisplay() override = default;
};

class MockRenderText : public RenderText {
DECLARE_ACE_TYPE(MockRenderText, RenderText);

public:
    MockRenderText() = default;
    ~MockRenderText() override = default;

    uint32_t GetTextLines() override
    {
        return 0;
    }

    double GetTextWidth() override
    {
        return 0;
    }

    Size Measure() override
    {
        return Size(0.0, 0.0);
    }

    int32_t GetTouchPosition(const Offset& offset) override
    {
        return 0;
    }
};

class IndexerTestUtils {
public:
    static RefPtr<RenderBox> CreateRenderBox(double width, double height);
    static RefPtr<RenderNode> CreateRenderItem(double width, double height, const RefPtr<PipelineContext>& context);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_INDEXER_INDEXER_TEST_UTILS_H
