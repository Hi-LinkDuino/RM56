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

#include "core/components/test/unittest/wrap/wrap_test_utils.h"

#include "core/components/box/box_component.h"

namespace OHOS::Ace {

RefPtr<RenderRoot> WrapTestUtils::CreateRenderRoot()
{
    RefPtr<RenderRoot> root = AceType::MakeRefPtr<MockRenderRoot>();
    const Rect wholeScreenRect = Rect(0.0, 0.0, SCREEN_WIDTH, SCREEN_HEIGHT);
    root->SetPaintRect(wholeScreenRect);
    return root;
}

RefPtr<RenderWrap> WrapTestUtils::CreateRenderWrap(
    const RefPtr<WrapComponent>& component, const RefPtr<PipelineContext>& context)
{
    RefPtr<RenderWrap> wrapRenderNode = AceType::MakeRefPtr<RenderWrap>();
    wrapRenderNode->Attach(context);
    wrapRenderNode->Update(component);
    return wrapRenderNode;
}

RefPtr<RenderBox> WrapTestUtils::CreateRenderBox(double width, double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    return renderBox;
}

} // namespace OHOS::Ace