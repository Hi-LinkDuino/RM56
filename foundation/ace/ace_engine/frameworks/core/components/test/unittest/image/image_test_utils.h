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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_IMAGE_IMAGE_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_IMAGE_IMAGE_TEST_UTILS_H

#include "base/geometry/offset.h"
#include "base/geometry/rect.h"
#include "core/common/window.h"
#include "core/components/common/properties/alignment.h"
#include "core/components/display/render_display.h"
#include "core/components/root/render_root.h"
#include "core/components/stack/render_stack.h"
#include "core/pipeline/base/constants.h"

#define protected public
#include "core/components/image/render_image.h"

namespace OHOS::Ace {

const double EXTRA_SMALL_LENGTH = 100.0;
const double SMALL_LENGTH = 200.0;
const double MINUS_EXTRA_SMALL_LENGTH = -100.0;
const double MIDDLE_LENGTH = 300.0;
const double LARGE_LENGTH = 400.0;
const double EXTRA_LARGE_LENGTH = 500.0;
const double DEFAULT_LENGTH = -1.0;
const int32_t REPEAT_LIST_LENGTH = 4;
const int32_t REPEATX_LIST_LENGTH = 2;
const int32_t REPEATY_LIST_LENGTH = 2;
const int32_t NOREPEAT_LIST_LENGTH = 1;
const Size PARENT_SIZE = Size(300.0, 300.0);
const bool FIT_MAX_SIZE = true;

class MockRenderImage final : public RenderImage {
    DECLARE_ACE_TYPE(MockRenderImage, RenderImage);

public:
    MockRenderImage() = default;
    ~MockRenderImage() override = default;

    Size Measure() override
    {
        return Size(LARGE_LENGTH, LARGE_LENGTH);
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

class MockRenderContext : public RenderContext {
public:
    void Repaint(const RefPtr<RenderNode>& node) override {};
    void PaintChild(const RefPtr<RenderNode>& child, const Offset& offset) override {};
    void Restore() override {};
};

class MockRenderStack : public RenderStack {
    DECLARE_ACE_TYPE(MockRenderStack, RenderStack);
};

class MockRenderDisplay : public RenderDisplay {
    DECLARE_ACE_TYPE(MockRenderDisplay, RenderDisplay);
};

struct ImageFitConfig final {
    ImageFitConfig(
        const Alignment& alignment, const ImageFit& imageFit, const Size& srcRectSize, const Size& dstRectSize)
        : alignment(alignment), imageFit(imageFit), srcRectSize(srcRectSize), dstRectSize(dstRectSize),
          srcRect(Offset::Zero(), srcRectSize), dstRect(Offset::Zero(), dstRectSize)
    {}
    ~ImageFitConfig() = default;

    Alignment alignment;
    ImageFit imageFit = ImageFit::CONTAIN;
    Size srcRectSize;
    Size dstRectSize;
    Rect srcRect;
    Rect dstRect;
};

void VerifyImageFit(ImageFitConfig&& imageFitConfig, const Rect& expectSrcRect, const Rect& expectDstRect);
void VerifyRectSize(const std::list<Rect>& rectList, const Size& expectRectSize);
RefPtr<RenderRoot> CreateRenderRoot(const Size& maxSize);
RefPtr<RenderImage> CreateRenderImage(double width, double height, bool fitMaxSize = false);
RefPtr<PipelineContext> GetMockContext();
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_IMAGE_IMAGE_TEST_UTILS_H
