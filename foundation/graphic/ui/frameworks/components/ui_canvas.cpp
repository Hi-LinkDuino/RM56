/*
 * Copyright (c) 2020-2022 Huawei Device Co., Ltd.
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

#include "components/ui_canvas.h"

#include "draw/clip_utils.h"
#include "draw/draw_arc.h"
#include "draw/draw_image.h"
#include "draw/draw_utils.h"
#include "gfx_utils/graphic_log.h"
#include "render/render_buffer.h"
#include "render/render_pixfmt_rgba_blend.h"
#include "render/render_scanline.h"

namespace OHOS {
void InitRenderAndTransform(BufferInfo& gfxDstBuffer,
                            RenderBuffer& renderBuffer,
                            const Rect& rect,
                            TransAffine& transform,
                            const Style& style,
                            const Paint& paint);

void RenderSolid(const Paint& paint,
                 RasterizerScanlineAntialias& rasterizer,
                 RenderBase& renBase,
                 const bool& isStroke);

void UICanvas::BeginPath()
{
    /* If the previous path is not added to the drawing linked list, it should be destroyed directly. */
    if (vertices_ != nullptr && vertices_->GetTotalVertices() == 0) {
        delete vertices_;
        vertices_ = nullptr;
    }

    vertices_ = new UICanvasVertices();
    if (vertices_ == nullptr) {
        GRAPHIC_LOGE("new UICanvasVertices fail");
        return;
    }
}

void UICanvas::MoveTo(const Point& point)
{
    if (vertices_ == nullptr) {
        return;
    }
    vertices_->MoveTo(point.x, point.y);
}

void UICanvas::LineTo(const Point& point)
{
    if (vertices_ == nullptr) {
        return;
    }
    vertices_->LineTo(point.x, point.y);
}

void UICanvas::ArcTo(const Point& center, uint16_t radius, int16_t startAngle, int16_t endAngle)
{
    if (vertices_ == nullptr || startAngle == endAngle) {
        return;
    }
    float sinma = radius * Sin(startAngle);
    float cosma = radius * Sin(QUARTER_IN_DEGREE - startAngle);
    if (vertices_->GetTotalVertices() != 0) {
        vertices_->LineTo(float(center.x + sinma), float(center.y - cosma));
    } else {
        vertices_->MoveTo(float(center.x + sinma), float(center.y - cosma));
    }
    if (MATH_ABS(startAngle - endAngle) < CIRCLE_IN_DEGREE) {
        sinma = radius * Sin(endAngle);
        cosma = radius * Sin(QUARTER_IN_DEGREE - endAngle);
    }
    int16_t angle = endAngle - startAngle;
    bool largeArcFlag = false;
    if (angle > SEMICIRCLE_IN_DEGREE || angle <= 0) {
        largeArcFlag = true;
    }
    vertices_->ArcTo(radius, radius, angle, largeArcFlag, 1, float(center.x + sinma), float(center.y - cosma));
}

void UICanvas::AddRect(const Point& point, int16_t height, int16_t width)
{
    if (vertices_ == nullptr) {
        return;
    }

    int16_t right = static_cast<int16_t>(point.x + width);
    int16_t bottom = point.y + height;

    float fRight = static_cast<float>(width) + static_cast<float>(point.x);
    float fBottom = static_cast<float>(height) + static_cast<float>(point.y);
    const int16_t setup = 3;
    if (fRight > INT16_MAX) {
        right += setup;
    }
    if (fBottom > INT16_MAX) {
        bottom += setup;
    }
    MoveTo(point);
    LineTo({right, point.y});
    LineTo({right, bottom});
    LineTo({point.x, bottom});
    ClosePath();
}

void UICanvas::ClosePath()
{
    if (vertices_ == nullptr) {
        return;
    }
    vertices_->ClosePolygon();
}

UICanvas::~UICanvas()
{
    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        param = curDraw->data_.param;
        curDraw->data_.DeleteParam(param);
        curDraw->data_.param = nullptr;
    }
    drawCmdList_.Clear();
    if (vertices_ != nullptr) {
        delete vertices_;
        vertices_ = nullptr;
    }
    DestroyMapBufferInfo();
}

void UICanvas::Clear()
{
    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        param = curDraw->data_.param;
        curDraw->data_.DeleteParam(param);
        curDraw->data_.param = nullptr;
    }
    drawCmdList_.Clear();
    if (vertices_ != nullptr) {
        delete vertices_;
        vertices_ = nullptr;
    }
    Invalidate();
}

void UICanvas::DrawLine(const Point& endPoint, const Paint& paint)
{
    DrawLine(startPoint_, endPoint, paint);
}

void UICanvas::DrawLine(const Point& startPoint, const Point& endPoint, const Paint& paint)
{
    LineParam* lineParam = new LineParam;
    if (lineParam == nullptr) {
        GRAPHIC_LOGE("new LineParam fail");
        return;
    }
    lineParam->start = startPoint;
    lineParam->end = endPoint;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = lineParam;
    cmd.DeleteParam = DeleteLineParam;
    cmd.DrawGraphics = DoDrawLine;
    drawCmdList_.PushBack(cmd);

    Invalidate();
    SetStartPosition(endPoint);
}

void UICanvas::DrawCurve(const Point& control1, const Point& control2, const Point& endPoint, const Paint& paint)
{
    DrawCurve(startPoint_, control1, control2, endPoint, paint);
}

void UICanvas::DrawCurve(const Point& startPoint,
                         const Point& control1,
                         const Point& control2,
                         const Point& endPoint,
                         const Paint& paint)
{
    CurveParam* curveParam = new CurveParam;
    if (curveParam == nullptr) {
        GRAPHIC_LOGE("new CurveParam fail");
        return;
    }
    curveParam->start = startPoint;
    curveParam->control1 = control1;
    curveParam->control2 = control2;
    curveParam->end = endPoint;

    DrawCmd cmd;
    cmd.paint = paint;
    if (paint.GetStrokeWidth() > MAX_CURVE_WIDTH) {
        cmd.paint.SetStrokeWidth(MAX_CURVE_WIDTH);
    }
    cmd.param = curveParam;
    cmd.DeleteParam = DeleteCurveParam;
    cmd.DrawGraphics = DoDrawCurve;
    drawCmdList_.PushBack(cmd);

    Invalidate();
    SetStartPosition(endPoint);
}

void UICanvas::DrawRect(const Point& startPoint, int16_t height, int16_t width, const Paint& paint)
{
    if (!paint.GetChangeFlag()) {
        if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
            DrawRectSetCmd(startPoint, height, width, paint, Paint::PaintStyle::STROKE_STYLE);
        }

        if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
            DrawRectSetCmd(startPoint, height, width, paint, Paint::PaintStyle::FILL_STYLE);
        }
    } else {
        BeginPath();
        MoveTo(startPoint);
        LineTo({static_cast<int16_t>(startPoint.x + width), startPoint.y});
        LineTo({static_cast<int16_t>(startPoint.x + width), static_cast<int16_t>(startPoint.y + height)});
        LineTo({startPoint.x, static_cast<int16_t>(startPoint.y + height)});
        ClosePath();
        FillPath(paint);
        DrawPath(paint);
    }
    Invalidate();
}

void UICanvas::DrawRectSetCmd(const Point& startPoint, int16_t height, int16_t width, const Paint& paint,
                              Paint::PaintStyle paintStyle)
{
    RectParam* rectParam = new RectParam;
    if (rectParam == nullptr) {
        GRAPHIC_LOGE("new RectParam fail");
        return;
    }
    rectParam->start = startPoint;
    rectParam->height = height;
    rectParam->width = width;

    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = rectParam;
    cmd.DeleteParam = DeleteRectParam;
    if (paintStyle == Paint::PaintStyle::STROKE_STYLE) {
        cmd.DrawGraphics = DoDrawRect;
    } else if (paintStyle == Paint::PaintStyle::FILL_STYLE) {
        cmd.DrawGraphics = DoFillRect;
    }
    drawCmdList_.PushBack(cmd);
}

