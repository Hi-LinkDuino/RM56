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

#include "core/components/clip/flutter_render_clip.h"

#include "third_party/skia/include/core/SkScalar.h"

namespace OHOS::Ace {

using namespace Flutter;

void FlutterRenderClip::Paint(RenderContext& context, const Offset& offset)
{
    RRect rrect;
    Corner corner = {
        Radius(NormalizeToPx(topLeftRadius_.GetX()), NormalizeToPx(topLeftRadius_.GetY())),
        Radius(NormalizeToPx(topRightRadius_.GetX()), NormalizeToPx(topRightRadius_.GetY())),
        Radius(NormalizeToPx(bottomRightRadius_.GetX()), NormalizeToPx(bottomRightRadius_.GetY())),
        Radius(NormalizeToPx(bottomLeftRadius_.GetX()), NormalizeToPx(bottomLeftRadius_.GetY()))
    };
    rrect.SetCorner(corner);
    rrect.SetRect(GetClipRect(offset));
    layer_->SetClipRRect(rrect, Clip::HARD_EDGE);

    RenderNode::Paint(context, offset);
}

RenderLayer FlutterRenderClip::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(
            0, GetLayoutParam().GetMaxSize().Width(), 0, GetLayoutParam().GetMaxSize().Height(), Clip::HARD_EDGE);
    }
    return AceType::RawPtr(layer_);
}

} // namespace OHOS::Ace
