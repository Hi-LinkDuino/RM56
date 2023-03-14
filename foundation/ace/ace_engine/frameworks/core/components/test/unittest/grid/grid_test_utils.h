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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_GRID_GRID_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_GRID_GRID_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/grid/render_grid.h"
#include "core/components/grid/render_grid_item.h"
#include "core/components/proxy/render_item_proxy.h"

namespace OHOS::Ace {
namespace {

using GridEventCallback = std::function<void(const std::string&, const std::string&)>;

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;
};

class MockRenderGridItem final : public RenderGridItem {
    DECLARE_ACE_TYPE(MockRenderGridItem, RenderGridItem);

public:
    MockRenderGridItem() = default;
    ~MockRenderGridItem() override = default;
};

class MockEventHandler : public AceEventHandler {
    void HandleAsyncEvent(const EventMarker& eventMarker) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, int32_t param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param) override {};
    void HandleAsyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const BaseEventInfo& info, bool& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const std::string& param, std::string& result) override {};
    void HandleSyncEvent(const EventMarker& eventMarker, const KeyEvent& keyEvent, bool& result) override {};
    void HandleSyncEvent(
        const EventMarker& eventMarker, const std::string& componentId, const int32_t nodeId) override {};
};

class TestGridEventHander : public MockEventHandler {
public:
    explicit TestGridEventHander(GridEventCallback eventCallback) : eventCallback_(eventCallback) {}
    ~TestGridEventHander() = default;

    void HandleAsyncEvent(const EventMarker& eventMarker, const std::string& param)
    {
        if (eventCallback_) {
            eventCallback_(eventMarker.GetData().eventId, param);
        }
    };

private:
    GridEventCallback eventCallback_;
};

} // namespace

class GridTestUtils {
public:
    static RefPtr<RenderBox> CreateRenderBox(double width, double height);
    static RefPtr<RenderBox> CreateRenderItem(double width, double height,
        int32_t span, const RefPtr<PipelineContext>& context);
    static RefPtr<RenderGrid> InitRenderGridDiffSpan(FlexDirection direction,
        const RefPtr<PipelineContext>& context);
    static RefPtr<RenderGrid> InitRenderGridSameSpan(FlexDirection direction,
        const RefPtr<PipelineContext>& context);
    static Offset GetChildPosition(const RefPtr<RenderGrid>& grid, int32_t index);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_GRID_GRID_TEST_UTILS_H
