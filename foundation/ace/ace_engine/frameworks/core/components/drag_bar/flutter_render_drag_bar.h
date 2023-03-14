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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_FLUTTER_RENDER_DRAG_BAR_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_FLUTTER_RENDER_DRAG_BAR_H

#include "base/geometry/offset.h"
#include "core/components/drag_bar/render_drag_bar.h"
#include "core/pipeline/base/flutter_render_context.h"
#include "core/pipeline/base/render_context.h"
#include "core/pipeline/layers/container_layer.h"

namespace OHOS::Ace {

class FlutterRenderDragBar final : public RenderDragBar {
    DECLARE_ACE_TYPE(FlutterRenderDragBar, RenderDragBar);

public:
    FlutterRenderDragBar() = default;
    ~FlutterRenderDragBar() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;

    bool IsRepaintBoundary() const override
    {
        return true;
    }

    RenderLayer GetRenderLayer() override;

private:
    RefPtr<Flutter::ContainerLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DRAG_BAR_FLUTTER_RENDER_DRAG_BAR_H
