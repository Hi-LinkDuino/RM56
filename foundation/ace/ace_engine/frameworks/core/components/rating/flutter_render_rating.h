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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_FLUTTER_RENDER_RATING_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_FLUTTER_RENDER_RATING_H

#include "core/components/rating/render_rating.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

class FlutterRenderRating final : public RenderRating {
    DECLARE_ACE_TYPE(FlutterRenderRating, RenderRating);

public:
    FlutterRenderRating() = default;
    ~FlutterRenderRating() override = default;

    void Paint(RenderContext& context, const Offset& offset) override;
    void PaintFocus(const Offset& offset, double rrectRadius, const Size& boardSize, RenderContext& context) override;
    void PaintRatingBar(RenderContext& context, const ScopedCanvas& canvas);
    void PaintImageArea(RenderContext& context, const RefPtr<RenderImage>& renderImage, const ScopedCanvas& canvas,
        const ImageAreaProperties& properties);
    void PaintHoverRect(const ScopedCanvas& canvas);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_RATING_FLUTTER_RENDER_RATING_H
