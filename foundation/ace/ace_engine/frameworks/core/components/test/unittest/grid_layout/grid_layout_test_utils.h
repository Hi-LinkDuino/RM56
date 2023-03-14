/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_GRID_LAYOUT_GRID_LAYOUT_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_GRID_LAYOUT_GRID_LAYOUT_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/grid_layout/render_grid_layout.h"
#include "core/components/grid_layout/render_grid_layout_item.h"

namespace OHOS::Ace {
namespace {
constexpr int32_t INIT_VALUE_UT_TEST = -2;
class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;
};

class MockRenderGridLayoutItem final : public RenderGridLayoutItem {
    DECLARE_ACE_TYPE(MockRenderGridLayoutItem, RenderGridLayoutItem);

public:
    MockRenderGridLayoutItem() = default;
    ~MockRenderGridLayoutItem() override = default;
};

class GridDragEventResult {
public:
    explicit GridDragEventResult() {}
    ~GridDragEventResult() = default;
    void SetLongPress(bool longPress)
    {
        longPress_ = longPress;
    }
    bool GetLongPress() const
    {
        return longPress_;
    }
    void SetOnDrop(bool onDrop)
    {
        onDrop_ = onDrop;
    }
    bool GetOnDrop() const
    {
        return onDrop_;
    }
    void SetOnDragEnter(bool onDragEnter)
    {
        onDragEnter_ = onDragEnter;
    }
    bool GetOnDragEnter() const
    {
        return onDragEnter_;
    }
    void SetOnDragMove(bool onDragMove)
    {
        onDragMove_ = onDragMove;
    }
    bool GetOnDragMove() const
    {
        return onDragMove_;
    }
    void SetOnDragLeave(bool onDragLeave)
    {
        onDragLeave_ = onDragLeave;
    }
    bool GetOnDragLeave() const
    {
        return onDragLeave_;
    }
    void SetOnDragStart(bool onDragStart)
    {
        onDragStart_ = onDragStart;
    }
    bool GetOnDragStart() const
    {
        return onDragStart_;
    }
    void SetDropItemIndex(int32_t itemIndex)
    {
        itemIndex_ = itemIndex;
    }
    int32_t GetDropItemIndex()
    {
        return itemIndex_ ;
    }
    void SetDropInsertIndex(int32_t insertIndex)
    {
        insertIndex_ = insertIndex;
    }
    int32_t GetDropInsertIndex()
    {
        return insertIndex_;
    }

private:
    bool longPress_ = false;
    bool onDrop_ = false;
    bool onDragEnter_ = false;
    bool onDragMove_ = false;
    bool onDragLeave_ = false;
    bool onDragStart_ = false;
    int32_t itemIndex_ = INIT_VALUE_UT_TEST;
    int32_t insertIndex_ = INIT_VALUE_UT_TEST;
};

} // namespace

class GridLayoutTestUtils {
public:
    static void PrintNodeInfo(const RefPtr<RenderNode>& node);
    static RefPtr<Component> CreateComponent(FlexDirection direction, std::string rows, std::string cols);
    static RefPtr<Component> CreateDragComponent(std::string rows, std::string cols);
    static RefPtr<RenderNode> CreateRenderItem(int32_t row, int32_t col, int32_t rowSpan, int32_t colSpan);
    static RefPtr<RenderNode> CreateDragRenderItem();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_GRID_LAYOUT_GRID_LAYOUT_TEST_UTILS_H
