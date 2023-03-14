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

#include "core/components/video/flutter_render_texture.h"

#include "flutter/fml/memory/ref_counted.h"
#include "flutter/lib/ui/painting/canvas.h"
#include "flutter/lib/ui/painting/picture_recorder.h"
#include "flutter/lib/ui/ui_dart_state.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/effects/SkBlurImageFilter.h"

#include "base/log/ace_trace.h"
#include "base/log/dump_log.h"
#include "core/components/display/render_display.h"
#include "core/pipeline/base/flutter_render_context.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t GAUSSIAN_DURATION = 1000;
constexpr Color GAUSSIAN_COLOR = Color(0xffc7bcb9);

} // namespace

using namespace Flutter;

void FlutterRenderTexture::Paint(RenderContext& context, const Offset& offset)
{
    const Size& layout = GetLayoutSize();

    layer_->SetClip(0.0, layout.Width(), 0.0, layout.Height(), Flutter::Clip::NONE);
    AddBackgroundLayer();

    if (textureId_ != INVALID_TEXTURE) {
        AddTextureLayer();
    }

    RenderNode::Paint(context, offset);
    if (IsAddGaussianFuzzy()) {
        AddGaussianFuzzy(context, offset);
    }
}

RenderLayer FlutterRenderTexture::GetRenderLayer()
{
    if (!layer_) {
        layer_ = AceType::MakeRefPtr<ClipLayer>(0.0, 0.0, 0.0, 0.0, Flutter::Clip::NONE);
    }

    return AceType::RawPtr(layer_);
}

void FlutterRenderTexture::AddTextureLayer()
{
    if (!textureLayer_) {
        textureLayer_ =
            AceType::MakeRefPtr<TextureLayer>(textureId_, false, drawSize_.Width(), drawSize_.Height(), opacity_);
    } else {
        textureLayer_->Update(textureId_, false, drawSize_.Width(), drawSize_.Height(), opacity_);
    }

    textureLayer_->SetOffset(alignmentX_, alignmentY_);

    layer_->AddChildren(textureLayer_);
}

void FlutterRenderTexture::InitGaussianFuzzyParas()
{
    gaussianFuzzySize_ = GetPaintRect();
    colorValue_ = GAUSSIAN_COLOR;
}

void FlutterRenderTexture::SetIsAddGaussianFuzzy(bool isAddGaussianFuzzy)
{
    RenderTexture::SetIsAddGaussianFuzzy(isAddGaussianFuzzy);
    if (isAddGaussianFuzzy) {
        if (!controller_) {
            controller_ = AceType::MakeRefPtr<Animator>(GetContext());
        } else if (controller_->IsRunning()) {
            controller_->Finish();
        }
        controller_->ClearInterpolators();
        controller_->ClearAllListeners();

        controller_->AddStartListener([weak = AceType::WeakClaim(this)]() {
            auto texture = weak.Upgrade();
            if (texture) {
                texture->InitGaussianFuzzyParas();
            }
        });

        controller_->AddStopListener([weak = AceType::WeakClaim(this)]() {
            auto texture = weak.Upgrade();
            if (texture) {
                texture->SetIsAddGaussianFuzzy(false);
            }
        });

        moveAnimation_ = AceType::MakeRefPtr<CurveAnimation<uint8_t>>(0, 1, Curves::LINEAR);
        moveAnimation_->AddListener(
            Animation<uint8_t>::ValueCallback([weak = AceType::WeakClaim(this)](uint8_t value) {
                auto texture = weak.Upgrade();
                if (texture) {
                    texture->MarkNeedRender();
                }
            }));

        controller_->SetDuration(GAUSSIAN_DURATION);
        controller_->AddInterpolator(moveAnimation_);
        controller_->Play();
    } else {
        if (controller_ && controller_->IsRunning()) {
            controller_->Stop();
        }
    }
}


void FlutterRenderTexture::AddBackgroundLayer()
{
    if (!backgroundLayer_) {
        backgroundLayer_ = AceType::MakeRefPtr<PictureLayer>();
    }

    if (needDrawBackground_) {
        DrawBackground();
    }

    if (imageFit_ != ImageFit::FILL) {
        layer_->AddChildren(backgroundLayer_);
    }
}

void FlutterRenderTexture::AddGaussianFuzzy(RenderContext& context, const Offset& offset)
{
    const auto flutterContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = flutterContext->GetCanvas();
    if (canvas == nullptr) {
        LOGE("Paint canvas is null.");
        return;
    }

    SkCanvas* skCanvas = canvas->canvas();
    if (skCanvas == nullptr) {
        LOGE("Paint skCanvas is null.");
        return;
    }

    SkRRect skRRect = SkRRect::MakeRect(SkRect::MakeLTRB(
        gaussianFuzzySize_.Left(), gaussianFuzzySize_.Top(), gaussianFuzzySize_.Right(), gaussianFuzzySize_.Bottom()));

    SkAutoCanvasRestore acr(skCanvas, true);
    skCanvas->clipRRect(skRRect, true);

    SkPaint paint;
    paint.setAntiAlias(true);
#ifdef USE_SYSTEM_SKIA
    paint.setColorFilter(SkColorFilter::MakeModeFilter(colorValue_.GetValue(), SkBlendMode::kDstOver));
#else
    paint.setColorFilter(SkColorFilters::Blend(colorValue_.GetValue(), SkBlendMode::kDstOver));
#endif
    paint.setImageFilter(SkBlurImageFilter::Make(gaussianFuzzySize_.Width(), gaussianFuzzySize_.Height(), nullptr));
    SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
    skCanvas->saveLayer(slr);
}

void FlutterRenderTexture::DrawBackground()
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

    needDrawBackground_ = false;
}

void FlutterRenderTexture::PerformLayout()
{
    RenderTexture::PerformLayout();
    needDrawBackground_ = true;
}

void FlutterRenderTexture::UpdateOpacity(uint8_t opacity)
{
    if (!SupportOpacity()) {
        return;
    }
    if (opacity_ != opacity) {
        RenderNode::UpdateOpacity(opacity);
        needDrawBackground_ = true;
        auto displayChild = AceType::DynamicCast<RenderDisplay>(GetFirstChild());
        if (displayChild) {
            displayChild->UpdateOpacity(opacity);
        }
    }
}

void FlutterRenderTexture::DumpTree(int32_t depth)
{
    auto children = GetChildren();

    if (DumpLog::GetInstance().GetDumpFile() > 0) {
        DumpLog::GetInstance().AddDesc("textureId:", textureId_);
        DumpLog::GetInstance().AddDesc("drawSize:", "width = ", drawSize_.Width(), " height = ", drawSize_.Height());
        DumpLog::GetInstance().AddDesc("sourceSize:", "width = ", drawSize_.Width(), " height = ", drawSize_.Height());
        DumpLog::GetInstance().AddDesc("alignmentX:", alignmentX_);
        DumpLog::GetInstance().AddDesc("alignmentY:", alignmentY_);
        DumpLog::GetInstance().Print(depth, AceType::TypeName(this), children.size());
    }

    for (const auto& item : children) {
        item->DumpTree(depth + 1);
    }
}

} // namespace OHOS::Ace
