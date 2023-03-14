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

#include "core/components/test/unittest/display/display_test_utils.h"

#include "adapter/aosp/entrance/java/jni/ace_application_info.h"
#include "core/components/box/box_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/test/json/json_frontend.h"
#include "core/components/text/text_component.h"
#include "core/mock/fake_asset_manager.h"
#include "core/mock/fake_task_executor.h"
#include "core/mock/mock_resource_register.h"

namespace OHOS::Ace {
namespace {

constexpr double RECT_WIDTH = 1080.0;
constexpr double RECT_HEIGHT = 2244.0;

} // namespace

RefPtr<RenderRoot> DisplayTestUtils::CreateRenderRoot()
{
    RefPtr<RenderRoot> root = AceType::MakeRefPtr<MockRenderRoot>();
    const Rect paintRect(0.0f, 0.0f, RECT_WIDTH, RECT_HEIGHT);
    root->SetPaintRect(paintRect);
    return root;
}

RefPtr<RenderFlex> DisplayTestUtils::CreateRenderFlex(
    const FlexDirection direction, const FlexAlign main, const FlexAlign cross)
{
    RefPtr<RenderFlex> flex = AceType::MakeRefPtr<RenderFlex>();
    std::list<RefPtr<Component>> children;
    if (direction == FlexDirection::ROW) {
        RefPtr<RowComponent> flexComponent = AceType::MakeRefPtr<RowComponent>(main, cross, children);
        flex->Update(flexComponent);
        return flex;
    } else {
        RefPtr<ColumnComponent> flexComponent = AceType::MakeRefPtr<ColumnComponent>(main, cross, children);
        flex->Update(flexComponent);
        return flex;
    }
}

RefPtr<RenderBox> DisplayTestUtils::CreateRenderBox(const double width, const double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<MockRenderDisplay> DisplayTestUtils::CreateRenderDisplay(const VisibleType visible, const double opacity)
{
    RefPtr<MockRenderDisplay> renderDisplay = AceType::MakeRefPtr<MockRenderDisplay>();
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<DisplayComponent> displayComponent = AceType::MakeRefPtr<DisplayComponent>(text);
    displayComponent->SetVisible(visible);
    displayComponent->SetOpacity(opacity);
    renderDisplay->Update(displayComponent);
    return renderDisplay;
}

RefPtr<PipelineContext> DisplayTestUtils::GetMockContext()
{
    auto platformWindow = std::make_unique<MockWindow>(nullptr);
    auto window = std::make_unique<Window>(std::move(platformWindow));
    auto taskExecutor = Referenced::MakeRefPtr<FakeTaskExecutor>();
    auto assetManager = Referenced::MakeRefPtr<FakeAssetManager>();
    auto resRegister = Referenced::MakeRefPtr<MockResourceRegister>();
    auto fakeFrontend = Frontend::CreateDefault();
    return AceType::MakeRefPtr<PipelineContext>(
        std::move(window), taskExecutor, assetManager, resRegister, fakeFrontend, 0);
}

} // namespace OHOS::Ace
