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

#include "frameworks/core/components/shape/flutter_render_shape_container.h"

#include "flutter/lib/ui/painting/picture.h"
#include "frameworks/core/components/transform/flutter_render_transform.h"
#include "frameworks/core/pipeline/base/flutter_render_context.h"
#include "utils/native/base/include/securec.h"

namespace OHOS::Ace {
namespace {

struct ZIndexCompartor {
    bool operator()(const RefPtr<RenderNode>& left, const RefPtr<RenderNode>& right) const
    {
        return (left->GetZIndex() < right->GetZIndex());
    }
};

inline std::multiset<RefPtr<RenderNode>, ZIndexCompartor> SortChildrenByZIndex(
    const std::list<RefPtr<RenderNode>>& children)
{
    return std::multiset<RefPtr<RenderNode>, ZIndexCompartor>(children.begin(), children.end());
}

} // namespace

using namespace Flutter;



RenderLayer FlutterRenderShapeContainer::GetRenderLayer()
{
    if (!transformLayer_) {
        transformLayer_ = AceType::MakeRefPtr<Flutter::TransformLayer>(Matrix4::CreateIdentity(), 0.0, 0.0);
    }
    return AceType::RawPtr(transformLayer_);
}

void FlutterRenderShapeContainer::Paint(RenderContext& context, const Offset& offset)
{
    double viewBoxWidth = NormalizePercentToPx(viewBox_.Width(), false);
    double viewBoxHeight = NormalizePercentToPx(viewBox_.Height(), true);
    double viewBoxLeft = NormalizePercentToPx(viewBox_.Left(), false);
    double viewBoxTop = NormalizePercentToPx(viewBox_.Top(), true);
    if (!GetLayoutSize().IsInfinite() && GreatNotEqual(viewBoxWidth, 0.0) && GreatNotEqual(viewBoxHeight, 0.0)) {
        double scale = std::min(GetLayoutSize().Width() / viewBoxWidth,
            GetLayoutSize().Height() / viewBoxHeight);
        double tx = GetLayoutSize().Width() * 0.5 - (viewBoxWidth * 0.5 + viewBoxLeft) * scale;
        double ty = GetLayoutSize().Height() * 0.5 - (viewBoxHeight * 0.5 + viewBoxTop) * scale;
        auto transform = Matrix4::CreateScale(static_cast<float>(scale), static_cast<float>(scale), 1.0f);
        transform = FlutterRenderTransform::GetTransformByOffset(transform, GetGlobalOffset());
        transform = Matrix4::CreateTranslate(static_cast<float>(tx), static_cast<float>(ty), 0.0f) * transform;
        transformLayer_->Update(transform);
    }
    RenderNode::Paint(context, offset);
}

RefPtr<FlutterRenderShape> FlutterRenderShapeContainer::GetShapeChild(const RefPtr<RenderNode>& node)
{
    const auto& children = node->GetChildren();
    for (const auto& item : SortChildrenByZIndex(children)) {
        RefPtr<FlutterRenderShape> renderShape = AceType::DynamicCast<FlutterRenderShape>(item);
        if (renderShape) {
            return renderShape;
        } else {
            RefPtr<FlutterRenderShape> temp = GetShapeChild(item);
            if (temp) {
                return temp;
            }
        }
    }

    return nullptr;
}

void FlutterRenderShapeContainer::BitmapMesh(RenderContext& context, const Offset& offset)
{
    // new no-gpu cancas;
    auto pipelineContext = GetContext().Upgrade();


    auto imageInfo = SkImageInfo::Make(GetLayoutSize().Width(), GetLayoutSize().Height(),
        SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kOpaque_SkAlphaType);
    skOffBitmap_.allocPixels(imageInfo);
    skOffBitmap_.eraseColor(SK_ColorTRANSPARENT);
    skOffCanvas_ = std::make_unique<SkCanvas>(skOffBitmap_);


    // for the child
    const auto& children = GetChildren();
    for (const auto& item : SortChildrenByZIndex(children)) {
        RefPtr<FlutterRenderShape> renderShape = GetShapeChild(item);
        if (renderShape) {
        } else {
            continue;
        }
        renderShape->PaintOnCanvas(skOffCanvas_.get(), Offset(renderShape->GetPosition().GetX() + offset.GetX(),
            renderShape->GetPosition().GetY() + offset.GetY()));
    }

    const auto renderContext = static_cast<FlutterRenderContext*>(&context);
    flutter::Canvas* canvas = renderContext->GetCanvas();
    if (!canvas) {
        return;
    }
    skCanvas_ = canvas->canvas();
    if (column_ == 0 && row_ == 0) {
        skCanvas_->drawBitmap(skOffBitmap_, 0, 0);
        return;
    }
    uint32_t size = mesh_.size();
    float verts[size];
    for (uint32_t i = 0; i < size; i++) {
        verts[i] = (float)mesh_[i];
    }
    DrawBitmapMesh(skOffBitmap_, column_, row_, verts, 0, nullptr);
}

void FlutterRenderShapeContainer::DrawBitmapMesh(SkBitmap& bitmap, int column, int row,
    const float* vertices, const int* colors, const SkPaint* paint)
{
    const int vertCounts = (column + 1) * (row + 1);
    int32_t size = 6;
    const int indexCount = column * row * size;
    uint32_t flags = SkVertices::kHasTexCoords_BuilderFlag;
    if (colors) {
        flags |= SkVertices::kHasColors_BuilderFlag;
    }
    SkVertices::Builder builder(SkVertices::kTriangles_VertexMode, vertCounts, indexCount, flags);
    if (memcpy_s(builder.positions(), vertCounts * sizeof(SkPoint), vertices, vertCounts * sizeof(SkPoint)) != 0) {
        return;
    }
    if (colors) {
        if (memcpy_s(builder.colors(), vertCounts * sizeof(SkColor), colors, vertCounts * sizeof(SkColor)) != 0) {
            return;
        }
    }
    SkPoint* texsPoint = builder.texCoords();
    uint16_t* indices = builder.indices();
    const SkScalar height = SkIntToScalar(bitmap.height());
    const SkScalar width = SkIntToScalar(bitmap.width());

    if (row == 0) {
        LOGE("row is zero");
        return;
    }
    if (column == 0) {
        LOGE("column is zero");
        return;
    }
    const SkScalar dy = height / row;
    const SkScalar dx = width / column;

    SkPoint* texsPit = texsPoint;
    SkScalar y = 0;
    for (int i = 0; i <= row; i++) {
        if (i == row) {
            y = height;  // to ensure numerically we hit h exactly
        }
        SkScalar x = 0;
        for (int j = 0; j < column; j++) {
            texsPit->set(x, y);
            texsPit += 1;
            x += dx;
        }
        texsPit->set(width, y);
        texsPit += 1;
        y += dy;
    }

    uint16_t* dex = indices;
    int index = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            *dex++ = index;
            *dex++ = index + column + 1;
            *dex++ = index + column + 2;

            *dex++ = index;
            *dex++ = index + column + 2;
            *dex++ = index + 1;

            index += 1;
        }
        index += 1;
    }

    SkPaint tempPaint;
    if (paint) {
        tempPaint = *paint;
    }
    sk_sp<SkShader> shader;
    sk_sp<SkImage> image = SkImage::MakeFromBitmap(bitmap);
#ifdef USE_SYSTEM_SKIA
    shader = image->makeShader(SkShader::kClamp_TileMode, SkShader::kClamp_TileMode);
#else
    shader = image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp);
#endif
    tempPaint.setShader(shader);
    skCanvas_->drawVertices(builder.detach(), SkBlendMode::kModulate, tempPaint);
}

} // namespace OHOS::Ace
