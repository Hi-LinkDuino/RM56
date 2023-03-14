/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/focus_collaboration/render_focus_collaboration.h"

namespace OHOS::Ace {

RefPtr<RenderNode> RenderFocusCollaboration::Create()
{
    return AceType::MakeRefPtr<RenderFocusCollaboration>();
}

void RenderFocusCollaboration::PerformLayout()
{
    if (!GetChildren().empty()) {
        for (const auto& child : GetChildren()) {
            child->Layout(GetLayoutParam());
        }
        SetLayoutSize(GetChildren().front()->GetLayoutSize());
    }
}

} // namespace OHOS::Ace
