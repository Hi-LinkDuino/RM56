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
#include "core/pipeline/layers/ace_scene_builder.h"

#include "base/utils/utils.h"

namespace OHOS::Ace::Flutter {
namespace {

// Mappings from SkMatrix-index to input-index.
static const int32_t K_SK_MATRIX_INDEX_TO_MATRIX4_INDEX[] = {
    0,
    4,
    12,
    1,
    5,
    13,
    3,
    7,
    15,
};

SkMatrix ToSkMatrix(const Matrix4& matrix4)
{
    SkMatrix skMatrix;
    for (std::size_t i = 0; i < ArraySize(K_SK_MATRIX_INDEX_TO_MATRIX4_INDEX); ++i) {
        int32_t matrixIndex = K_SK_MATRIX_INDEX_TO_MATRIX4_INDEX[i];
        if (matrixIndex < matrix4.Count())
            skMatrix[i] = matrix4[matrixIndex];
        else
            skMatrix[i] = 0.0;
    }
    return skMatrix;
}

} // namespace

AceSceneBuilder::AceSceneBuilder()
{
    layerTreeBuilder_ = flutter::OHOS::LayerTreeBuilder::Create();
}

void AceSceneBuilder::AddPicture(double dx, double dy, fml::RefPtr<flutter::Picture> picture, int hints)
{
    if (!picture) {
        LOGE("AceSceneBuilder:can't add picture");
        return;
    }
    layerTreeBuilder_->AddPicture(dx, dy, picture.get()->picture());
}

void AceSceneBuilder::AddTexture(
    double dx, double dy, double width, double height, int64_t textureId, bool freeze, uint8_t opacity)
{
    layerTreeBuilder_->AddTexture(dx, dy, width, height, textureId, freeze, opacity);
}

void AceSceneBuilder::PushTransform(const Matrix4& matrix4)
{
    auto skMatrix = ToSkMatrix(matrix4);
    layerTreeBuilder_->PushTransform(skMatrix);
}

void AceSceneBuilder::PushClipRRect(const flutter::RRect& rrect, int32_t clipBehavior)
{
    layerTreeBuilder_->PushClipRRect(rrect.sk_rrect, clipBehavior);
}

void AceSceneBuilder::PushClipPath(const flutter::CanvasPath* path, int32_t clipBehavior)
{
    if (!path) {
        LOGE("AceSceneBuilder:can't create clip path");
        return;
    }
    layerTreeBuilder_->PushClipPath(path->path(), clipBehavior);
}

void AceSceneBuilder::PushOpacity(int32_t alpha, double dx, double dy)
{
    layerTreeBuilder_->PushOpacity(alpha, dx, dy);
}

void AceSceneBuilder::PushBackdropFilter(fml::RefPtr<flutter::ImageFilter> imageFilter)
{
    if (!imageFilter) {
        LOGE("AceSceneBuilder:can't create image filter");
        return;
    }
    layerTreeBuilder_->PushBackdropFilter(imageFilter.get()->filter());
}

void AceSceneBuilder::PushShaderMask(fml::RefPtr<flutter::Shader> shader, double maskRectLeft, double maskRectRight,
    double maskRectTop, double maskRectBottom, int32_t blendMode)
{
    if (!shader) {
        LOGE("AceSceneBuilder:can't push shader mask");
        return;
    }
    layerTreeBuilder_->PushShaderMask(
        shader.get()->shader(), maskRectLeft, maskRectRight, maskRectTop, maskRectBottom, blendMode);
}

void AceSceneBuilder::PushSvgMask(const sk_sp<SkSVGDOM>& svgDom, double x, double y, double scaleX, double scaleY)
{
    layerTreeBuilder_->PushSvgMask(svgDom, x, y, scaleX, scaleY);
}

void AceSceneBuilder::PushGradientColorMask(const SkPaint& maskPaint)
{
    layerTreeBuilder_->PushGradientColorMask(maskPaint);
}

void AceSceneBuilder::PushPathMask(const SkPaint& maskPaint, const SkPath& maskPath)
{
    layerTreeBuilder_->PushPathMask(maskPaint, maskPath);
}

void AceSceneBuilder::PushFilter(const SkPaint& filterPaint)
{
    layerTreeBuilder_->PushFilter(filterPaint);
}

void AceSceneBuilder::Pop()
{
    layerTreeBuilder_->Pop();
}

std::unique_ptr<flutter::OHOS::LayerTree> AceSceneBuilder::GetLayerTree() const
{
    return layerTreeBuilder_->GetLayerTree();
}

} // namespace OHOS::Ace::Flutter