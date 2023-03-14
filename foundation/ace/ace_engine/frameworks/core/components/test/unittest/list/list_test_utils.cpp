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

#include "core/components/test/unittest/list/list_test_utils.h"

#include "core/components/box/box_component.h"
#include "core/components/list/list_item_group_component.h"

namespace OHOS::Ace {

RefPtr<RenderBox> ListTestUtils::CreateRenderBox(double width, double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<RenderNode> ListTestUtils::CreateRenderItem(
    double width, double height, const RefPtr<PipelineContext>& context, bool primary)
{
    RefPtr<RenderBox> parent = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<MockRenderListItem> listItem = AceType::MakeRefPtr<MockRenderListItem>();
    listItem->Attach(context);
    listItem->SetPrimary(primary);
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<Decoration> decoration = AceType::MakeRefPtr<Decoration>();
    decoration->SetBorderRadius(Radius(10.0));
    renderBox->SetBackDecoration(decoration);
    parent->Attach(context);
    renderBox->Attach(context);
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    parent->AddChild(listItem);
    listItem->AddChild(renderBox);
    return parent;
}

RefPtr<RenderNode> ListTestUtils::CreateRenderItemGroup(
    double width, double height, const RefPtr<PipelineContext>& context, bool needItem, bool specifyPrimary)
{
    RefPtr<RenderBox> parent = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<MockRenderListItemGroup> listItemGroup = AceType::MakeRefPtr<MockRenderListItemGroup>();
    listItemGroup->SetOnClick(
        AceAsyncEvent<void(const std::string&)>::Create(EventMarker { "groupclick_id" }, context));
    listItemGroup->SetOnCollapse(
        AceAsyncEvent<void(const std::string&)>::Create(EventMarker { "groupcollapse_id" }, context));
    listItemGroup->SetOnExpand(
        AceAsyncEvent<void(const std::string&)>::Create(EventMarker { "groupexpand_id" }, context));
    listItemGroup->SetGroupId("test");
    listItemGroup->SetExpand(true);
    auto primaryItem = CreateRenderItem(width, height, context, specifyPrimary);
    listItemGroup->AddChild(primaryItem);
    auto arrowItem = CreateRenderBox(width, height);
    listItemGroup->AddChild(arrowItem); // Add arrow child.
    if (needItem) {
        auto secondLevelItem = CreateRenderItem(width, height, context, false);
        listItemGroup->AddChild(secondLevelItem);
    }
    listItemGroup->Attach(context);
    parent->Attach(context);
    parent->AddChild(listItemGroup);
    return parent;
}

} // namespace OHOS::Ace
