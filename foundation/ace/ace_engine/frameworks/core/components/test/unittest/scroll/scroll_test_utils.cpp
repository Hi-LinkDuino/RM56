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

#include "core/components/box/box_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/test/unittest/scroll/scroll_test_utils.h"

namespace OHOS::Ace {

RefPtr<RenderFlex> ScrollTestUtils::CreateRenderFlex(FlexDirection direction, FlexAlign main, FlexAlign cross)
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

RefPtr<RenderBox> ScrollTestUtils::CreateRenderBox(double width, double height, bool deliverMinToChild)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    boxComponent->SetDeliverMinToChild(deliverMinToChild);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<BoxComponent> ScrollTestUtils::CreateBoxComponent(double width, double height, bool deliverMinToChild)
{
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    boxComponent->SetDeliverMinToChild(deliverMinToChild);
    return boxComponent;
}

} // namespace OHOS::Ace