void UICanvas::StrokeRect(const Point& startPoint, int16_t height, int16_t width, const Paint& paint)
{
    if (!paint.GetChangeFlag()) {
        RectParam* rectParam = new RectParam;
        if (rectParam == nullptr) {
            GRAPHIC_LOGE("new RectParam fail");
            return;
        }
        rectParam->start = startPoint;
        rectParam->height = height;
        rectParam->width = width;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = rectParam;
        cmd.DeleteParam = DeleteRectParam;
        cmd.DrawGraphics = DoDrawRect;
        drawCmdList_.PushBack(cmd);
    } else {
        BeginPath();
        MoveTo(startPoint);
        LineTo({static_cast<int16_t>(startPoint.x + width), startPoint.y});
        LineTo({static_cast<int16_t>(startPoint.x + width), static_cast<int16_t>(startPoint.y + height)});
        LineTo({startPoint.x, static_cast<int16_t>(startPoint.y + height)});
        ClosePath();
        DrawPath(paint);
    }
    SetStartPosition(startPoint);
}

void UICanvas::ClearRect(const Point& startPoint, int16_t height, int16_t width)
{
    Paint paint;
    paint.SetFillColor(this->style_->bgColor_);
    paint.SetStyle(Paint::FILL_STYLE);
    BeginPath();
    MoveTo(startPoint);
    LineTo({static_cast<int16_t>(startPoint.x + width), startPoint.y});
    LineTo({static_cast<int16_t>(startPoint.x + width), static_cast<int16_t>(startPoint.y + height)});
    LineTo({startPoint.x, static_cast<int16_t>(startPoint.y + height)});
    ClosePath();
    FillPath(paint);
}

void UICanvas::DrawCircle(const Point& center, uint16_t radius, const Paint& paint)
{
    if (paint.GetChangeFlag()) {
#if GRAPHIC_ENABLE_BEZIER_ARC_FLAG
        if (vertices_ == nullptr) {
            vertices_ = new UICanvasVertices();
        }
        vertices_->RemoveAll();
        BezierArc arc(center.x, center.y, radius, radius, 0, TWO_TIMES * PI);
        vertices_->ConcatPath(arc, 0);
        vertices_->ClosePolygon();
        if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
            DrawPath(paint);
        }
        if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
            FillPath(paint);
        }
#endif
    } else {
        CircleParam* circleParam = new CircleParam;
        if (circleParam == nullptr) {
            GRAPHIC_LOGE("new CircleParam fail");
            return;
        }
        circleParam->center = center;
        circleParam->radius = radius;

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = circleParam;
        cmd.DeleteParam = DeleteCircleParam;
        cmd.DrawGraphics = DoDrawCircle;
        drawCmdList_.PushBack(cmd);
    }

    Invalidate();
}

void UICanvas::DrawSector(const Point& center,
                          uint16_t radius,
                          int16_t startAngle,
                          int16_t endAngle,
                          const Paint& paint)
{
    BeginPath();
    MoveTo(center);
    ArcTo(center, radius, startAngle, endAngle);
    ClosePath();
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
        DrawPath(paint);
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        FillPath(paint);
    }
}

void UICanvas::DrawArc(const Point& center, uint16_t radius, int16_t startAngle,
                       int16_t endAngle, const Paint& paint)
{
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE) {
        if (paint.GetChangeFlag()) {
            ArcTo(center, radius, startAngle, endAngle);
            DrawPath(paint);
        } else {
            ArcParam* arcParam = new ArcParam;
            if (arcParam == nullptr) {
                GRAPHIC_LOGE("new ArcParam fail");
                return;
            }
            arcParam->center = center;
            arcParam->radius = radius;

            int16_t start;
            int16_t end;
            if (startAngle > endAngle) {
                start = endAngle;
                end = startAngle;
            } else {
                start = startAngle;
                end = endAngle;
            }

            DrawArc::GetInstance()->GetDrawRange(start, end);
            arcParam->startAngle = start;
            arcParam->endAngle = end;

            DrawCmd cmd;
            cmd.paint = paint;
            cmd.param = arcParam;
            cmd.DeleteParam = DeleteArcParam;
            cmd.DrawGraphics = DoDrawArc;
            drawCmdList_.PushBack(cmd);
        }
        Invalidate();
    }
}

void UICanvas::DrawLabel(const Point& startPoint,
                         const char* text,
                         uint16_t maxWidth,
                         const FontStyle& fontStyle,
                         const Paint& paint)
{
    if (text == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        UILabel* label = new UILabel();
        if (label == nullptr) {
            GRAPHIC_LOGE("new UILabel fail");
            return;
        }
        label->SetLineBreakMode(UILabel::LINE_BREAK_CLIP);
        label->SetPosition(startPoint.x, startPoint.y);
        label->SetWidth(maxWidth);
        label->SetHeight(GetHeight());
        label->SetText(text);
        label->SetFont(fontStyle.fontName, fontStyle.fontSize);
        label->SetAlign(fontStyle.align);
        label->SetDirect(fontStyle.direct);
        label->SetStyle(STYLE_LETTER_SPACE, fontStyle.letterSpace);
        label->SetStyle(STYLE_TEXT_COLOR, paint.GetFillColor().full);
        label->SetStyle(STYLE_TEXT_OPA, paint.GetOpacity());

        DrawCmd cmd;
        cmd.param = label;
        cmd.DeleteParam = DeleteLabel;
        cmd.DrawGraphics = DoDrawLabel;
        drawCmdList_.PushBack(cmd);

        Invalidate();
    }
}
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
void UICanvas::DrawImage(const Point &startPoint, const char* image, const Paint& paint)
{
    if (image == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        UIExtendImageView* imageView = new UIExtendImageView();
        if (imageView == nullptr) {
            GRAPHIC_LOGE("new UIImageView fail");
            return;
        }
        imageView->SetCanvas(this);
        imageView->SetPosition(startPoint.x, startPoint.y);
        imageView->SetSrc(image);

        DrawCmd cmd;
        cmd.paint = paint;
        cmd.param = imageView;
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
        SetStartPosition(startPoint);
    }
}

void UICanvas::DrawImage(const Point& startPoint, const char* image,
                         const Paint& paint, int16_t width, int16_t height)
{
    if (image == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        UIExtendImageView* imageView = new UIExtendImageView();
        if (imageView == nullptr) {
            GRAPHIC_LOGE("new UIImageView fail");
            return;
        }
        imageView->SetCanvas(this);
        imageView->SetPosition(startPoint.x, startPoint.y);
        imageView->SetSrc(image);
        float scaleX = 1.0;
        float scaleY = 1.0;
        if (width > 0 && imageView->GetWidth() > 0) {
            scaleX = (float)width / (float)imageView->GetWidth();
        }
        if (height > 0 && imageView->GetHeight() > 0) {
            scaleY = (float)height / (float)imageView->GetHeight();
        }
        DrawCmd cmd;
        cmd.paint = paint;
        cmd.paint.Scale(scaleX, scaleY);
        cmd.param = imageView;
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
        SetStartPosition(startPoint);
    }

    Invalidate();
    SetStartPosition(startPoint);
}

