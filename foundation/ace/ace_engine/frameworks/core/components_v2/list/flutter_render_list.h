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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_FLUTTER_RENDER_LIST_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_FLUTTER_RENDER_LIST_H

#include "base/utils/noncopyable.h"
#include "core/components_v2/list/render_list.h"
#include "core/pipeline/base/render_context.h"
#include "core/pipeline/layers/clip_layer.h"

namespace OHOS::Ace::V2 {

class FlutterRenderList : public RenderList {
    DECLARE_ACE_TYPE(V2::FlutterRenderList, RenderList);

public:
    FlutterRenderList() = default;
    ~FlutterRenderList() override = default;

    RenderLayer GetRenderLayer() override;
    bool IsRepaintBoundary() const override;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    RefPtr<Flutter::ClipLayer> layer_;

    ACE_DISALLOW_COPY_AND_MOVE(FlutterRenderList);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_LIST_FLUTTER_RENDER_LIST_H
