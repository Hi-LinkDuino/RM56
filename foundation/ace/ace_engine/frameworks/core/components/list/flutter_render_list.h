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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_FLUTTER_RENDER_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_FLUTTER_RENDER_LIST_H

#include "core/components/list/render_list.h"
#include "core/pipeline/layers/clip_layer.h"

namespace OHOS::Ace {

class FlutterRenderList : public RenderList {
    DECLARE_ACE_TYPE(FlutterRenderList, RenderList);

public:
    FlutterRenderList() = default;
    ~FlutterRenderList() override = default;

    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;
    void PaintChild(const RefPtr<RenderNode>& child, RenderContext& context, const Offset& offset) override;
    void PaintStickyItem(RenderContext& context, const Offset& offset);

    bool IsRepaintBoundary() const override
    {
        return true;
    }

private:
    RefPtr<Flutter::ClipLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_LIST_FLUTTER_RENDER_LIST_H
