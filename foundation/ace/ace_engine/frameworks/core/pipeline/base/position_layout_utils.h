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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_POSITION_LAYOUT_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_POSITION_LAYOUT_UTILS_H

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

class PositionLayoutUtils final {
public:
    static Offset GetAbsoluteOffset(const RefPtr<RenderNode>& parentNode, const RefPtr<RenderNode>& selfNode)
    {
        if (!parentNode || !selfNode) {
            return Offset::Zero();
        }
        double deltaX = 0.0;
        if (selfNode->HasLeft()) {
            deltaX = selfNode->NormalizePercentToPx(selfNode->GetLeft(), false);
        } else if (selfNode->HasRight()) {
            deltaX = parentNode->GetLayoutSize().Width() - selfNode->NormalizePercentToPx(selfNode->GetRight(), false) -
                     selfNode->GetLayoutSize().Width();
        } else {
            deltaX = 0.0;
        }
        deltaX = deltaX - selfNode->NormalizePercentToPx(selfNode->GetAnchorX(), false, true);
        double deltaY = 0.0;
        if (selfNode->HasTop()) {
            deltaY = selfNode->NormalizePercentToPx(selfNode->GetTop(), true);
        } else if (selfNode->HasBottom()) {
            deltaY = parentNode->GetLayoutSize().Height() - selfNode->NormalizePercentToPx(selfNode->GetBottom(), true)
                - selfNode->GetLayoutSize().Height();
        } else {
            deltaY = 0.0;
        }
        deltaY = deltaY - selfNode->NormalizePercentToPx(selfNode->GetAnchorY(), true, true);
        return Offset(deltaX, deltaY);
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_POSITION_LAYOUT_UTILS_H