void UICanvas::DrawImage(const Point& startPoint, const char* image,
                         const Paint& paint, int16_t width,bool rePaint,int16_t index)
{
    int16_t height=width;
    if (image == nullptr) {
        return;
    }
    if(rePaint){
        DrawCmd cmd=(cmds_[index]);
        UIExtendImageView* image1 = static_cast<UIExtendImageView*>(cmds_[index].param);

        image1->SetCanvas(this);
        image1->SetPosition(startPoint.x, startPoint.y);

        float scaleX = 1.0;
        float scaleY = 1.0;
        if (width > 0 && image1->GetWidth() > 0) {
            scaleX = (float)width / (float)image1->GetWidth();
        }
        if (height > 0 && image1->GetHeight() > 0) {
            scaleY = (float)height / (float)image1->GetHeight();
        }

        cmd.paint = paint;
        cmd.paint.Scale(scaleX, scaleY);
        cmd.param = image1;
        cmd.DeleteParam = DeleteImageView;
        cmd.DrawGraphics = DoDrawImage;
        drawCmdList_.PushBack(cmd);

        Invalidate();
        SetStartPosition(startPoint);

    }else {
        if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
            UIExtendImageView* imageView = new UIExtendImageView();
            if (imageView == nullptr) {
                GRAPHIC_LOGE("new UIImageView fail");
                return;
            }
            imageView->SetCanvas(this);
            imageView->SetPosition(startPoint.x, startPoint.y);
            imageView->SetSrc(image);
            float scaleX = 1.0;
            float scaleY = 1.0;
            if (width > 0 && imageView->GetWidth() > 0) {
                scaleX = (float)width / (float)imageView->GetWidth();
            }
            if (height > 0 && imageView->GetHeight() > 0) {
                scaleY = (float)height / (float)imageView->GetHeight();
            }
            DrawCmd cmd;
            cmd.paint = paint;
            cmd.paint.Scale(scaleX, scaleY);
            cmd.param = imageView;
            cmd.DeleteParam = DeleteImageView;
            cmd.DrawGraphics = DoDrawImage;
            drawCmdList_.PushBack(cmd);

            cmds_[index] = cmd;

            Invalidate();
            SetStartPosition(startPoint);
        }
    }

    Invalidate();
    SetStartPosition(startPoint);
}
#endif

void UICanvas::DrawPath(const Paint& paint)
{
    if (vertices_ == nullptr) {
        return;
    }

    PathParam* pathParam = new PathParam;
    if (pathParam == nullptr) {
        GRAPHIC_LOGE("new LineParam fail");
        return;
    }

    pathParam->vertices = vertices_;
    pathParam->isStroke = true;
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    if (paint.GetStyle() == Paint::PATTERN) {
        ImageParam* imageParam = new ImageParam;
        if (imageParam == nullptr) {
            GRAPHIC_LOGE("new ImageParam fail");
            return;
        }
        imageParam->image = new Image();
        if (imageParam->image == nullptr) {
            delete imageParam;
            imageParam = nullptr;
            return;
        }

        imageParam->image->SetSrc(paint.GetPatternImage());
        ImageHeader header = {0};
        imageParam->image->GetHeader(header);
        imageParam->start = {0, 0};
        imageParam->height = header.height;
        imageParam->width = header.width;
        pathParam->imageParam = imageParam;
    }
#endif
    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = pathParam;
    cmd.DeleteParam = DeletePathParam;
    cmd.DrawGraphics = DoDrawPath;
    drawCmdList_.PushBack(cmd);
    Invalidate();
}

void UICanvas::FillPath(const Paint& paint)
{
    if (vertices_ == nullptr) {
        return;
    }

    PathParam* pathParam = new PathParam;
    if (pathParam == nullptr) {
        GRAPHIC_LOGE("new LineParam fail");
        return;
    }

    pathParam->vertices = vertices_;
    pathParam->isStroke = false;
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    if (paint.GetStyle() == Paint::PATTERN) {
        ImageParam* imageParam = new ImageParam;
        if (imageParam == nullptr) {
            GRAPHIC_LOGE("new ImageParam fail");
            return;
        }
        imageParam->image = new Image();
        if (imageParam->image == nullptr) {
            delete imageParam;
            imageParam = nullptr;
            return;
        }

        imageParam->image->SetSrc(paint.GetPatternImage());
        ImageHeader header = {0};
        imageParam->image->GetHeader(header);
        imageParam->start = {0, 0};
        imageParam->height = header.height;
        imageParam->width = header.width;
        pathParam->imageParam = imageParam;
    }
#endif
    DrawCmd cmd;
    cmd.paint = paint;
    cmd.param = pathParam;
    cmd.DeleteParam = DeletePathParam;
    cmd.DrawGraphics = DoFillPath;
    drawCmdList_.PushBack(cmd);
    Invalidate();
}

void UICanvas::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    Rect rect = GetOrigRect();
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, rect, invalidatedArea, *style_, opaScale_);

    void* param = nullptr;
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    Rect coords = GetOrigRect();
    Rect trunc = invalidatedArea;
    if (!trunc.Intersect(trunc, coords)) {
        return;
    }
    bool haveComposite = false;
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        if (curDraw->data_.paint.HaveComposite()) {
            haveComposite = true;
            break;
        }
    }

    if (haveComposite) {
        OnBlendDraw(gfxDstBuffer, trunc);
    } else {
        curDraw = drawCmdList_.Begin();
        for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
            param = curDraw->data_.param;
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
            if (curDraw->data_.paint.HaveShadow()) {
                curDraw->data_.paint.SetUICanvas(this);
            }
#endif
            curDraw->data_.DrawGraphics(gfxDstBuffer, param, curDraw->data_.paint, rect, trunc, *style_);
        }
    }
}

void UICanvas::OnBlendDraw(BufferInfo& gfxDstBuffer, const Rect& trunc)
{
    Rect rect = GetOrigRect();
    RenderBuffer renderBuffer;
    TransAffine transform;
    ListNode<DrawCmd>* curDrawEnd = drawCmdList_.Begin();
    RasterizerScanlineAntialias blendRasterizer;
    DrawCmd drawCmd;
    int count = 0;
    for (; curDrawEnd != drawCmdList_.End(); curDrawEnd = curDrawEnd->next_) {
        if (curDrawEnd->data_.paint.HaveComposite()) {
            drawCmd = curDrawEnd->data_;
            count++;
        }
    }
    if (drawCmd.param == nullptr) {
        return;
    }
    PathParam* pathParamBlend = static_cast<PathParam*>(drawCmd.param);
    ListNode<DrawCmd>* curDraw = drawCmdList_.Begin();
    InitRenderAndTransform(gfxDstBuffer, renderBuffer, rect, transform, *style_, curDraw->data_.paint);
    SetRasterizer(*pathParamBlend->vertices, drawCmd.paint, blendRasterizer, transform, pathParamBlend->isStroke);
    RasterizerScanlineAntialias scanline;
    RenderPixfmtRgbaBlend pixFormat(renderBuffer);
    RenderBase renBase(pixFormat);
    renBase.ResetClipping(true);
    renBase.ClipBox(trunc.GetLeft(), trunc.GetTop(), trunc.GetRight(), trunc.GetBottom());
    for (; curDraw != drawCmdList_.End(); curDraw = curDraw->next_) {
        if (curDraw->data_.paint.HaveComposite()) {
            drawCmd = curDraw->data_;
            count--;
        }
        if (count <= 0) {
            continue;
        }
        RasterizerScanlineAntialias rasterizer;
        if (curDraw->data_.param == nullptr) {
            continue;
        }
        PathParam* pathParam = static_cast<PathParam*>(curDraw->data_.param);
#if GRAPHIC_ENABLE_BLUR_EFFECT_FLAG
        if (curDraw->data_.paint.HaveShadow()) {
            curDraw->data_.paint.SetUICanvas(this);
            DoDrawShadow(gfxDstBuffer, curDraw->data_.param, curDraw->data_.paint, rect,
                         trunc, *style_, pathParam->isStroke);
        }
#endif
        InitRenderAndTransform(gfxDstBuffer, renderBuffer, rect, transform, *style_, curDraw->data_.paint);
        rasterizer.ClipBox(0, 0, gfxDstBuffer.width, gfxDstBuffer.height);
        SetRasterizer(*pathParam->vertices, curDraw->data_.paint, rasterizer, transform, pathParam->isStroke);
        if (IsSoild(curDraw->data_.paint)) {
            Rgba8T color;
            RenderBlendSolid(curDraw->data_.paint, color, pathParam->isStroke);
            SpanSoildColor spanSoildColor(color);
            BlendRaster(drawCmd.paint, drawCmd.param, blendRasterizer, rasterizer, renBase, transform,
                        spanSoildColor, rect, pathParamBlend->isStroke);
        }

        OnBlendDrawGradient(curDraw, drawCmd, trunc, blendRasterizer,
                            rasterizer, renBase, transform, pathParamBlend);

        OnBlendDrawPattern(curDraw, drawCmd, rect, trunc, blendRasterizer,
                           rasterizer, renBase, transform, pathParamBlend);
    }
}

