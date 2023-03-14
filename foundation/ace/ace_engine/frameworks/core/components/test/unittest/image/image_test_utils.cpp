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

#include "core/components/test/unittest/image/image_test_utils.h"

#include "gtest/gtest.h"

#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "core/animation/animator.h"
#include "core/animation/card_transition_controller.h"
#include "core/animation/shared_transition_controller.h"
#include "core/components/align/render_align.h"
#include "core/components/image/image_component.h"
#include "core/components/test/json/json_frontend.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"

namespace OHOS::Ace {

void VerifyImageFit(ImageFitConfig&& imageFitConfig, const Rect& expectSrcRect, const Rect& expectDstRect)
{
    /**
     * @tc.steps: step1. Create an object of ImageComponent and set expected properties.
     */
    RefPtr<ImageComponent> image = AceType::MakeRefPtr<ImageComponent>("");
    image->SetAlignment(imageFitConfig.alignment);
    image->SetImageFit(imageFitConfig.imageFit);

    /**
     * @tc.steps: step2. Create an object of FlutterRenderImage initialized with ImageComponent and then set properties.
     */
    RefPtr<RenderImage> renderImage = AceType::MakeRefPtr<MockRenderImage>();
    auto mockContext = GetMockContext();
    renderImage->Attach(mockContext);
    renderImage->Update(image);
    renderImage->SetLayoutSize(imageFitConfig.dstRect.GetSize());
    renderImage->ApplyImageFit(imageFitConfig.srcRect, imageFitConfig.dstRect);

    /**
     * @tc.steps: step3. Verify that the properties are calculated correctly.
     * @tc.expected: step3. Properties are calculated correctly.
     */
    ASSERT_TRUE(imageFitConfig.srcRect == expectSrcRect);
    ASSERT_TRUE(imageFitConfig.dstRect == expectDstRect);
}

void VerifyRectSize(const std::list<Rect>& rectList, const Size& expectRectSize)
{
    for (const auto& rect : rectList) {
        ASSERT_TRUE(rect.GetSize() == expectRectSize);
    }
}

RefPtr<RenderContext> RenderContext::Create()
{
    return AceType::MakeRefPtr<MockRenderContext>();
}

RefPtr<RenderNode> RenderImage::Create()
{
    return AceType::MakeRefPtr<MockRenderImage>();
}

std::unique_ptr<PlatformWindow> PlatformWindow::Create(AceView* aceView)
{
    return std::make_unique<MockWindow>(aceView);
}

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

RefPtr<RenderRoot> CreateRenderRoot(const Size& maxSize)
{
    RefPtr<RenderRoot> root = AceType::MakeRefPtr<MockRenderRoot>();
    const Rect paintRect(Offset(), maxSize);
    root->SetPaintRect(paintRect);
    return root;
}

RefPtr<RenderImage> CreateRenderImage(double width, double height, bool fitMaxSize)
{
    RefPtr<RenderImage> renderImage = AceType::MakeRefPtr<MockRenderImage>();
    RefPtr<ImageComponent> imageComponent = AceType::MakeRefPtr<ImageComponent>("");
    imageComponent->SetWidth(Dimension(width));
    imageComponent->SetHeight(Dimension(height));
    imageComponent->SetFitMaxSize(fitMaxSize);
    auto mockContext = GetMockContext();
    renderImage->Attach(mockContext);
    renderImage->Update(imageComponent);
    return renderImage;
}

RefPtr<PipelineContext> GetMockContext()
{
    auto platformWindow = PlatformWindow::Create(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = Frontend::CreateDefault();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);
}

SharedTransitionController::SharedTransitionController(const WeakPtr<PipelineContext>& context) {}

CardTransitionController::CardTransitionController(const WeakPtr<PipelineContext>& context) {}

RRect CardTransitionController::GetCardRect(const ComposeId& composeId) const
{
    return RRect();
}

} // namespace OHOS::Ace
