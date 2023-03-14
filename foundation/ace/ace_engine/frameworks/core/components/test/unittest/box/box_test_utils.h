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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_BOX_BOX_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_BOX_BOX_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/root/render_root.h"
#include "core/components/text/render_text.h"

namespace OHOS::Ace {

constexpr double LENGTH = 100.0;

class MockRenderRoot final : public RenderRoot {
    DECLARE_ACE_TYPE(MockRenderRoot, RenderRoot);
};

class MockRenderBoxBase final : public RenderBoxBase {
    DECLARE_ACE_TYPE(MockRenderBoxBase, RenderBoxBase);

public:
    void OnMouseHoverEnterTest() override
    {
        enterCallBackCount_++;
    }

    void OnMouseHoverExitTest() override
    {
        exitCallBackCount_++;
    }

    int32_t enterCallBackCount_ = 0;
    int32_t exitCallBackCount_ = 0;
};

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    void OnMouseHoverEnterAnimation() override
    {
        enterAnimationCount_++;
    }

    void OnMouseHoverExitAnimation() override
    {
        exitAnimationCount_++;
    }

    int32_t enterAnimationCount_ = 0;
    int32_t exitAnimationCount_ = 0;
};

class MockBoxBaseComponent final : public BoxBaseComponent {
    DECLARE_ACE_TYPE(MockBoxBaseComponent, BoxBaseComponent);

public:
    RefPtr<Element> CreateElement() override
    {
        return nullptr;
    }

    RefPtr<RenderNode> CreateRenderNode() override
    {
        return nullptr;
    }
};

class MockRenderText final : public RenderText {
    DECLARE_ACE_TYPE(MockRenderText, RenderText);

public:
    uint32_t GetTextLines() override
    {
        return 0;
    }

    Size Measure() override
    {
        return Size(LENGTH, LENGTH);
    }

    double GetTextWidth() override
    {
        return 0;
    }

    int32_t GetTouchPosition(const Offset& offset) override
    {
        return 0;
    }
};

class BoxTestUtils {
public:
    static RefPtr<RenderRoot> CreateRenderRoot();
    static RefPtr<RenderText> CreateRenderText(const RefPtr<PipelineContext>& context);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_BOX_BOX_TEST_UTILS_H