void UICanvas::OnBlendDrawGradient(ListNode<DrawCmd>* curDraw, DrawCmd& drawCmd,
                                   const Rect& trunc,
                                   RasterizerScanlineAntialias& blendRasterizer,
                                   RasterizerScanlineAntialias& rasterizer,
                                   RenderBase& renBase,
                                   TransAffine& transform,
                                   PathParam* pathParamBlend)
{
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    if (curDraw->data_.paint.GetStyle() == Paint::GRADIENT) {
        TransAffine gradientMatrix;
        FillInterpolator interpolatorType(gradientMatrix);
        FillGradientLut gradientColorMode;
        BuildGradientColor(curDraw->data_.paint, gradientColorMode);
        if (curDraw->data_.paint.GetGradient() == Paint::Linear) {
            float distance = 0;
            BuildLineGradientMatrix(drawCmd.paint, gradientMatrix, transform, distance);
            GradientLinearCalculate gradientLinearCalculate;
            FillGradient span(interpolatorType, gradientLinearCalculate, gradientColorMode, 0, distance);
            BlendRaster(drawCmd.paint, drawCmd.param, blendRasterizer, rasterizer, renBase, transform,
                        span, trunc, pathParamBlend->isStroke);
        }
        if (curDraw->data_.paint.GetGradient() == Paint::Radial) {
            Paint::RadialGradientPoint radialPoint = drawCmd.paint.GetRadialGradientPoint();
            float startRadius = 0;
            float endRadius = 0;
            BuildRadialGradientMatrix(drawCmd.paint, gradientMatrix, transform, startRadius, endRadius);
            GradientRadialCalculate gradientRadialCalculate(endRadius, radialPoint.x0 - radialPoint.x1,
                                                            radialPoint.y0 - radialPoint.y1);
            FillGradient span(interpolatorType, gradientRadialCalculate, gradientColorMode,
                                    startRadius, endRadius);
            BlendRaster(drawCmd.paint, drawCmd.param, blendRasterizer, rasterizer, renBase, transform,
                        span, trunc, pathParamBlend->isStroke);
        }
    }
#endif
}

void UICanvas::OnBlendDrawPattern(ListNode<DrawCmd>* curDraw, DrawCmd& drawCmd,
                                  Rect& rect, const Rect& trunc,
                                  RasterizerScanlineAntialias& blendRasterizer,
                                  RasterizerScanlineAntialias& rasterizer,
                                  RenderBase& renBase,
                                  TransAffine& transform,
                                  PathParam* pathParamBlend)
{
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    if (curDraw->data_.paint.GetStyle() == Paint::PATTERN) {
        if (curDraw->data_.param == nullptr) {
            return;
        }
        PathParam* pathParam = static_cast<PathParam*>(curDraw->data_.param);
        ImageParam* imageParam = static_cast<ImageParam*>(pathParam->imageParam);
        if (imageParam->image == nullptr) {
            return;
        }
        FillPatternRgba spanPattern(imageParam->image->GetImageInfo(),
            curDraw->data_.paint.GetPatternRepeatMode(), rect.GetLeft(), rect.GetTop());
        BlendRaster(drawCmd.paint, drawCmd.param, blendRasterizer, rasterizer, renBase, transform,
                    spanPattern, trunc, pathParamBlend->isStroke);
    }
#endif
}

void UICanvas::GetAbsolutePosition(const Point& prePoint, const Rect& rect, const Style& style, Point& point)
{
    point.x = prePoint.x + rect.GetLeft() + style.paddingLeft_ + style.borderWidth_;
    point.y = prePoint.y + rect.GetTop() + style.paddingTop_ + style.borderWidth_;
}

void UICanvas::DoDrawLine(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    LineParam* lineParam = static_cast<LineParam*>(param);
    Point start;
    Point end;
    GetAbsolutePosition(lineParam->start, rect, style, start);
    GetAbsolutePosition(lineParam->end, rect, style, end);

    BaseGfxEngine::GetInstance()->DrawLine(gfxDstBuffer, start, end, invalidatedArea, paint.GetStrokeWidth(),
                                           paint.GetStrokeColor(), paint.GetOpacity());
}

void UICanvas::DoDrawCurve(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CurveParam* curveParam = static_cast<CurveParam*>(param);
    Point start;
    Point end;
    Point control1;
    Point control2;
    GetAbsolutePosition(curveParam->start, rect, style, start);
    GetAbsolutePosition(curveParam->end, rect, style, end);
    GetAbsolutePosition(curveParam->control1, rect, style, control1);
    GetAbsolutePosition(curveParam->control2, rect, style, control2);

    BaseGfxEngine::GetInstance()->DrawCubicBezier(gfxDstBuffer,
                                                  start,
                                                  control1,
                                                  control2,
                                                  end,
                                                  invalidatedArea,
                                                  paint.GetStrokeWidth(),
                                                  paint.GetStrokeColor(),
                                                  paint.GetOpacity());
}

void UICanvas::DoDrawRect(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    RectParam* rectParam = static_cast<RectParam*>(param);
    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.bgColor_ = paint.GetStrokeColor();
    drawStyle.bgOpa_ = paint.GetOpacity();
    drawStyle.borderRadius_ = 0;

    int16_t lineWidth = static_cast<int16_t>(paint.GetStrokeWidth());
    Point start;
    GetAbsolutePosition(rectParam->start, rect, style, start);

    int16_t x = start.x - lineWidth / 2; // 2: half
    int16_t y = start.y - lineWidth / 2; // 2: half
    Rect coords;
    if ((rectParam->height <= lineWidth) || (rectParam->width <= lineWidth)) {
        coords.SetPosition(x, y);
        coords.SetHeight(rectParam->height + lineWidth);
        coords.SetWidth(rectParam->width + lineWidth);
        BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
        return;
    }

    coords.SetPosition(x, y);
    coords.SetHeight(lineWidth);
    coords.SetWidth(rectParam->width);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x + rectParam->width, y);
    coords.SetHeight(rectParam->height);
    coords.SetWidth(lineWidth);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x, y + lineWidth);
    coords.SetHeight(rectParam->height);
    coords.SetWidth(lineWidth);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);

    coords.SetPosition(x + lineWidth, y + rectParam->height);
    coords.SetHeight(lineWidth);
    coords.SetWidth(rectParam->width);
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
}

