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

#include "core/components/bubble/bubble_component.h"

#include "base/geometry/dimension.h"
#include "core/components/box/box_component.h"
#include "core/components/bubble/bubble_element.h"
#include "core/components/bubble/render_bubble.h"

namespace OHOS::Ace {
RefPtr<Element> BubbleComponent::CreateElement()
{
    return AceType::MakeRefPtr<BubbleElement>();
}

RefPtr<RenderNode> BubbleComponent::CreateRenderNode()
{
    return RenderBubble::Create();
}

} // namespace OHOS::Ace