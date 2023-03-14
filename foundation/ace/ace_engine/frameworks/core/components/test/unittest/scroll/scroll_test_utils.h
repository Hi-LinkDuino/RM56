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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_SCROLL_SCROLL_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_SCROLL_SCROLL_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/flex/render_flex.h"
#include "core/components/scroll/render_single_child_scroll.h"

namespace OHOS::Ace {
namespace {

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;
};

} // namespace

class ScrollTestUtils {
public:
    static RefPtr<RenderFlex> CreateRenderFlex(FlexDirection direction, FlexAlign main, FlexAlign cross);
    static RefPtr<RenderBox> CreateRenderBox(double width, double height, bool deliverMinToChild = true);
    static RefPtr<BoxComponent> CreateBoxComponent(double width, double height, bool deliverMinToChild = true);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_SCROLL_SCROLL_TEST_UTILS_H
