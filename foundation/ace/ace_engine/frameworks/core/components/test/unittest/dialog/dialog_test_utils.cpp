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

#include "core/components/test/unittest/dialog/dialog_test_utils.h"

#include "core/components/box/box_component.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {

RefPtr<MockRenderBubble> DialogTestUtils::CreateRenderBubble(
    const Placement placement, const Offset& offset, const Size& size)
{
    RefPtr<MockRenderBubble> renderBubble = AceType::MakeRefPtr<MockRenderBubble>();
    RefPtr<TextComponent> text = AceType::MakeRefPtr<TextComponent>("HiAce");
    RefPtr<BubbleComponent> bubbleComponent = AceType::MakeRefPtr<BubbleComponent>(text);
    renderBubble->Update(bubbleComponent);
    renderBubble->SetTargetOffset(offset);
    renderBubble->SetTargetSize(size);
    renderBubble->SetPlacement(placement);
    return renderBubble;
}

} // namespace OHOS::Ace
