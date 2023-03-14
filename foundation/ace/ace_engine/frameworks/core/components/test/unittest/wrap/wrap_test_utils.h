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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_WRAP_WRAP_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_WRAP_WRAP_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/root/render_root.h"
#include "core/components/wrap/wrap_component.h"

namespace OHOS::Ace {
namespace {

constexpr double SCREEN_WIDTH = 1080.0;
constexpr double SCREEN_HEIGHT = 2048.0;

} // namespace

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

class WrapTestUtils {
public:
    static RefPtr<RenderRoot> CreateRenderRoot();
    static RefPtr<RenderWrap> CreateRenderWrap(const RefPtr<WrapComponent>& component,
        const RefPtr<PipelineContext>& context);
    static RefPtr<RenderBox> CreateRenderBox(double width, double height);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_WRAP_WRAP_TEST_UTILS_H
