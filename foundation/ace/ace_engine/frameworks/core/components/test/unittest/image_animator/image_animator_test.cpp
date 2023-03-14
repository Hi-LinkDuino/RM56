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

#include "gtest/gtest.h"

#include "adapter/aosp/entrance/java/jni/jni_environment.h"
#include "base/log/log.h"
#include "core/animation/card_transition_controller.h"
#include "core/components/box/box_component.h"
#include "core/components/image/image_animator_component.h"
#include "core/components/image/image_animator_element.h"
#include "core/components/stage/stage_element.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/test/unittest/mock/render_mock.h"
#include "core/components/test/unittest/mock/window_mock.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"
#include "core/pipeline/pipeline_context.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Ace {

Platform::JniEnvironment::JniEnvironment() {}

Platform::JniEnvironment::~JniEnvironment() = default;

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

void CardTransitionController::RegisterTransitionListener() {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

std::shared_ptr<JNIEnv> Platform::JniEnvironment::GetJniEnv(JNIEnv* jniEnv) const
{
    return nullptr;
}

Platform::JniEnvironment& Platform::JniEnvironment::GetInstance()
{
    static Platform::JniEnvironment jniEnvironment;
    return jniEnvironment;
}

namespace {

constexpr int32_t FRAME_TIME_IN_MILLISECOND = 10;
constexpr int32_t NANOSECOND_TO_MILLISECOND = 1000000;
constexpr int32_t TEST_SURFACE_WIDTH = 1080;
constexpr int32_t TEST_SURFACE_HEIGHT = 1920;
std::vector<ImageProperties> g_images;
std::vector<ImageProperties> g_secondImages;
std::vector<ImageProperties> g_thirdImages;

using ConfigImageAnimator = std::function<void(const RefPtr<ImageAnimatorComponent>&)>;
ConfigImageAnimator g_configImageAnimator;

} // namespace

class ImageAnimatorTest : public testing::Test {
public:
    static void SetUpTestCase() {}

    static void TearDownTestCase() {}

    void SetUp() override
    {
        ImageProperties image1 = { "image1", Dimension(220), Dimension(220), Dimension(100), Dimension(100) };
        ImageProperties image2 = { "image2", Dimension(240), Dimension(240), Dimension(110), Dimension(110) };
        ImageProperties image3 = { "image3", Dimension(260), Dimension(260), Dimension(120), Dimension(120) };
        g_images.emplace_back(image1);
        g_images.emplace_back(image2);
        g_images.emplace_back(image3);
        ImageProperties secondImage1 = { "image1", Dimension(220), Dimension(220), Dimension(100), Dimension(100), 30 };
        ImageProperties secondImage2 = { "image2", Dimension(240), Dimension(240), Dimension(110), Dimension(110), 30 };
        ImageProperties secondImage3 = { "image3", Dimension(260), Dimension(260), Dimension(120), Dimension(120), 30 };
        g_secondImages.emplace_back(secondImage1);
        g_secondImages.emplace_back(secondImage2);
        g_secondImages.emplace_back(secondImage3);
        ImageProperties thirdImage1 = { "image1", Dimension(220), Dimension(220), Dimension(100), Dimension(100) };
        ImageProperties thirdImage2 = { "image2", Dimension(240), Dimension(240), Dimension(110), Dimension(110), 30 };
        ImageProperties thirdImage3 = { "image3", Dimension(260), Dimension(260), Dimension(120), Dimension(120), 30 };
        g_thirdImages.emplace_back(thirdImage1);
        g_thirdImages.emplace_back(thirdImage2);
        g_thirdImages.emplace_back(thirdImage3);
    }

    void TearDown() override
    {
        g_images.clear();
        g_secondImages.clear();
        g_thirdImages.clear();
    }

    void InitImageAnimatorComponent()
    {
        std::unique_ptr<PlatformWindow> platformWindow = TweenTestUtils::CreatePlatformWindow();
        platformWindowRaw_ = reinterpret_cast<MockPlatformWindow*>(platformWindow.get());
        auto window = TweenTestUtils::CreateWindow(std::move(platformWindow));
        auto taskExecutor = AceType::MakeRefPtr<FakeTaskExecutor>();
        auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
        auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
        RefPtr<Frontend> frontend = Frontend::CreateDefault();
        context_ = AceType::MakeRefPtr<PipelineContext>(
            std::move(window), taskExecutor, assetManager, resRegister, frontend, 0);
        context_->SetTimeProvider(
            [this] { return this->platformWindowRaw_->GetCurrentTimestampNano() + NANOSECOND_TO_MILLISECOND * 10; });

        imageAnimatorComponent_ = AceType::MakeRefPtr<ImageAnimatorComponent>("ImageAnimatorTest");
        if (g_configImageAnimator) {
            g_configImageAnimator(imageAnimatorComponent_);
        }
        auto pageComponent = AceType::MakeRefPtr<PageComponent>(0, "", imageAnimatorComponent_);
        context_->SetupRootElement();
        context_->PushPage(pageComponent);

        context_->OnSurfaceChanged(TEST_SURFACE_WIDTH, TEST_SURFACE_HEIGHT);
        platformWindowRaw_->SetNanoFrameTime(NANOSECOND_TO_MILLISECOND * frameTimeMs_);

        GetImageAnimatorElement();
    }

    void GetImageAnimatorElement()
    {
        EXPECT_TRUE(context_);
        auto stageElement = context_->GetStageElement();
        EXPECT_TRUE(stageElement);
        auto displayElement = stageElement->GetFirstChild();
        EXPECT_TRUE(displayElement);
        auto pageElement = displayElement->GetFirstChild();
        EXPECT_TRUE(pageElement);
        imageAnimatorElement_ = AceType::DynamicCast<ImageAnimatorElement>(pageElement->GetFirstChild());
        EXPECT_TRUE(imageAnimatorElement_);
    }

protected:
    RefPtr<ImageAnimatorElement> imageAnimatorElement_;
    RefPtr<ImageAnimatorComponent> imageAnimatorComponent_;
    RefPtr<PipelineContext> context_;
    uint32_t frameTimeMs_ = FRAME_TIME_IN_MILLISECOND;
    MockPlatformWindow* platformWindowRaw_ = nullptr;
};

/**
 * @tc.name: ImageAnimatorTest001
 * @tc.desc: test image animator element(go forward).
 * @tc.type: FUNC
 * @tc.require: AR000E3DMG
 * @tc.author: jiangdayuan
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(1);
        // go forward.
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_images);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. get the init value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(220, render->GetWidth());
    EXPECT_EQ(220, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames to let image animation go
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest002
 * @tc.desc: test image animator element(go back).
 * @tc.type: FUNC
 * @tc.require: AR000E3DMG
 * @tc.author: jiangdayuan
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(1);
        imageAnimator->SetIsFixedSize(false);
        // go back.
        imageAnimator->SetIsReverse(true);
        imageAnimator->SetImageProperties(g_images);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. get the init value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames to let image animation go
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest003
 * @tc.desc: test image animator element(infinite play).
 * @tc.type: FUNC
 * @tc.require: AR000E3DMG
 * @tc.author: jiangdayuan
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        // infinite repeat
        imageAnimator->SetIteration(-1);
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_images);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. trigger frames and get the first repeat time value
     * @tc.expected: step2. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames and get the second repeat time value
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest004
 * @tc.desc: test image animator element(go forward).
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(1);
        // go forward.
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_secondImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. get the init value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(220, render->GetWidth());
    EXPECT_EQ(220, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames to let image animation go
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(220, render->GetWidth());
    EXPECT_EQ(220, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest005
 * @tc.desc: test image animator element(go back).
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(1);
        imageAnimator->SetIsFixedSize(false);
        // go back.
        imageAnimator->SetIsReverse(true);
        imageAnimator->SetImageProperties(g_secondImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. get the init value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames to let image animation go
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest006
 * @tc.desc: test image animator element(infinite play).
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest006, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        // infinite repeat
        imageAnimator->SetIteration(-1);
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_secondImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. trigger frames and get the first repeat time value
     * @tc.expected: step2. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(220, render->GetWidth());
    EXPECT_EQ(220, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames and get the second repeat time value
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest007
 * @tc.desc: test image animator element(go forward).
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest007, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(1);
        // go forward.
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_thirdImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. get the init value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames to let image animation go
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest008
 * @tc.desc: test image animator element(go back).
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest008, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(1);
        imageAnimator->SetIsFixedSize(false);
        // go back.
        imageAnimator->SetIsReverse(true);
        imageAnimator->SetImageProperties(g_thirdImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. get the init value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames to let image animation go
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest009
 * @tc.desc: test image animator element(infinite play).
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest009, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        // infinite repeat
        imageAnimator->SetIteration(-1);
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_thirdImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. trigger frames and get the first repeat time value
     * @tc.expected: step2. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames and get the second repeat time value
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());
}

/**
 * @tc.name: ImageAnimatorTest010
 * @tc.desc: test image animator paly duration.
 * @tc.type: FUNC
 * @tc.require: AR000FL0UC
 * @tc.author: jiachunhui
 */
HWTEST_F(ImageAnimatorTest, ImageAnimatorTest010, TestSize.Level1)
{
    /**
     * @tc.steps: step1. init ImageAnimator component
     */
    g_configImageAnimator = [](const RefPtr<ImageAnimatorComponent>& imageAnimator) {
        std::vector<RefPtr<Component>> images;
        imageAnimator->SetDuration(20);
        imageAnimator->SetIteration(-1);
        imageAnimator->SetIsReverse(false);
        imageAnimator->SetIsFixedSize(false);
        imageAnimator->SetImageProperties(g_thirdImages);
    };
    InitImageAnimatorComponent();

    /**
     * @tc.steps: step2. trigger frames and get the first repeat time value
     * @tc.expected: step2. value is correct
     */
    auto box = imageAnimatorElement_->GetFirstChild();
    EXPECT_TRUE(box);
    auto image = box->GetFirstChild();
    EXPECT_TRUE(image);
    auto renderImage = image->GetRenderNode();
    EXPECT_TRUE(renderImage);
    auto render = AceType::DynamicCast<MockRenderImage>(renderImage);
    EXPECT_TRUE(render);
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames and get the second repeat time value
     * @tc.expected: step3. value is correct
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    EXPECT_EQ(260, render->GetWidth());
    EXPECT_EQ(260, render->GetHeight());

    /**
     * @tc.steps: step3. trigger frames six times and get the second repeat time value
     * @tc.expected: step3. if value is same as first frame value, the duration is 60.
     */
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    platformWindowRaw_->TriggerOneFrame();
    // verify the animation duration is 60, not 20.
    EXPECT_EQ(240, render->GetWidth());
    EXPECT_EQ(240, render->GetHeight());
}

} // namespace OHOS::Ace
