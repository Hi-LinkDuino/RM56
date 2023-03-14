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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_GRID_POSITION_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_GRID_POSITION_CONTROLLER_H

#include "core/components/scroll/scroll_controller_interface.h"

namespace OHOS::Ace::V2 {

class ACE_EXPORT GridPositionController : public ScrollController {
    DECLARE_ACE_TYPE(GridPositionController, ScrollController);

public:
    GridPositionController() = default;
    ~GridPositionController() override = default;
    Axis GetScrollDirection() const override;

    void JumpTo(int32_t index, int32_t source = 3) override;
    bool AnimateTo(const Dimension& position, float duration, const RefPtr<Curve>& curve) override;
    void ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth) override;

    Offset GetCurrentOffset() const override;
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_GRID_GRID_POSITION_CONTROLLER_H
