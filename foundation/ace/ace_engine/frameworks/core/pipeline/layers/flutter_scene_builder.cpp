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
#include "core/pipeline/layers/flutter_scene_builder.h"

namespace OHOS::Ace::Flutter {
namespace {

tonic::Float64List ToFloat64List(const Matrix4& matrix4)
{
    tonic::Float64List floatData(matrix4.Count());
    for (int32_t i = 0; i < matrix4.Count(); i++) {
        floatData[i] = matrix4[i];
    }
    return floatData;
}

}

FlutterSceneBuilder::FlutterSceneBuilder()
{
    sceneBuilder_ = flutter::SceneBuilder::create();
}

void FlutterSceneBuilder::AddPicture(double dx, double dy, fml::RefPtr<flutter::Picture> picture, int hints)
{
    if (!picture) {
        LOGE("FlutterSceneBuilder:can't add picture");
        return;
    }
    sceneBuilder_->addPicture(dx, dy, picture.get(), hints);
}

void FlutterSceneBuilder::AddTexture(
    double dx, double dy, double width, double height, int64_t textureId, bool freeze, uint8_t opacity)
{
    sceneBuilder_->addTexture(dx, dy, width, height, textureId, freeze, opacity);
}

void FlutterSceneBuilder::PushTransform(const Matrix4& matrix4)
{
    tonic::Float64List&& list = ToFloat64List(matrix4);
    sceneBuilder_->pushTransform(list);
}

void FlutterSceneBuilder::PushClipRRect(const flutter::RRect& rrect, int32_t clipBehavior)
{
    sceneBuilder_->pushClipRRect(rrect, clipBehavior);
}

void FlutterSceneBuilder::PushClipPath(const flutter::CanvasPath* path, int32_t clipBehavior)
{
    sceneBuilder_->pushClipPath(path, clipBehavior);
}

void FlutterSceneBuilder::PushOpacity(int32_t alpha, double dx, double dy)
{
    sceneBuilder_->pushOpacity(alpha, dx, dy);
}

void FlutterSceneBuilder::PushBackdropFilter(fml::RefPtr<flutter::ImageFilter> imageFilter)
{
    if (!imageFilter) {
        LOGE("FlutterSceneBuilder:can't create image filter");
        return;
    }
    sceneBuilder_->pushBackdropFilter(imageFilter.get());
}

void FlutterSceneBuilder::PushShaderMask(fml::RefPtr<flutter::Shader> shader, double maskRectLeft, double maskRectRight,
    double maskRectTop, double maskRectBottom, int32_t blendMode)
{
    if (!shader) {
        LOGE("FlutterSceneBuilder:can't push shader mask");
        return;
    }
    sceneBuilder_->pushShaderMask(shader.get(), maskRectLeft, maskRectRight, maskRectTop, maskRectBottom, blendMode);
}

void FlutterSceneBuilder::PushGradientColorMask(const SkPaint& maskPaint)
{
    sceneBuilder_->PushGradientColorMask(maskPaint);
}

void FlutterSceneBuilder::PushSvgMask(const sk_sp<SkSVGDOM>& svgDom, double x, double y, double scaleX, double scaleY)
{
    sceneBuilder_->PushSvgMask(svgDom, x, y, scaleX, scaleY);
}

void FlutterSceneBuilder::PushPathMask(const SkPaint& maskPaint, const SkPath& path)
{
    sceneBuilder_->PushPathMask(maskPaint, path);
}

void FlutterSceneBuilder::PushFilter(const SkPaint& filterPaint)
{
    sceneBuilder_->PushFilter(filterPaint);
}

void FlutterSceneBuilder::Pop()
{
    sceneBuilder_->pop();
}

fml::RefPtr<flutter::Scene> FlutterSceneBuilder::Build()
{
    return sceneBuilder_->build();
}

} // namespace OHOS::Ace::Flutter