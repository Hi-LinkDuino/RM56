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

#include "frameworks/core/components/svg/flutter_render_svg_use.h"

#include "frameworks/core/components/transform/flutter_render_transform.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

using namespace Flutter;

RenderLayer FlutterRenderSvgUse::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderSvgUse::Paint(RenderContext& context, const Offset& offset)
{
    if (GreatNotEqual(x_.Value(), 0.0) || GreatNotEqual(y_.Value(), 0.0)) {
        if (GetRenderLayer()) {
            auto transform = Matrix4::CreateTranslate(ConvertDimensionToPx(x_, LengthType::HORIZONTAL),
                ConvertDimensionToPx(y_, LengthType::VERTICAL), 0.0f);
            if (!transformLayer_) {
                transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
            }
            transformLayer_->Update(transform);
        }
    }
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(GetTransformMatrix4());
    }

    RenderNode::Paint(context, offset);
}

void FlutterRenderSvgUse::OnGlobalPositionChanged()
{
    if (transformLayer_ && NeedTransform()) {
        transformLayer_->Update(UpdateTransformMatrix4());
    }
    RenderNode::OnGlobalPositionChanged();
}

} // namespace OHOS::Ace