void UICanvas::DoFillRect(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    if (param == nullptr) {
        return;
    }
    RectParam* rectParam = static_cast<RectParam*>(param);
    uint8_t enableStroke = static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE;
    int16_t lineWidth = enableStroke ? paint.GetStrokeWidth() : 0;
    if ((rectParam->height <= lineWidth) || (rectParam->width <= lineWidth)) {
        return;
    }
    Point start;
    GetAbsolutePosition(rectParam->start, rect, style, start);

    Rect coords;
    coords.SetPosition(start.x + (lineWidth + 1) / 2, start.y + (lineWidth + 1) / 2); // 2: half
    coords.SetHeight(rectParam->height - lineWidth);
    coords.SetWidth(rectParam->width - lineWidth);

    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.bgColor_ = paint.GetFillColor();
    drawStyle.bgOpa_ = paint.GetOpacity();
    drawStyle.borderRadius_ = 0;
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, coords, invalidatedArea, drawStyle, OPA_OPAQUE);
}

void UICanvas::DoDrawCircle(BufferInfo& gfxDstBuffer,
                            void* param,
                            const Paint& paint,
                            const Rect& rect,
                            const Rect& invalidatedArea,
                            const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CircleParam* circleParam = static_cast<CircleParam*>(param);

    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.lineOpa_ = paint.GetOpacity();

    ArcInfo arcInfo = {{0}};
    arcInfo.imgPos = {0, 0};
    arcInfo.startAngle = 0;
    arcInfo.endAngle = CIRCLE_IN_DEGREE;
    GetAbsolutePosition(circleParam->center, rect, style, arcInfo.center);
    uint8_t enableStroke = static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::STROKE_STYLE;
    uint16_t halfLineWidth = enableStroke ? (paint.GetStrokeWidth() >> 1) : 0;
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        arcInfo.radius = circleParam->radius - halfLineWidth;
        drawStyle.lineWidth_ = arcInfo.radius;
        drawStyle.lineColor_ = paint.GetFillColor();
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }

    if (enableStroke) {
        arcInfo.radius = circleParam->radius + halfLineWidth - 1;
        drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
        drawStyle.lineColor_ = paint.GetStrokeColor();
        BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                              CapType::CAP_NONE);
    }
}

void UICanvas::DoDrawArc(BufferInfo& gfxDstBuffer,
                         void* param,
                         const Paint& paint,
                         const Rect& rect,
                         const Rect& invalidatedArea,
                         const Style& style)
{
    if (param == nullptr) {
        return;
    }
    ArcParam* arcParam = static_cast<ArcParam*>(param);

    ArcInfo arcInfo = {{0}};
    arcInfo.imgPos = {0, 0};
    arcInfo.startAngle = arcParam->startAngle;
    arcInfo.endAngle = arcParam->endAngle;
    Style drawStyle = StyleDefault::GetDefaultStyle();
    drawStyle.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
    drawStyle.lineColor_ = paint.GetStrokeColor();
    drawStyle.lineOpa_ = paint.GetOpacity();
    arcInfo.radius = arcParam->radius + ((paint.GetStrokeWidth() + 1) >> 1);

    GetAbsolutePosition(arcParam->center, rect, style, arcInfo.center);
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcInfo, invalidatedArea, drawStyle, OPA_OPAQUE,
                                          CapType::CAP_NONE);
}
#if GRAPHIC_ENABLE_DRAW_IMAGE_FLAG
void UICanvas::DoDrawImage(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    UIImageView* imageView = static_cast<UIImageView*>(param);
    Point startPos = {imageView->GetX(), imageView->GetY()};
    Point start;
    GetAbsolutePosition({startPos.x, startPos.y}, rect, style, start);
    imageView->SetPosition(start.x, start.y);
    if (!paint.GetTransAffine().IsIdentity()) {
        float angle = paint.GetRotateAngle();
        imageView->Rotate(MATH_ROUND(angle),Vector2<float>(0, 0));
        imageView->Translate(Vector3<int16_t>(paint.GetTranslateX(), paint.GetTranslateY(), 1));
        Vector2<float> scale(static_cast<float>(paint.GetScaleX()), static_cast<float>(paint.GetScaleY()));
        imageView->Scale(scale, Vector2<float>(0, 0));
    }
    imageView->OnDraw(gfxDstBuffer, invalidatedArea);
    imageView->SetPosition(startPos.x, startPos.y);
}
#endif

void UICanvas::DoDrawLabel(BufferInfo& gfxDstBuffer,
                           void* param,
                           const Paint& paint,
                           const Rect& rect,
                           const Rect& invalidatedArea,
                           const Style& style)
{
    if (param == nullptr) {
        return;
    }
    UILabel* label = static_cast<UILabel*>(param);
    Point startPos = {label->GetX(), label->GetY()};
    Point start;
    GetAbsolutePosition({startPos.x, startPos.y}, rect, style, start);
    label->SetPosition(start.x, start.y);
    label->OnDraw(gfxDstBuffer, invalidatedArea);
    label->SetPosition(startPos.x, startPos.y);
}

void UICanvas::DoDrawLineJoin(BufferInfo& gfxDstBuffer,
                              const Point& center,
                              const Rect& invalidatedArea,
                              const Paint& paint)
{
    ArcInfo arcinfo = {{0}};
    arcinfo.center = center;
    arcinfo.imgPos = {0, 0};
    arcinfo.radius = (paint.GetStrokeWidth() + 1) >> 1;
    arcinfo.startAngle = 0;
    arcinfo.endAngle = CIRCLE_IN_DEGREE;

    Style style;
    style.lineColor_ = paint.GetStrokeColor();
    style.lineWidth_ = static_cast<int16_t>(paint.GetStrokeWidth());
    style.lineOpa_ = OPA_OPAQUE;
    BaseGfxEngine::GetInstance()->DrawArc(gfxDstBuffer, arcinfo, invalidatedArea,
                                          style, OPA_OPAQUE, CapType::CAP_NONE);
}

void UICanvas::DoDrawPath(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    DoRender(gfxDstBuffer, param, paint, rect, invalidatedArea, style, true);
}

void UICanvas::DoFillPath(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    DoRender(gfxDstBuffer, param, paint, rect, invalidatedArea, style, false);
}

void UICanvas::SetRasterizer(UICanvasVertices& vertices,
                             const Paint& paint,
                             RasterizerScanlineAntialias& rasterizer,
                             TransAffine& transform,
                             const bool& isStroke)
{
    DepictCurve canvasPath(vertices);
    if (isStroke) {
#if GRAPHIC_ENABLE_DASH_GENERATE_FLAG
        if (paint.IsLineDash()) {
            using DashStyle = DepictDash;
            using StrokeDashStyle = DepictStroke<DashStyle>;
            using StrokeDashTransform = DepictTransform<StrokeDashStyle>;
            DashStyle dashStyle(canvasPath);
            LineDashStyleCalc(dashStyle, paint);
            StrokeDashStyle strokeDashStyle(dashStyle);
            LineStyleCalc(strokeDashStyle, paint);
            StrokeDashTransform strokeDashTransform(strokeDashStyle, transform);
            rasterizer.Reset();
            rasterizer.AddPath(strokeDashTransform);
            return;
        }
#endif
        using StrokeLineStyle = DepictStroke<DepictCurve>;
        StrokeLineStyle strokeLineStyle(canvasPath);
        LineStyleCalc(strokeLineStyle, paint);

        DepictTransform<StrokeLineStyle> strokeTransform(strokeLineStyle, transform);
        rasterizer.Reset();
        rasterizer.AddPath(strokeTransform);
    } else {
        DepictTransform<DepictCurve> pathTransform(canvasPath, transform);
        rasterizer.Reset();
        rasterizer.AddPath(pathTransform);
    }
}

