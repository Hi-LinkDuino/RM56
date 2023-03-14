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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DISPLAY_DISPLAY_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DISPLAY_DISPLAY_TEST_UTILS_H

#include "core/common/window.h"
#include "core/components/box/render_box.h"
#include "core/components/display/display_component.h"
#include "core/components/display/render_display.h"
#include "core/components/flex/render_flex.h"
#include "core/components/root/render_root.h"

namespace OHOS::Ace {

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;
};

class MockRenderDisplay final : public RenderDisplay {
    DECLARE_ACE_TYPE(MockRenderDisplay, RenderDisplay);

public:
    MockRenderDisplay() = default;
    ~MockRenderDisplay() override = default;

    double GetOpacity() const
    {
        return opacity_;
    }
};

class MockRenderRoot final : public RenderRoot {
    DECLARE_ACE_TYPE(MockRenderRoot, RenderRoot);

public:
    MockRenderRoot() = default;
    ~MockRenderRoot() override = default;
};

class MockWindow final : public PlatformWindow {
public:
    explicit MockWindow(AceView* aceView) {}
    ~MockWindow() override = default;

    // Platform window interface
    void RequestFrame() override {}
    void RegisterVsyncCallback(AceVsyncCallback&& callback) override {}
    void SetRootRenderNode(const RefPtr<RenderNode>& root) override {}

private:
    ACE_DISALLOW_COPY_AND_MOVE(MockWindow);
};

class DisplayTestUtils {
public:
    static RefPtr<RenderRoot> CreateRenderRoot();
    static RefPtr<RenderFlex> CreateRenderFlex(
        const FlexDirection direction, const FlexAlign main, const FlexAlign cross);
    static RefPtr<RenderBox> CreateRenderBox(const double width, const double height);
    static RefPtr<MockRenderDisplay> CreateRenderDisplay(const VisibleType visible, const double opacity);
    static RefPtr<PipelineContext> GetMockContext();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_DISPLAY_DISPLAY_TEST_UTILS_H
