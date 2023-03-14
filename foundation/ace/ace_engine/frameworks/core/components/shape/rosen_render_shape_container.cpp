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

#include "frameworks/core/components/shape/rosen_render_shape_container.h"

#include "render_service_client/core/ui/rs_node.h"

#include "frameworks/core/components/transform/rosen_render_transform.h"
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

void RosenRenderShapeContainer::PerformLayout()
{
    RenderShapeContainer::PerformLayout();

    auto node = GetRSNode();
    if (node == nullptr) {
        return;
    }
    node->SetClipToFrame(true);

    double viewBoxWidth = NormalizePercentToPx(viewBox_.Width(), false);
    double viewBoxHeight = NormalizePercentToPx(viewBox_.Height(), true);
    double viewBoxLeft = NormalizePercentToPx(viewBox_.Left(), false);
    double viewBoxTop = NormalizePercentToPx(viewBox_.Top(), true);
    if (!GetLayoutSize().IsInfinite() && GreatNotEqual(viewBoxWidth, 0.0) && GreatNotEqual(viewBoxHeight, 0.0)) {
        double scale = std::min(GetLayoutSize().Width() / viewBoxWidth, GetLayoutSize().Height() / viewBoxHeight);
        double tx = GetLayoutSize().Width() * 0.5 - (viewBoxWidth * 0.5 + viewBoxLeft) * scale;
        double ty = GetLayoutSize().Height() * 0.5 - (viewBoxHeight * 0.5 + viewBoxTop) * scale;
        for (const auto& child : GetChildren()) {
            auto rsNode = child->GetRSNode();
            if (!rsNode) {
                continue;
            }
            rsNode->SetPivot(0.0f, 0.0f);
            rsNode->SetScale(scale);
            rsNode->SetTranslate({ tx, ty });
        }
    }
}

void RosenRenderShapeContainer::Paint(RenderContext& context, const Offset& offset)
{
    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    skCanvas_ = renderContext->GetCanvas();
    if (!skCanvas_) {
        return;
    }
    BitmapMesh(context, offset);
}

RefPtr<RosenRenderShape> RosenRenderShapeContainer::GetShapeChild(const RefPtr<RenderNode>& node, Offset& offset)
{
    offset += node->GetPosition();
    if (auto renderShape = AceType::DynamicCast<RosenRenderShape>(node)) {
        return renderShape;
    }
    const auto& children = node->GetChildren();
    for (const auto& item : SortChildrenByZIndex(children)) {
        if (RefPtr<RosenRenderShape> temp = GetShapeChild(item, offset)) {
            return temp;
        }
    }
    offset = offset - node->GetPosition();
    return nullptr;
}

void RosenRenderShapeContainer::BitmapMesh(RenderContext& context, const Offset& offset)
{
    auto pipelineContext = GetContext().Upgrade();

    auto imageInfo = SkImageInfo::Make(GetLayoutSize().Width(), GetLayoutSize().Height(),
        SkColorType::kRGBA_8888_SkColorType, SkAlphaType::kUnpremul_SkAlphaType);
    skOffBitmap_.allocPixels(imageInfo);
    skOffBitmap_.eraseColor(SK_ColorTRANSPARENT);
    skOffCanvas_ = std::make_unique<SkCanvas>(skOffBitmap_);

    const auto renderContext = static_cast<RosenRenderContext*>(&context);
    skCanvas_ = renderContext->GetCanvas();
    if (!skCanvas_) {
        return;
    }

    // for the child
    double viewBoxWidth = NormalizePercentToPx(viewBox_.Width(), false);
    double viewBoxHeight = NormalizePercentToPx(viewBox_.Height(), true);
    double viewBoxLeft = NormalizePercentToPx(viewBox_.Left(), false);
    double viewBoxTop = NormalizePercentToPx(viewBox_.Top(), true);
    if (!GetLayoutSize().IsInfinite() && GreatNotEqual(viewBoxWidth, 0.0) && GreatNotEqual(viewBoxHeight, 0.0)) {
        double scale = std::min(GetLayoutSize().Width() / viewBoxWidth, GetLayoutSize().Height() / viewBoxHeight);
        double tx = GetLayoutSize().Width() * 0.5 - (viewBoxWidth * 0.5 + viewBoxLeft) * scale;
        double ty = GetLayoutSize().Height() * 0.5 - (viewBoxHeight * 0.5 + viewBoxTop) * scale;
        skOffCanvas_->scale(scale, scale);
        skOffCanvas_->translate(tx, ty);
    }

    const auto& children = GetChildren();
    for (const auto& item : SortChildrenByZIndex(children)) {
        Offset childOffset = offset;
        RefPtr<RosenRenderShape> renderShape = GetShapeChild(item, childOffset);
        if (renderShape) {
            renderShape->PaintOnCanvas(skOffCanvas_.get(), childOffset);
        }
    }
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

void RosenRenderShapeContainer::DrawBitmapMesh(SkBitmap& bitmap, int column, int row,
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