void UICanvas::DoRender(BufferInfo& gfxDstBuffer,
                        void* param,
                        const Paint& paint,
                        const Rect& rect,
                        const Rect& invalidatedArea,
                        const Style& style,
                        const bool& isStroke)
{
    if (param == nullptr) {
        return;
    }
#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    if (paint.HaveShadow()) {
        DoDrawShadow(gfxDstBuffer, param, paint, rect, invalidatedArea, style, isStroke);
    }
#endif
    TransAffine transform;
    RenderBuffer renderBuffer;
    InitRenderAndTransform(gfxDstBuffer, renderBuffer, rect, transform, style, paint);

    RasterizerScanlineAntialias rasterizer;
    GeometryScanline scanline;

    PathParam* pathParam = static_cast<PathParam*>(param);
    rasterizer.ClipBox(0, 0, gfxDstBuffer.width, gfxDstBuffer.height);
    SetRasterizer(*pathParam->vertices, paint, rasterizer, transform, isStroke);

    RenderPixfmtRgbaBlend pixFormat(renderBuffer);
    RenderBase renBase(pixFormat);
    FillBase allocator;

    renBase.ResetClipping(true);
    renBase.ClipBox(invalidatedArea.GetLeft(), invalidatedArea.GetTop(), invalidatedArea.GetRight(),
                    invalidatedArea.GetBottom());

    if (paint.GetStyle() == Paint::STROKE_STYLE || paint.GetStyle() == Paint::FILL_STYLE ||
        paint.GetStyle() == Paint::STROKE_FILL_STYLE) {
        RenderSolid(paint, rasterizer, renBase, isStroke);
    }

#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    if (paint.GetStyle() == Paint::GRADIENT) {
        RenderGradient(paint, rasterizer, transform, renBase, renderBuffer, allocator, invalidatedArea);
    }
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    if (paint.GetStyle() == Paint::PATTERN) {
        RenderPattern(paint, pathParam->imageParam, rasterizer, renBase, allocator, rect);
    }
#endif
}

#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
void UICanvas::DoDrawShadow(BufferInfo& gfxDstBuffer,
                            void* param,
                            const Paint& paint,
                            const Rect& rect,
                            const Rect& invalidatedArea,
                            const Style& style,
                            const bool& isStroke)
{
    if (param == nullptr) {
        return;
    }

#if GRAPHIC_ENABLE_SHADOW_EFFECT_FLAG
    TransAffine transform;
    RenderBuffer renderBuffer;
    InitRenderAndTransform(gfxDstBuffer, renderBuffer, rect, transform, style, paint);

    transform.Translate(paint.GetShadowOffsetX(), paint.GetShadowOffsetY());

    RasterizerScanlineAntialias rasterizer;
    GeometryScanline scanline;
    PathParam* pathParam = static_cast<PathParam*>(param);
    rasterizer.ClipBox(0, 0, gfxDstBuffer.width, gfxDstBuffer.height);
    SetRasterizer(*pathParam->vertices, paint, rasterizer, transform, isStroke);
    Rect bbox(rasterizer.GetMinX(), rasterizer.GetMinY(), rasterizer.GetMaxX(), rasterizer.GetMaxY());

    RenderPixfmtRgbaBlend pixFormat(renderBuffer);
    RenderBase renBase(pixFormat);
    FillBase allocator;

    renBase.ResetClipping(true);
    renBase.ClipBox(invalidatedArea.GetLeft(), invalidatedArea.GetTop(), invalidatedArea.GetRight(),
                    invalidatedArea.GetBottom());

    Rgba8T shadowColor;
    uint8_t paintAlpha = 0xff;
    #if COLOR_DEPTH==32
    paintAlpha = paint.GetShadowColor().alpha;
    #endif
    ChangeColor(shadowColor, paint.GetShadowColor(), paintAlpha * paint.GetGlobalAlpha());

    RenderScanlinesAntiAliasSolid(rasterizer, scanline, renBase, shadowColor);
#if GRAPHIC_ENABLE_BLUR_EFFECT_FLAG
    bbox.SetLeft(bbox.GetLeft() - paint.GetShadowBlur());
    bbox.SetTop(bbox.GetTop() - paint.GetShadowBlur());
    bbox.SetRight(bbox.GetRight() + paint.GetShadowBlur());
    bbox.SetBottom(bbox.GetBottom() + paint.GetShadowBlur());
    RenderBuffer shadowBuffer;
    RenderPixfmtRgbaBlend pixf2(shadowBuffer);
    Rect shadowRect = {int16_t(bbox.GetLeft()), int16_t(bbox.GetTop()),
                       int16_t(bbox.GetRight()), int16_t(bbox.GetBottom())};
    shadowRect.Intersect(shadowRect, invalidatedArea);
    pixf2.Attach(pixFormat, shadowRect.GetLeft(), shadowRect.GetTop(),
                 shadowRect.GetRight(), shadowRect.GetBottom());
    uint8_t pixelByteSize = DrawUtils::GetPxSizeByColorMode(gfxDstBuffer.mode) >> 3; // 3: Shift right 3 bits

    paint.GetUICanvas()->GetDrawBoxBlur().BoxBlur(pixf2, MATH_UROUND(paint.GetShadowBlur()),
                                                  pixelByteSize, gfxDstBuffer.stride);

#endif
#endif
}
#endif
/**
 * Renders monochrome polygon paths and fills
 */
void RenderSolid(const Paint& paint,
                 RasterizerScanlineAntialias& rasterizer,
                 RenderBase& renBase,
                 const bool& isStroke)
{
    GeometryScanline scanline;
    Rgba8T color;
    UICanvas::RenderBlendSolid(paint, color, isStroke);
    RenderScanlinesAntiAliasSolid(rasterizer, scanline, renBase, color);
}

void InitRenderAndTransform(BufferInfo& gfxDstBuffer,
                            RenderBuffer& renderBuffer,
                            const Rect& rect,
                            TransAffine& transform,
                            const Style& style,
                            const Paint& paint)
{
    int16_t realLeft = rect.GetLeft() + style.paddingLeft_ + style.borderWidth_;
    int16_t realTop = rect.GetTop() + style.paddingTop_ + style.borderWidth_;
    transform.Reset();
    transform *= paint.GetTransAffine();
    transform.Translate(realLeft, realTop);
    renderBuffer.Attach(static_cast<uint8_t*>(gfxDstBuffer.virAddr), gfxDstBuffer.width, gfxDstBuffer.height,
                        gfxDstBuffer.stride);
}

#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
void UICanvas::StrokeText(const char* text, const Point& point, const FontStyle& fontStyle, const Paint& paint)
{
    if (text == nullptr) {
        return;
    }
    if (static_cast<uint8_t>(paint.GetStyle()) & Paint::PaintStyle::FILL_STYLE) {
        TextParam* textParam = new TextParam;
        if (textParam == nullptr) {
            GRAPHIC_LOGE("new TextParam fail");
            return;
        }
        textParam->text = text;
        textParam->fontStyle = fontStyle;
        textParam->fontOpa = paint.GetOpacity();
        textParam->fontColor = paint.GetFillColor();
        textParam->position = point;
        DrawCmd cmd;
        cmd.param = textParam;
        cmd.DeleteParam = DeleteTextParam;
        cmd.DrawGraphics = DoDrawText;
        cmd.paint = paint;
        cmd.paint.SetUICanvas(this);
        drawCmdList_.PushBack(cmd);
        Invalidate();
        SetStartPosition(point);
    }
}
#endif

Point UICanvas::MeasureText(const char* text, const FontStyle& fontStyle)
{
    Text* textCompent = new Text;
    textCompent->SetText(text);
    textCompent->SetFont(fontStyle.fontName, fontStyle.fontSize);
    textCompent->SetDirect(static_cast<UITextLanguageDirect>(fontStyle.direct));
    textCompent->SetAlign(static_cast<UITextLanguageAlignment>(fontStyle.align));
    Style drawStyle;
    drawStyle.SetStyle(STYLE_LETTER_SPACE, fontStyle.letterSpace);
    textCompent->ReMeasureTextSize(this->GetRect(), drawStyle);
    Point textSize = textCompent->GetTextSize();
    delete textCompent;
    return textSize;
}

