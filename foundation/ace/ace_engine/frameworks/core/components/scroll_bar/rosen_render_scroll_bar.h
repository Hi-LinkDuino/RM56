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

#include "flutter/lib/ui/ui_dart_state.h"

#include "core/components/scroll_bar/render_scroll_bar.h"
#include "core/pipeline/base/rosen_render_context.h"

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_ROSEN_RENDER_SCROLL_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_ROSEN_RENDER_SCROLL_BAR_H

namespace OHOS::Ace {

class RosenRenderScrollBar : public RenderScrollBar {
    DECLARE_ACE_TYPE(RosenRenderScrollBar, RenderScrollBar);

public:
    RosenRenderScrollBar() = default;
    ~RosenRenderScrollBar() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_ROSEN_RENDER_SCROLL_BAR_H
