/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/components/xcomponent/flutter_render_xcomponent.h"

#include "base/log/ace_trace.h"
#include "base/log/dump_log.h"

namespace OHOS::Ace {
using namespace Flutter;

RenderLayer FlutterRenderXComponent::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(0.0, 0.0, 0.0, 0.0, Flutter::Clip::NONE);
    }

    return AceType::RawPtr(layer_);
}

void FlutterRenderXComponent::AddBackgroundLayer()
{
    if (!backgroundLayer_) {
        backgroundLayer_ = AceType::MakeRefPtr<PictureLayer>();
    }
    DrawBackground();
    layer_->AddChildren(backgroundLayer_);
}

void FlutterRenderXComponent::DrawBackground()
{
    const Size& layout = GetLayoutSize();
    fml::RefPtr<flutter::PictureRecorder> recorder;
    fml::RefPtr<flutter::Canvas> canvas;
    flutter::Paint paint;
    flutter::PaintData paintData;

    recorder = flutter::PictureRecorder::Create();
    canvas = flutter::Canvas::Create(recorder.get(), 0.0, 0.0, layout.Width(), layout.Height());
    paint.paint()->setColor(SK_ColorBLACK);
    paint.paint()->setAlpha(opacity_);
    canvas->drawRect(0.0, 0.0, layout.Width(), layout.Height(), paint, paintData);
    backgroundLayer_->SetPicture(recorder->endRecording());
}

void FlutterRenderXComponent::AddTextureLayer()
{
    const Size& layout = GetLayoutSize();
    if (!textureLayer_) {
        textureLayer_ = AceType::MakeRefPtr<TextureLayer>(textureId_, false, layout.Width(), layout.Height(), opacity_);
    } else {
        textureLayer_->Update(textureId_, false, layout.Width(), layout.Height(), opacity_);
    }

    textureLayer_->SetOffset(0, 0);
    layer_->AddChildren(textureLayer_);
}

void FlutterRenderXComponent::PerformLayout()
{
    RenderXComponent::PerformLayout();
}

void FlutterRenderXComponent::Paint(RenderContext& context, const Offset& offset)
{
    if (textureId_ != X_INVALID_ID) {
        const Size& layout = GetLayoutSize();
        layer_->SetClip(0.0, layout.Width(), 0.0, layout.Height(), Flutter::Clip::NONE);
        AddBackgroundLayer();

        if (!hasSetDefaultSize_) {
            if (xcomponentSizeChangeEvent_ && (!drawSize_.IsHeightInfinite())) {
                xcomponentSizeChangeEvent_(textureId_, drawSize_.Width(), drawSize_.Height());
            }
            hasSetDefaultSize_ = true;
        }
        AddTextureLayer();
    }
#ifdef OHOS_STANDARD_SYSTEM
    if (!hasSetDefaultSize_) {
        if (xcomponentSizeChangeEvent_ && (!drawSize_.IsHeightInfinite())) {
            xcomponentSizeChangeEvent_(textureId_, drawSize_.Width(), drawSize_.Height());
        }
        hasSetDefaultSize_ = true;
    }
#endif
    RenderNode::Paint(context, offset);
}

void FlutterRenderXComponent::DumpTree(int32_t depth)
{
    auto children = GetChildren();

    if (DumpLog::GetInstance().GetDumpFile() > 0) {
        DumpLog::GetInstance().AddDesc("sourceSize:", " width = ", GetLayoutSize().Width(),
            " height = ", GetLayoutSize().Height());
        DumpLog::GetInstance().AddDesc("globalPosition:", "x = ", GetGlobalOffset().GetX(),
            " y = ", GetGlobalOffset().GetY());
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children.size());
    }

    for (const auto& item : children) {
        item->DumpTree(depth + 1);
    }
}
} // namespace OHOS::Ace