void UICanvas::BlitMapBuffer(BufferInfo &gfxDstBuffer, BufferInfo& gfxMapBuffer, Rect& textRect,
                             TransformMap& transMap, const Rect& invalidatedArea)
{
    Rect invalidRect = textRect;
    transMap.SetTransMapRect(textRect);
    if (invalidRect.Intersect(invalidRect, transMap.GetBoxRect())) {
        uint8_t pxSize = DrawUtils::GetPxSizeByColorMode(gfxDstBuffer.mode);
        ImageInfo imageInfo;
        imageInfo.header.colorMode = gfxDstBuffer.mode;
        imageInfo.dataSize = gfxMapBuffer.width * gfxMapBuffer.height *
                DrawUtils::GetByteSizeByColorMode(gfxDstBuffer.mode);
        imageInfo.header.width = gfxMapBuffer.width;
        imageInfo.header.height = gfxMapBuffer.height;
        imageInfo.header.reserved = 0;
        uint8_t* addr = reinterpret_cast<uint8_t*>(gfxMapBuffer.virAddr);
        imageInfo.data = addr;
        TransformDataInfo imageTranDataInfo = {imageInfo.header, imageInfo.data, pxSize,
                                               BlurLevel::LEVEL0, TransformAlgorithm::BILINEAR};
        BaseGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, invalidatedArea, {0, 0}, Color::Black(),
                                                    OPA_OPAQUE, transMap, imageTranDataInfo);
    }
}

#if GRAPHIC_ENABLE_DRAW_TEXT_FLAG
void UICanvas::DoDrawText(BufferInfo& gfxDstBuffer,
                          void* param,
                          const Paint& paint,
                          const Rect& rect,
                          const Rect& invalidatedArea,
                          const Style& style)
{
    TextParam* textParam = static_cast<TextParam*>(param);
    if (textParam == nullptr) {
        return;
    }
    if (textParam->fontStyle.fontSize <= 0) {
        return;
    }
    Text* text = textParam->textComment;
    text->SetText(textParam->text);
    text->SetFont(textParam->fontStyle.fontName, textParam->fontStyle.fontSize);
    text->SetDirect(static_cast<UITextLanguageDirect>(textParam->fontStyle.direct));
    text->SetAlign(static_cast<UITextLanguageAlignment>(textParam->fontStyle.align));

    Point start;
    Rect textRect = invalidatedArea;
    GetAbsolutePosition(textParam->position, rect, style, start);
    textRect.SetPosition(start.x, start.y);
    Style drawStyle = style;
    drawStyle.textColor_ = textParam->fontColor;
    drawStyle.lineColor_ = textParam->fontColor;
    drawStyle.bgColor_ = textParam->fontColor;
    drawStyle.SetStyle(STYLE_LETTER_SPACE, textParam->fontStyle.letterSpace);
    text->ReMeasureTextSize(textRect, drawStyle);
    if (text->GetTextSize().x == 0 || text->GetTextSize().y == 0) {
        return;
    }
    textRect.SetWidth(text->GetTextSize().x + 1);
    textRect.SetHeight(text->GetTextSize().y + 1);
    OpacityType opa = DrawUtils::GetMixOpacity(textParam->fontOpa, style.bgOpa_);
    if (!paint.GetTransAffine().IsIdentity()) {
        Rect textImageRect(0, 0, textRect.GetWidth(), textRect.GetHeight());
        if (paint.GetUICanvas() == nullptr) {
            return;
        }
        BufferInfo* mapBufferInfo = paint.GetUICanvas()->UpdateMapBufferInfo(gfxDstBuffer, textImageRect);
        text->OnDraw(*mapBufferInfo, textImageRect, textImageRect, textImageRect, 0, drawStyle,
                     Text::TEXT_ELLIPSIS_END_INV, opa);
        TransformMap trans;
        trans.SetTransMapRect(textRect);
        trans.Scale(Vector2<float>(static_cast<float>(paint.GetScaleX()), static_cast<float>(paint.GetScaleY())),
                    Vector2<float>(0, 0));
        float angle = paint.GetRotateAngle();
        trans.Rotate(MATH_ROUND(angle),  Vector2<float>(0, 0));
        trans.Translate(Vector2<int16_t>(paint.GetTranslateX(), paint.GetTranslateY()));
        BlitMapBuffer(gfxDstBuffer, *mapBufferInfo, textRect, trans, invalidatedArea);
    } else {
        text->OnDraw(gfxDstBuffer, invalidatedArea, textRect, textRect, 0,
                     drawStyle, Text::TEXT_ELLIPSIS_END_INV, opa);
    }
}
#endif
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
void UICanvas::RenderGradient(const Paint &paint, RasterizerScanlineAntialias &rasterizer,
                              TransAffine &transform, RenderBase &renBase,
                              RenderBuffer &renderBuffer, FillBase &allocator,
                              const Rect &invalidatedArea)
{
    GeometryScanline scanline;

    RenderPixfmtRgbaBlend pixFormatComp(renderBuffer);
    RenderBase m_renBaseComp(pixFormatComp);

    m_renBaseComp.ResetClipping(true);
    m_renBaseComp.ClipBox(invalidatedArea.GetLeft(), invalidatedArea.GetTop(),
                          invalidatedArea.GetRight(), invalidatedArea.GetBottom());
    TransAffine gradientMatrix;
    FillInterpolator interpolatorType(gradientMatrix);
    FillGradientLut gradientColorMode;
    BuildGradientColor(paint, gradientColorMode);
    if (paint.GetGradient() == Paint::Linear) {
        float distance = 0;
        BuildLineGradientMatrix(paint, gradientMatrix, transform, distance);
        GradientLinearCalculate gradientLinearCalculate;
        FillGradient span(interpolatorType, gradientLinearCalculate, gradientColorMode, 0, distance);
        RenderScanlinesAntiAlias(rasterizer, scanline, renBase, allocator, span);
    }

    if (paint.GetGradient() == Paint::Radial) {
        Paint::RadialGradientPoint radialPoint = paint.GetRadialGradientPoint();
        float startRadius = 0;
        float endRadius = 0;
        BuildRadialGradientMatrix(paint, gradientMatrix, transform, startRadius, endRadius);
        GradientRadialCalculate gradientRadialCalculate(radialPoint.r1, radialPoint.x0 - radialPoint.x1,
                                                        radialPoint.y0 - radialPoint.y1);
        FillGradient span(interpolatorType, gradientRadialCalculate, gradientColorMode,
                                startRadius, endRadius);
        RenderScanlinesAntiAlias(rasterizer, scanline, renBase, allocator, span);
    }
}

void UICanvas::BuildGradientColor(const Paint &paint, FillGradientLut &gradientColorMode)
{
    gradientColorMode.RemoveAll();
    ListNode<Paint::StopAndColor>* iter = paint.getStopAndColor().Begin();
    uint16_t count = 0;
    for (; count < paint.getStopAndColor().Size(); count++) {
        ColorType stopColor = iter->data_.color;
        Rgba8T sRgba8;
        uint8_t stopColorAlpha = 0xff;
        #if COLOR_DEPTH==32
        stopColorAlpha = stopColor.alpha;
        #endif
        ChangeColor(sRgba8, stopColor, stopColorAlpha * paint.GetGlobalAlpha());
        gradientColorMode.AddColor(iter->data_.stop, sRgba8);
        iter = iter->next_;
    }
    gradientColorMode.BuildLut();
}

