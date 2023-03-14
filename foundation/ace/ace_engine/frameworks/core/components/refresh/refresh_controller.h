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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_CONTROLLER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_CONTROLLER_H

#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class RefreshStatus {
    // The default status.
    INACTIVE = 0,
    // While being dragged but not enough to trig refresh.
    DRAG,
    // Dragging enough to refresh, and less than the max distance.
    OVER_DRAG,
    // While it is refreshing.
    REFRESH,
    // While it will scroll back to the top after refreshing.
    DONE,
};

class RefreshController : public AceType {
    DECLARE_ACE_TYPE(RefreshController, AceType);

public:
    RefreshController() = default;
    ~RefreshController() override = default;

    void SetRefresh(const WeakPtr<RenderNode>& refresh)
    {
        refresh_ = refresh;
    }

private:
    WeakPtr<RenderNode> refresh_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_REFRESH_REFRESH_CONTROLLER_H
