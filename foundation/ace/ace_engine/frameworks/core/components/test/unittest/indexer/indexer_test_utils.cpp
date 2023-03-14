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

#include "core/components/test/unittest/indexer/indexer_test_utils.h"

#include "core/components/box/box_component.h"

namespace OHOS::Ace {

RefPtr<RenderBox> IndexerTestUtils::CreateRenderBox(double width, double height)
{
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<BoxComponent> boxComponent = AceType::MakeRefPtr<BoxComponent>();
    boxComponent->SetWidth(width);
    boxComponent->SetHeight(height);
    renderBox->Update(boxComponent);
    return renderBox;
}

RefPtr<RenderNode> IndexerTestUtils::CreateRenderItem(
    double width, double height, const RefPtr<PipelineContext>& context)
{
    RefPtr<RenderBox> parent = AceType::MakeRefPtr<MockRenderBox>();
    RefPtr<RenderListItem> listItem = AceType::MakeRefPtr<MockRenderListItem>();
    RefPtr<RenderBox> renderBox = AceType::MakeRefPtr<MockRenderBox>();
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

} // namespace OHOS::Ace
