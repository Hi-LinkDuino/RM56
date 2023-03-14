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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_MOCK_RENDER_DEPEND_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_MOCK_RENDER_DEPEND_H

#ifndef OHOS_STANDARD_SYSTEM
#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#endif
#include "core/animation/animator.h"
#include "core/animation/card_transition_controller.h"
#include "core/animation/shared_transition_controller.h"
#include "core/common/window.h"
#include "core/components/box/box_component.h"
#include "core/components/box/render_box.h"
#include "core/components/clock/render_clock.h"
#include "core/components/dialog_modal/dialog_modal_component.h"
#include "core/components/dialog_modal/dialog_modal_element.h"
#include "core/components/display/render_display.h"
#include "core/components/positioned/render_positioned.h"
#include "core/components/rating/render_rating.h"
#include "core/components/root/render_root.h"
#include "core/components/root/root_element.h"
#include "core/components/semi_modal/semi_modal_component.h"
#include "core/components/semi_modal/semi_modal_element.h"
#include "core/components/stack/render_stack.h"
#include "core/components/swiper/render_swiper.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/text/render_text.h"
#include "core/components/transform/render_transform.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/base/composed_element.h"

namespace OHOS::Ace {

std::function<void(const RefPtr<RenderBox>&)> hookRenderBox;

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

#ifndef OHOS_STANDARD_SYSTEM
Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}
#endif

std::unique_ptr<PlatformWindow> PlatformWindow::Create(AceView* aceView)
{
    return std::make_unique<MockWindow>(aceView);
}

class MockRenderSwiper : public RenderSwiper {
    DECLARE_ACE_TYPE(MockRenderSwiper, RenderSwiper);

private:
    void UpdateIndicator() override {}
};

class MockRenderText : public RenderText {
    DECLARE_ACE_TYPE(MockRenderText, RenderText);

public:
    MockRenderText() = default;
    ~MockRenderText() override = default;

    uint32_t GetTextLines() override
    {
        return 0;
    }

    double GetTextWidth() override
    {
        return 0;
    }

    Size Measure() override
    {
        return Size(1080.1, 0.0);
    }

    int32_t GetTouchPosition(const Offset& offset) override
    {
        return 0;
    }
};

class MockRenderPositioned : public RenderPositioned {
    DECLARE_ACE_TYPE(MockRenderPositioned, RenderPositioned);
};

class MockRenderTransform : public RenderTransform {
    DECLARE_ACE_TYPE(MockRenderTransform, RenderTransform);
};

class MockRenderContext : public RenderContext {
    DECLARE_ACE_TYPE(MockRenderContext, RenderContext);

public:
    void Repaint(const RefPtr<RenderNode>& node) override {};
    void PaintChild(const RefPtr<RenderNode>& child, const Offset& offset) override {};
    void Restore() override {};
};

class MockRenderImage : public RenderImage {
    DECLARE_ACE_TYPE(MockRenderImage, RenderImage);

public:
    MockRenderImage() = default;
    ~MockRenderImage() override = default;

    Size Measure() override
    {
        return Size();
    }
};

RefPtr<RenderNode> RenderImage::Create()
{
    return AceType::MakeRefPtr<MockRenderImage>();
}

class MockRenderRating : public RenderRating {
    DECLARE_ACE_TYPE(MockRenderRating, RenderRating);

public:
    MockRenderRating() = default;
    ~MockRenderRating() override = default;

protected:
    void PaintFocus(
        const Offset& offset, double rrectRadius, const Size& boardSize, RenderContext& context) override {};
};

RefPtr<RenderNode> RenderRating::Create()
{
    return AceType::MakeRefPtr<MockRenderRating>();
}

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

class MockRenderBox : public RenderBox {};
class MockRenderBoxBase : public RenderBoxBase {};
class MockRenderRoot : public RenderRoot {};
class MockRenderStack : public RenderStack {};
class MockRenderDisplay : public RenderDisplay {};

class RenderTestUtils {
public:
    static RefPtr<PipelineContext> GetMockPipelineContext(const RefPtr<Frontend>& frontend);
};