void UICanvas::BuildRadialGradientMatrix(const Paint &paint, TransAffine &gradientMatrix,
                                         TransAffine &transform, float &startRadius,
                                         float &endRadius)
{
    Paint::RadialGradientPoint radialPoint = paint.GetRadialGradientPoint();
    gradientMatrix.Reset();
    gradientMatrix *= TransAffine::TransAffineTranslation(radialPoint.x1, radialPoint.y1);
    gradientMatrix *= transform;
    gradientMatrix.Invert();
    startRadius = radialPoint.r0;
    endRadius = radialPoint.r1;
}
#endif

#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
void UICanvas::RenderPattern(const Paint &paint, void *param,
                             RasterizerScanlineAntialias &rasterizer,
                             RenderBase &renBase, FillBase &allocator,
                             const Rect &rect)
{
    if (param == nullptr) {
        return;
    }
    ImageParam* imageParam = static_cast<ImageParam*>(param);
    if (imageParam->image == nullptr) {
        return;
    }
    GeometryScanline scanline;
    FillPatternRgba spanPattern(imageParam->image->GetImageInfo(),
        paint.GetPatternRepeatMode(), rect.GetLeft(), rect.GetTop());
    RenderScanlinesAntiAlias(rasterizer, scanline, renBase, allocator, spanPattern);
}
#endif

void UICanvas::InitGfxMapBuffer(const BufferInfo& srcBuff, const Rect& rect)
{
    gfxMapBuffer_ = new BufferInfo();
    gfxMapBuffer_->rect = rect;
    gfxMapBuffer_->mode = srcBuff.mode;
    gfxMapBuffer_->color = srcBuff.color;
    gfxMapBuffer_->width = static_cast<uint16_t>(rect.GetWidth());
    gfxMapBuffer_->height = static_cast<uint16_t>(rect.GetHeight());
    uint8_t destByteSize = DrawUtils::GetByteSizeByColorMode(srcBuff.mode);
    gfxMapBuffer_->stride = static_cast<int32_t>(gfxMapBuffer_->width) * static_cast<int32_t>(destByteSize);
    uint32_t buffSize = gfxMapBuffer_->height * gfxMapBuffer_->stride;
    gfxMapBuffer_->virAddr = UIMalloc(buffSize);
    gfxMapBuffer_->phyAddr = gfxMapBuffer_->virAddr;

    errno_t err = memset_s(gfxMapBuffer_->virAddr, buffSize, 0, buffSize);
    if (err != EOK) {
        BaseGfxEngine::GetInstance()->FreeBuffer((uint8_t*)gfxMapBuffer_->virAddr);
        GRAPHIC_LOGE("memset_s gfxMapBuffer_ fail");
        return;
    }
}

BufferInfo* UICanvas::UpdateMapBufferInfo(const BufferInfo& srcBuff, const Rect& rect)
{
    if (gfxMapBuffer_ == nullptr) {
        InitGfxMapBuffer(srcBuff, rect);
        return gfxMapBuffer_;
    }

    if (rect.GetWidth() != gfxMapBuffer_->width ||
        rect.GetHeight() != gfxMapBuffer_->height ||
        srcBuff.mode != gfxMapBuffer_->mode) {
        DestroyMapBufferInfo();
        InitGfxMapBuffer(srcBuff, rect);
    } else {
        uint32_t buffSize = gfxMapBuffer_->height * gfxMapBuffer_->stride;
        errno_t err = memset_s(gfxMapBuffer_->virAddr, buffSize, 0, buffSize);
        if (err != EOK) {
            BaseGfxEngine::GetInstance()->FreeBuffer((uint8_t*)gfxMapBuffer_->virAddr);
            GRAPHIC_LOGE("memset_s gfxMapBuffer_ fail");
        }
    }
    return gfxMapBuffer_;
}

void UICanvas::DestroyMapBufferInfo()
{
    if (gfxMapBuffer_ != nullptr) {
        BaseGfxEngine::GetInstance()->FreeBuffer(static_cast<uint8_t*>(gfxMapBuffer_->virAddr));
        gfxMapBuffer_->virAddr = nullptr;
        gfxMapBuffer_->phyAddr = nullptr;
        delete gfxMapBuffer_;
        gfxMapBuffer_ = nullptr;
    }
}

void UICanvas::BlendRaster(const Paint& paint,
                           void* param,
                           RasterizerScanlineAntialias& blendRasterizer,
                           RasterizerScanlineAntialias& rasterizer,
                           RenderBase& renBase,
                           TransAffine& transform,
                           SpanBase& spanGen,
                           const Rect& rect,
                           bool isStroke)
{
    TransAffine gradientMatrixBlend;
    GeometryScanline scanline1;
    GeometryScanline scanline2;
    FillBase allocator1;

    if (IsSoild(paint)) {
        Rgba8T blendColor;
        RenderBlendSolid(paint, blendColor, isStroke);
        SpanSoildColor spanBlendSoildColor(blendColor);
        BlendScanLine(paint.GetGlobalCompositeOperation(), blendRasterizer, rasterizer,
                      scanline1, scanline2, renBase, allocator1, spanBlendSoildColor, spanGen);
    }
#if GRAPHIC_ENABLE_GRADIENT_FILL_FLAG
    FillInterpolator interpolatorTypeBlend(gradientMatrixBlend);
    FillGradientLut gradientColorModeBlend;
    if (paint.GetStyle() == Paint::GRADIENT) {
        BuildGradientColor(paint, gradientColorModeBlend);
        if (paint.GetGradient() == Paint::Linear) {
            float distance = 0;
            BuildLineGradientMatrix(paint, gradientMatrixBlend, transform, distance);
            GradientLinearCalculate gradientLinearCalculate;
            FillGradient span(interpolatorTypeBlend, gradientLinearCalculate,
                                    gradientColorModeBlend, 0, distance);
            BlendScanLine(paint.GetGlobalCompositeOperation(), blendRasterizer, rasterizer,
                          scanline1, scanline2, renBase, allocator1, span, spanGen);
        } else if (paint.GetGradient() == Paint::Radial) {
            Paint::RadialGradientPoint radialPoint = paint.GetRadialGradientPoint();
            float startRadius = 0;
            float endRadius = 0;
            BuildRadialGradientMatrix(paint, gradientMatrixBlend, transform, startRadius, endRadius);
            GradientRadialCalculate gradientRadialCalculate(endRadius, radialPoint.x0 - radialPoint.x1,
                                                            radialPoint.y0 - radialPoint.y1);
            FillGradient span(interpolatorTypeBlend, gradientRadialCalculate, gradientColorModeBlend,
                                    startRadius, endRadius);
            BlendScanLine(paint.GetGlobalCompositeOperation(), blendRasterizer, rasterizer,
                          scanline1, scanline2, renBase, allocator1, span, spanGen);
        }
    }
#endif
#if GRAPHIC_ENABLE_PATTERN_FILL_FLAG
    if (paint.GetStyle() == Paint::PATTERN) {
        if (param == nullptr) {
            return;
        }

        PathParam* pathParam = static_cast<PathParam*>(param);

        ImageParam* imageParam = static_cast<ImageParam*>(pathParam->imageParam);

        if (imageParam->image == nullptr) {
            return;
        }
        FillPatternRgba spanPattern(imageParam->image->GetImageInfo(),
            paint.GetPatternRepeatMode(), rect.GetLeft(), rect.GetTop());
        BlendScanLine(paint.GetGlobalCompositeOperation(), blendRasterizer, rasterizer,
                      scanline1, scanline2, renBase, allocator1, spanPattern, spanGen);
    }
#endif
}
} // namespace OHOS
