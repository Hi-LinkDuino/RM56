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

#include "core/components/progress/bubble_progress_component.h"

#include "core/components/progress/bubble_progress_element.h"
#include "core/components/progress/render_bubble_progress.h"

namespace OHOS::Ace {

RefPtr<RenderNode> BubbleProgressComponent::CreateRenderNode()
{
    return RenderBubbleProgress::Create();
}

RefPtr<Element> BubbleProgressComponent::CreateElement()
{
    return AceType::MakeRefPtr<BubbleProgressElement>();
}

} // namespace OHOS::Ace