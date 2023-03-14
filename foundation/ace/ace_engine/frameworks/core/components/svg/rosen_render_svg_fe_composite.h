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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_FE_COMPOSITE_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_FE_COMPOSITE_H

#include "frameworks/core/components/svg/render_svg_fe_composite.h"
#include "frameworks/core/components/svg/rosen_render_svg_fe.h"

namespace OHOS::Ace {

class RosenRenderSvgFeComposite : public RenderSvgFeComposite, RosenRenderSvgFe {
    DECLARE_ACE_TYPE(RosenRenderSvgFeComposite, RenderSvgFeComposite, RosenRenderSvgFe);

public:
    void OnAsImageFilter(const sk_sp<SkImageFilter>& backImageFilter, const sk_sp<SkImageFilter>& foreImageFilter,
        sk_sp<SkImageFilter>& imageFilter) const override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_ROSEN_RENDER_SVG_FE_COMPOSITE_H
