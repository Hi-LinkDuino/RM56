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

#include "core/components/test/unittest/tabbar/tabbar_test_utils.h"

#include "core/components/box/box_component.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {
namespace {

constexpr double RECT_WIDTH = 1080.0;
constexpr double RECT_HEIGHT = 2244.0;

} // namespace

RefPtr<RenderRoot> TabBarTestUtils::CreateRenderRoot()
{
    RefPtr<RenderRoot> root = AceType::MakeRefPtr<MockRenderRoot>();
    const Rect paintRect(0.0, 0.0, RECT_WIDTH, RECT_HEIGHT);
    root->SetPaintRect(paintRect);
    return root;
}

RefPtr<RenderBox> TabBarTestUtils::CreateRenderBox(const double width, const double height, BoxFlex flex)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    boxComponent->SetFlex(flex);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<MockRenderTabBar> TabBarTestUtils::CreateRenderTabBar()
{
    return nullptr;
}

} // namespace OHOS::Ace
