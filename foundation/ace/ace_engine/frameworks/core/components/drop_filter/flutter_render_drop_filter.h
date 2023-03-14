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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_FLUTTER_RENDER_DROP_FILTER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_FLUTTER_RENDER_DROP_FILTER_H

#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/compositing/scene_builder.h"
#include "flutter/lib/ui/painting/image_filter.h"

#include "core/components/common/properties/color.h"
#include "core/components/drop_filter/render_drop_filter.h"
#include "core/pipeline/layers/dropfilter_layer.h"

namespace OHOS::Ace {

class FlutterRenderDropFilter final : public RenderDropFilter {
    DECLARE_ACE_TYPE(FlutterRenderDropFilter, RenderDropFilter);

public:
    FlutterRenderDropFilter() = default;
    ~FlutterRenderDropFilter() override = default;
    RenderLayer GetRenderLayer() override;
    void Paint(RenderContext& context, const Offset& offset) override;

private:
    fml::RefPtr<flutter::ImageFilter> imageFilter_ = flutter::ImageFilter::Create();
    RefPtr<Flutter::DropFilterLayer> layer_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DROP_FILTER_FLUTTER_RENDER_DROP_FILTER_H
