/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "flutter_render_texttimer.h"

namespace OHOS::Ace {
RenderLayer FlutterRenderTextTimer::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<Flutter::GradientLayer>();
    }

    return AceType::RawPtr(layer_);
}

void FlutterRenderTextTimer::Paint(RenderContext& context, const Offset& offset)
{
    if (layer_) {
        layer_->SetOffset(offset.GetX(), offset.GetY());
    }
    RenderNode::Paint(context, offset);
}
} // namespace OHOS::Ace
