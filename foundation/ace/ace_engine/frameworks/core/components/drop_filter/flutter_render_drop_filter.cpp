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

#include "core/components/drop_filter/flutter_render_drop_filter.h"

#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/painting/canvas.h"
#include "flutter/lib/ui/painting/picture_recorder.h"
#include "flutter/lib/ui/ui_dart_state.h"

#include "base/log/dump_log.h"
#include "core/components/drop_filter/drop_filter_component.h"
#include "core/pipeline/base/scoped_canvas_state.h"

namespace OHOS::Ace {

RenderLayer FlutterRenderDropFilter::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::DropFilterLayer>(imageFilter_, sigmaX_, sigmaY_);
    }
    return AceType::RawPtr(layer_);
}

void FlutterRenderDropFilter::Paint(RenderContext& context, const Offset& offset)
{
    if (layer_) {
        layer_->SetDropFilter(imageFilter_, sigmaX_, sigmaY_);
    }
    RenderNode::Paint(context, offset);
}

} // namespace OHOS::Ace
