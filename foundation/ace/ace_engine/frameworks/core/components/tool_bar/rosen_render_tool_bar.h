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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_ROSEN_RENDER_TOOL_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_ROSEN_RENDER_TOOL_BAR_H

#include "core/components/tool_bar/render_tool_bar.h"

namespace OHOS::Ace {
class RosenRenderToolBar : public RenderToolBar {
    DECLARE_ACE_TYPE(RosenRenderToolBar, RenderToolBar);

public:
    void Update(const RefPtr<Component>& component) override;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TOOL_BAR_ROSEN_RENDER_TOOL_BAR_H