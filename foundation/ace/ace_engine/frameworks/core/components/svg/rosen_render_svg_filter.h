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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_FILTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_FILTER_H

#include "third_party/skia/include/core/SkImageFilter.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "frameworks/core/components/svg/render_svg_fe.h"
#include "frameworks/core/components/svg/render_svg_filter.h"

namespace OHOS::Ace {

class RosenRenderSvgFilter : public RenderSvgFilter {
    DECLARE_ACE_TYPE(RosenRenderSvgFilter, RenderSvgFilter)

public:
    void Paint(RenderContext& context, const Offset& offset) override;
    const SkPaint OnAsPaint();

protected:
    void GetImageFilter(
        const RefPtr<RenderSvgFe>& fe, sk_sp<SkImageFilter>& imageFilter, ColorInterpolationType& currentColor);
    void InitFilterColor(const RefPtr<RenderSvgFe>& fe, ColorInterpolationType& currentColor);
    sk_sp<SkImageFilter> MakeImageFilter(const FeInType& in, sk_sp<SkImageFilter>& imageFilter);
    void ConverImageFilterColor(
        sk_sp<SkImageFilter>& imageFilter, const ColorInterpolationType& src, const ColorInterpolationType& dst);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_FILTER_H
