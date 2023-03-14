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

#include "core/components/clip/rosen_render_clip.h"

#include "render_service_client/core/ui/rs_node.h"
#include "third_party/skia/include/core/SkScalar.h"

#include "core/pipeline/base/rosen_render_context.h"

namespace OHOS::Ace {

void RosenRenderClip::SetOffsetX(double offsetX)
{
    RenderClip::SetOffsetX(offsetX);
    SyncGeometryProperties();
}

void RosenRenderClip::SetOffsetY(double offsetY)
{
    RenderClip::SetOffsetY(offsetY);
    SyncGeometryProperties();
}

void RosenRenderClip::SetWidth(double width)
{
    RenderClip::SetWidth(width);
    SyncGeometryProperties();
}

void RosenRenderClip::SetHeight(double height)
{
    RenderClip::SetHeight(height);
    SyncGeometryProperties();
}

void RosenRenderClip::SyncGeometryProperties()
{
    auto rsNode = GetRSNode();
    if (!rsNode) {
        return;
    }
    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    double dipScale = context->GetDipScale();
    rsNode->SetCornerRadius(topLeftRadius_.GetX().ConvertToPx(dipScale));
    rsNode->SetClipToBounds(true);

    Rect paintSize = GetTransitionPaintRect();
    Offset paintOffset = GetPaintOffset();
    Offset clipOffset = paintOffset + Offset(offsetX_, offsetY_);
    double clipWidth = NearZero(width_) ? paintSize.Width() : width_;
    double clipHeight = NearZero(height_) ? paintSize.Height() : height_;
    rsNode->SetBounds(clipOffset.GetX(), clipOffset.GetY(), clipWidth, clipHeight);
    if (IsTailRenderNode()) {
        rsNode->SetFrame(paintOffset.GetX(), paintOffset.GetY(), paintSize.Width(), paintSize.Height());
    }
}

} // namespace OHOS::Ace
