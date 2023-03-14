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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_ANIMATION_TEST_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_ANIMATION_TEST_UTILS_H

#include <cstdint>

#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "base/json/json_util.h"
#include "core/common/platform_window.h"
#include "core/common/window.h"
#include "core/components/box/render_box.h"
#include "core/components/display/render_display.h"
#include "core/components/root/render_root.h"
#include "core/components/stack/render_stack.h"
#include "core/components/transform/render_transform.h"
#include "core/pipeline/base/render_context.h"

namespace OHOS::Ace {

class MockPlatformWindow final : public PlatformWindow {
public:
    MockPlatformWindow() = default;
    ~MockPlatformWindow() override = default;

    // Request next vsync.
    void RequestFrame() override
    {
        LOGD("MockPlatformWindow RequestFrame");
        requestFrameCnt_++;
    }

    int32_t GetRequestFrameCnt() const
    {
        return requestFrameCnt_;
    }

    void ResetRequestFrameCnt()
    {
        requestFrameCnt_ = 0;
    }

    void TriggerOneFrame()
    {
        TriggerFrame(nanoFrameTime_, 1);
    }

    void TriggerFrames(int32_t frameCount)
    {
        for (int32_t loop = 0; loop < frameCount; loop++) {
            TriggerOneFrame();
        }
    }

    uint64_t GetCurrentTimestampNano() const
    {
        return nanoTimestamp_;
    }

    void TriggerFrame(uint64_t deltaNanoTimestamp, uint32_t deltaFrameCount)
    {
        nanoTimestamp_ += deltaNanoTimestamp;
        frameCount_ += deltaFrameCount;
        if (callback_) {
            callback_(nanoTimestamp_, frameCount_);
        }
    }

    // Register Vsync callback.
    void RegisterVsyncCallback(AceVsyncCallback&& callback) override
    {
        callback_ = std::move(callback);
    }

    // Attach root render node to container
    void SetRootRenderNode(const RefPtr<RenderNode>& root) override {}

    void SetNanoFrameTime(uint64_t nanoFrameTime)
    {
        nanoFrameTime_ = nanoFrameTime;
    }

private:
    static constexpr uint16_t SCREEN_FREQUENCY = 60;
    static constexpr uint64_t NANO_FRAME_TIME = static_cast<const uint64_t>(1e9 / SCREEN_FREQUENCY);
    uint32_t frameCount_ = 0;
    uint32_t requestFrameCnt_ = 0;
    uint64_t nanoFrameTime_ = NANO_FRAME_TIME;
    uint64_t nanoTimestamp_ = 0;
    AceVsyncCallback callback_;
};

class MockRenderContext : public RenderContext {
public:
    void Repaint(const RefPtr<RenderNode>& node) override {};
    void PaintChild(const RefPtr<RenderNode>& child, const Offset& offset) override {};
    void Restore() override {};
};

RefPtr<RenderContext> RenderContext::Create()
{
    return AceType::MakeRefPtr<MockRenderContext>();
}

class MockRenderRoot : public RenderRoot {};
class MockRenderStack : public RenderStack {};
class MockRenderDisplay : public RenderDisplay {};
class MockRenderPositioned : public RenderPositioned {};
class MockRenderTransform : public RenderTransform {};
class MockRenderBox : public RenderBox {};
class MockRenderBoxBase : public RenderBoxBase {};
class MockRenderImage : public RenderImage {
public:
    Size Measure() override
    {
        return Size();
    }
};

RefPtr<RenderNode> RenderRoot::Create()
{
    return AceType::MakeRefPtr<MockRenderRoot>();
}

RefPtr<RenderNode> RenderStack::Create()
{
    return AceType::MakeRefPtr<MockRenderStack>();
}

RefPtr<RenderNode> RenderDisplay::Create()
{
    return AceType::MakeRefPtr<MockRenderDisplay>();
}

RefPtr<RenderNode> RenderTransform::Create()
{
    return AceType::MakeRefPtr<MockRenderTransform>();
}

RefPtr<RenderNode> RenderBox::Create()
{
    return AceType::MakeRefPtr<MockRenderBox>();
}

RefPtr<RenderNode> RenderImage::Create()
{
    return AceType::MakeRefPtr<MockRenderImage>();
}

class AnimationTestUtils {
public:
    AnimationTestUtils() = delete;
    ~AnimationTestUtils() = delete;

    static std::unique_ptr<PlatformWindow> CreatePlatformWindow()
    {
        return std::make_unique<MockPlatformWindow>();
    }

    static std::unique_ptr<Window> CreateWindow(std::unique_ptr<PlatformWindow> platformWindow)
    {
        return std::make_unique<Window>(std::move(platformWindow));
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ANIMATION_TEST_UNITTEST_MOCK_ANIMATION_TEST_UTILS_H
