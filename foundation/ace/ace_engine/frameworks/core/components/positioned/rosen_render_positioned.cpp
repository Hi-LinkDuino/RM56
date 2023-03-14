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

#include "core/components/positioned/rosen_render_positioned.h"

#include "render_service_client/core/ui/rs_node.h"

namespace OHOS::Ace {

void RosenRenderPositioned::Update(const RefPtr<Component>& component)
{
    RenderPositioned::Update(component);
    auto context = context_.Upgrade();
    auto rsNode = GetRSNode();
    if (context && rsNode) {
        auto dipScale = context->GetDipScale();
        if (hasLeft_) {
            rsNode->SetFramePositionX(left_.ConvertToPx(dipScale));
        }
        if (hasTop_) {
            rsNode->SetFramePositionY(top_.ConvertToPx(dipScale));
        }
    }
}

} // namespace OHOS::Ace