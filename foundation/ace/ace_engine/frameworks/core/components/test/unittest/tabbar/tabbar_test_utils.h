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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_TABBAR_TABBAR_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_TABBAR_TABBAR_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/flex/render_flex.h"
#include "core/components/root/render_root.h"
#include "core/components/tab_bar/render_tab_bar.h"

namespace OHOS::Ace {

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;
};

class MockRenderRoot final : public RenderRoot {
    DECLARE_ACE_TYPE(MockRenderRoot, RenderRoot);

public:
    MockRenderRoot() = default;
    ~MockRenderRoot() override = default;
};

class MockRenderTabBar final : public RenderTabBar {
    DECLARE_ACE_TYPE(MockRenderTabBar, RenderTabBar);

public:
    MockRenderTabBar() = default;
    ~MockRenderTabBar() override = default;

    double GetActualWidth() const
    {
        return actualWidth_;
    }

    TabBarMode GetMode() const
    {
        return mode_;
    }

    int32_t GetIndex() const
    {
        return index_;
    }

    int32_t GetTabsSize() const
    {
        return tabsSize_;
    }
};

class TabBarTestUtils {
public:
    static RefPtr<RenderRoot> CreateRenderRoot();
    static RefPtr<RenderBox> CreateRenderBox(const double width, const double height, BoxFlex flex = BoxFlex::FLEX_NO);
    static RefPtr<MockRenderTabBar> CreateRenderTabBar();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_TABBAR_TABBAR_TEST_UTILS_H
