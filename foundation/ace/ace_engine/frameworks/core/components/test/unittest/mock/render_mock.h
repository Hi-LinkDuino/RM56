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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_RENDER_MOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_RENDER_MOCK_H

#include "core/common/platform_window.h"
#include "core/common/window.h"
#include "core/components/box/render_box.h"
#include "core/components/clip/render_clip.h"
#include "core/components/display/render_display.h"
#include "core/components/drop_filter/render_drop_filter.h"
#include "core/components/image/render_image.h"
#include "core/components/positioned/render_positioned.h"
#include "core/components/root/render_root.h"
#include "core/components/stack/render_stack.h"
#include "core/components/test/unittest/mock/transform_mock.h"

namespace OHOS::Ace {

class MockRenderRoot : public RenderRoot { DECLARE_ACE_TYPE(MockRenderRoot, RenderRoot); };
class MockRenderStack : public RenderStack { DECLARE_ACE_TYPE(MockRenderStack, RenderStack); };
class MockRenderPositioned : public RenderPositioned { DECLARE_ACE_TYPE(MockRenderPositioned, RenderPositioned); };

class MockRenderImage : public RenderImage {
    DECLARE_ACE_TYPE(MockRenderImage, RenderImage);

public:
    double GetWidth() const
    {
        return width_.Value();
    }

    double GetHeight() const
    {
        return height_.Value();
    }

    Size Measure() override
    {
        return Size();
    }
};

class MockRenderDropFilter : public RenderDropFilter {
    DECLARE_ACE_TYPE(MockRenderDropFilter, RenderDropFilter);

public:
    using HookRenderDropFilter = std::function<void(const RefPtr<MockRenderDropFilter>&)>;
    double GetSigmaX() const
    {
        return sigmaX_;
    }
    double GetSigmaY() const
    {
        return sigmaY_;
    }
    static void SetMockHook(const HookRenderDropFilter& hook);
};

class MockRenderBox : public RenderBox {
    DECLARE_ACE_TYPE(MockRenderBox, RenderBox);

public:
    int32_t GetPercentFlag() const
    {
        return percentFlag_;
    }

    const Edge& GetMarginOrigin() const
    {
        return marginOrigin_;
    }

    const Edge& GetPaddingOrigin() const
    {
        return paddingOrigin_;
    }
};

class MockRenderClip : public RenderClip {
    DECLARE_ACE_TYPE(MockRenderClip, RenderClip);
};

class MockRenderBoxBase : public RenderBoxBase {
    DECLARE_ACE_TYPE(MockRenderBoxBase, RenderBoxBase);
};

class MockImageCache : public ImageCache {
    DECLARE_ACE_TYPE(MockImageCache, ImageCache);

public:
    void Clear() override {};
};

class MockRenderDisplay : public RenderDisplay {
    DECLARE_ACE_TYPE(MockRenderDisplay, RenderDisplay);

public:
    using HookRenderDisplay = std::function<void(const RefPtr<MockRenderDisplay>&)>;
    float GetOpacity()
    {
        return static_cast<float>(opacity_) / UINT8_MAX;
    }
    static void SetMockHook(const HookRenderDisplay& hook);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_RENDER_MOCK_H
