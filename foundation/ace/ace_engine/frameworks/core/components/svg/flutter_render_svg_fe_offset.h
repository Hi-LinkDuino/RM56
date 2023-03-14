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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_FLUTTER_RENDER_SVG_FE_OFFSET_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_FLUTTER_RENDER_SVG_FE_OFFSET_H

#include "frameworks/core/components/svg/flutter_render_svg_fe.h"
#include "frameworks/core/components/svg/render_svg_fe_offset.h"

namespace OHOS::Ace {

class FlutterRenderSvgFeOffset : public RenderSvgFeOffset, FlutterRenderSvgFe {
    DECLARE_ACE_TYPE(FlutterRenderSvgFeOffset, RenderSvgFeOffset, FlutterRenderSvgFe);

public:
    void OnAsImageFilter(sk_sp<SkImageFilter>& imageFilter) const override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SVG_FLUTTER_RENDER_SVG_FE_OFFSET_H