RefPtr<PipelineContext> RenderTestUtils::GetMockPipelineContext(const RefPtr<Frontend>& frontend)
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
}

RefPtr<RenderContext> RenderContext::Create()
{
    return AceType::MakeRefPtr<MockRenderContext>();
}

RefPtr<RenderNode> RenderSwiper::Create()
{
    return AceType::MakeRefPtr<MockRenderSwiper>();
}

RefPtr<RenderNode> RenderDisplay::Create()
{
    return AceType::MakeRefPtr<MockRenderDisplay>();
}

RefPtr<RenderNode> RenderStack::Create()
{
    return AceType::MakeRefPtr<MockRenderStack>();
}

RefPtr<RenderNode> RenderBox::Create()
{
    const auto& box = AceType::MakeRefPtr<MockRenderBox>();

    if (hookRenderBox) {
        hookRenderBox(box);
    }
    return box;
}

RefPtr<RenderNode> RenderRoot::Create()
{
    return AceType::MakeRefPtr<MockRenderRoot>();
}

RefPtr<RenderNode> RenderText::Create()
{
    return AceType::MakeRefPtr<MockRenderText>();
}

void RenderPositioned::Update(const RefPtr<Component>& component) {}

void RenderPositioned::SetLeft(const Dimension& left) {}

void RenderPositioned::SetTop(const Dimension& top) {}

void RenderPositioned::SetRight(const Dimension& right) {}

void RenderPositioned::SetBottom(const Dimension& bottom) {}

SharedTransitionController::SharedTransitionController(const WeakPtr<PipelineContext>& context) {}
void SharedTransitionController::RegisterTransitionListener() {}
bool SharedTransitionController::HasSharedTransition(TransitionEvent event)
{
    return false;
};

RefPtr<Element> SharedTransitionElement::GetContentElement() const
{
    return nullptr;
}

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

RefPtr<RenderNode> RenderPositioned::Create()
{
    return AceType::MakeRefPtr<MockRenderPositioned>();
}

RefPtr<RenderNode> RenderTransform::Create()
{
    return AceType::MakeRefPtr<MockRenderTransform>();
}

void SemiModalElement::UpdateSystemBarHeight(double statusBar, double navigationBar) {}

RefPtr<OverlayElement> SemiModalElement::GetOverlayElement() const
{
    return nullptr;
}

RefPtr<StageElement> SemiModalElement::GetStageElement() const
{
    return nullptr;
}

RefPtr<Component> SemiModalComponent::Create(
    RefPtr<Component> child, bool isFullScreen, int32_t modalHeight, uint32_t color)
{
    return nullptr;
}

void DialogModalElement::UpdateSystemBarHeight(double statusBar, double navigationBar) {}

RefPtr<OverlayElement> DialogModalElement::GetOverlayElement() const
{
    return nullptr;
}

RefPtr<StageElement> DialogModalElement::GetStageElement() const
{
    return nullptr;
}

RefPtr<Component> DialogModalComponent::Create(RefPtr<Component> child)
{
    return nullptr;
}

void SemiModalElement::AnimateToExitApp() const {}

bool SemiModalElement::IsFullWindow() const
{
    return true;
}

void DialogModalElement::AnimateToExitApp() {}

void DialogModalElement::RegisterTransitionListener() {}

class MockRenderClock : public RenderClock {
    DECLARE_ACE_TYPE(MockRenderClock, RenderClock);

public:
    MockRenderClock() = default;
    ~MockRenderClock() override = default;
};

RefPtr<RenderNode> RenderClock::Create()
{
    return AceType::MakeRefPtr<MockRenderClock>();
}

class MockRenderClockHand : public RenderClockHand {
    DECLARE_ACE_TYPE(MockRenderClockHand, RenderClockHand);

public:
    MockRenderClockHand() = default;
    ~MockRenderClockHand() override = default;
};

RefPtr<RenderNode> RenderClockHand::Create()
{
    return AceType::MakeRefPtr<MockRenderClockHand>();
}

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_MOCK_RENDER_DEPEND_H
