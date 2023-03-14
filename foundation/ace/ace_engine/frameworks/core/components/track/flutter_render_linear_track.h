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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_FLUTTER_RENDER_LINEAR_TRACK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_FLUTTER_RENDER_LINEAR_TRACK_H

#include "base/geometry/offset.h"
#include "core/components/track/render_track.h"
#include "core/pipeline/base/render_context.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

class FlutterRenderLinearTrack : public RenderTrack {
    DECLARE_ACE_TYPE(FlutterRenderLinearTrack, RenderTrack);

public:
    FlutterRenderLinearTrack() = default;
    ~FlutterRenderLinearTrack() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PaintSliderTrack(RenderContext& context, const Offset& offset);
    void PaintSliderSteps(RenderContext& context, const Offset& offset);
    void PaintBackgroundTrack(const ScopedCanvas& canvas, const Offset& offset, double trackHeight) const;

private:
    sk_sp<SkShader> BlendSkShader(const SkPoint pts, const SkColor color, bool useAnimator = false);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TRACK_FLUTTER_RENDER_LINEAR_TRACK_H
