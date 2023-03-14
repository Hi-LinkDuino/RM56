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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_FLEX_FLEX_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_FLEX_FLEX_TEST_UTILS_H

#include "core/components/box/render_box.h"
#include "core/components/flex/flex_component.h"
#include "core/components/flex/render_flex.h"
#include "core/components/flex/render_flex_item.h"
#include "core/components/root/render_root.h"
#include "core/components/test/unittest/mock/mock_render_common.h"
#include "core/components/text/render_text.h"

namespace OHOS::Ace {

class MockRenderBox final : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    MockRenderBox() = default;
    ~MockRenderBox() override = default;

    Size GetContentSize() override
    {
        return Size();
    }
};

class MockRenderRoot final : public RenderRoot {
    DECLARE_ACE_TYPE(MockRenderRoot, RenderRoot);

public:
    MockRenderRoot() = default;
    ~MockRenderRoot() override = default;
};

class MockRenderText final : public RenderText {
    DECLARE_ACE_TYPE(MockRenderText, RenderText);

public:
    MockRenderText() = default;
    ~MockRenderText() override = default;

    uint32_t GetTextLines() override
    {
        return 0;
    }

    void SetFontSize(double fontSize)
    {
        textStyle_.SetFontSize(Dimension(fontSize, DimensionUnit::PX));
    }

    int32_t GetTouchPosition(const Offset& offset) override
    {
        return 0;
    }

    double GetTextWidth() override;

    double GetBaselineDistance(TextBaseline textBaseline) override;

protected:
    Size Measure() override;
};

class FlexTestUtils {
public:
    static RefPtr<RenderRoot> CreateRenderRoot();
    static RefPtr<RenderFlex> CreateRenderFlex(FlexDirection direction, FlexAlign main, FlexAlign cross);
    static RefPtr<RenderBox> CreateRenderBox(double width, double height);
    static RefPtr<RenderBox> CreateWeightedBox(double flexWeight, double width = 0.0, double height = 100.0);
    static RefPtr<RenderFlexItem> CreateRenderFlexItem(double flexBasis, double flexGrow, double flexShrink);
    static RefPtr<RenderFlexItem> CreateMagicNode(double aspectRatio, double flexWeight, int32_t displayIndex,
        const LayoutParam& constraints, const RefPtr<PipelineContext>& context);
    static RefPtr<MockRenderText> CreateRenderText(double fontSize);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_FLEX_FLEX_TEST_UTILS_H
