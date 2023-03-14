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

#include "core/components/common/properties/decoration.h"

namespace OHOS::Ace {

void Decoration::SetContextAndCallback(
    const WeakPtr<PipelineContext>& context, const RenderNodeAnimationCallback& callback)
{
    backgroundColor_.SetContextAndCallback(context, callback);
    border_.SetContextAndCallback(context, callback);
    blurRadius_.SetContextAndCallback(context, callback);
}

void Decoration::AddShadow(const Shadow& shadow)
{
    shadows_.push_back(shadow);
}

void Decoration::ClearAllShadow()
{
    shadows_.clear();
}

Size Decoration::GetOccupiedSize(double dipScale) const
{
    return border_.GetLayoutSize(dipScale) + padding_.GetLayoutSizeInPx(dipScale);
}

Offset Decoration::GetOffset(double dipScale) const
{
    return border_.GetOffset(dipScale) + padding_.GetOffsetInPx(dipScale);
}

double Decoration::VerticalSpaceOccupied(double dipScale) const
{
    return border_.VerticalWidth(dipScale) + padding_.VerticalInPx(dipScale);
}

double Decoration::HorizontalSpaceOccupied(double dipScale) const
{
    return border_.HorizontalWidth(dipScale) + padding_.HorizontalInPx(dipScale);
}

void Gradient::AddColor(const GradientColor& color)
{
    colors_.push_back(color);
}

void Gradient::ClearColors()
{
    colors_.clear();
}

void BackgroundImageSize::SetSizeTypeX(BackgroundImageSizeType type)
{
    typeX_ = type;
}

void BackgroundImageSize::SetSizeTypeY(BackgroundImageSizeType type)
{
    if (type == BackgroundImageSizeType::CONTAIN || type == BackgroundImageSizeType::COVER) {
        return;
    }
    typeY_ = type;
}

void BackgroundImageSize::SetSizeValueX(double value)
{
    if (value < -0.0) {
        return;
    }
    valueX_ = value;
}

void BackgroundImageSize::SetSizeValueY(double value)
{
    if (value < -0.0) {
        return;
    }
    valueY_ = value;
}

bool BackgroundImageSize::IsValid() const
{
    if (typeY_ == BackgroundImageSizeType::CONTAIN || typeY_ == BackgroundImageSizeType::COVER) {
        return false;
    }

    if ((typeX_ == BackgroundImageSizeType::LENGTH || typeX_ == BackgroundImageSizeType::PERCENT) &&
        LessOrEqual(valueX_, 0.0)) {
        return false;
    }

    if ((typeY_ == BackgroundImageSizeType::LENGTH || typeY_ == BackgroundImageSizeType::PERCENT) &&
        LessOrEqual(valueY_, 0.0)) {
        return false;
    }

    return true;
}

BackgroundImageSizeType BackgroundImageSize::GetSizeTypeX() const
{
    return typeX_;
}

BackgroundImageSizeType BackgroundImageSize::GetSizeTypeY() const
{
    return typeY_;
}

double BackgroundImageSize::GetSizeValueX() const
{
    return valueX_;
}

double BackgroundImageSize::GetSizeValueY() const
{
    return valueY_;
}

BackgroundImageSize BackgroundImageSize::operator+(const BackgroundImageSize& rhs) const
{
    if ((rhs.GetSizeTypeX() != GetSizeTypeX()) || (rhs.GetSizeTypeY() != GetSizeTypeY())) {
        // error: unit not same, just return lhs value
        return *this;
    }
    auto rhsX = rhs.GetSizeValueX();
    auto rhsY = rhs.GetSizeValueY();
    auto lhsX = GetSizeValueX();
    auto lhsY = GetSizeValueY();
    BackgroundImageSize size;
    size.SetSizeValueX(rhsX + lhsX);
    size.SetSizeTypeX(GetSizeTypeX());
    size.SetSizeValueY(rhsY + lhsY);
    size.SetSizeTypeY(GetSizeTypeY());
    return size;
}

BackgroundImageSize BackgroundImageSize::operator-(const BackgroundImageSize& rhs) const
{
    auto rhsX = rhs.GetSizeValueX();
    auto rhsY = rhs.GetSizeValueY();
    auto lhsX = GetSizeValueX();
    auto lhsY = GetSizeValueY();
    if ((rhs.GetSizeTypeX() != GetSizeTypeX()) || (rhs.GetSizeTypeY() != GetSizeTypeY())) {
        // error: unit not same, just return lhs value
        return *this;
    }
    BackgroundImageSize size;
    size.SetSizeValueX(lhsX - rhsX);
    size.SetSizeTypeX(GetSizeTypeX());
    size.SetSizeValueY(lhsY - rhsY);
    size.SetSizeTypeY(GetSizeTypeY());
    return size;
}

BackgroundImageSize BackgroundImageSize::operator*(double value) const
{
    BackgroundImageSize size;
    size.SetSizeValueX(GetSizeValueX() * value);
    size.SetSizeTypeX(GetSizeTypeX());
    size.SetSizeValueY(GetSizeValueY() * value);
    size.SetSizeTypeY(GetSizeTypeY());
    return size;
}

bool BackgroundImageSize::operator==(const BackgroundImageSize& size) const
{
    return typeX_ == size.GetSizeTypeX() && NearEqual(valueX_, size.GetSizeValueX()) && typeY_ == size.GetSizeTypeY() &&
           NearEqual(valueY_, size.GetSizeValueY());
}

bool BackgroundImageSize::operator!=(const BackgroundImageSize& size) const
{
    return !operator==(size);
}

BackgroundImagePosition BackgroundImagePosition::operator+(const BackgroundImagePosition& rhs) const
{
    auto rhsX = rhs.GetSizeValueX();
    auto rhsY = rhs.GetSizeValueY();
    auto lhsX = GetSizeValueX();
    auto lhsY = GetSizeValueY();
    if ((rhs.GetSizeTypeX() != GetSizeTypeX()) || (rhs.GetSizeTypeY() != GetSizeTypeY())) {
        // error: unit not same, just return lhs value
        return *this;
    }
    BackgroundImagePosition position;
    position.SetSizeValueX(lhsX + rhsX);
    position.SetSizeTypeX(GetSizeTypeX());
    position.SetSizeValueY(lhsY + rhsY);
    position.SetSizeTypeY(GetSizeTypeY());
    return position;
}

BackgroundImagePosition BackgroundImagePosition::operator-(const BackgroundImagePosition& rhs) const
{
    auto rhsX = rhs.GetSizeValueX();
    auto rhsY = rhs.GetSizeValueY();
    auto lhsX = GetSizeValueX();
    auto lhsY = GetSizeValueY();
    if ((rhs.GetSizeTypeX() != GetSizeTypeX()) || (rhs.GetSizeTypeY() != GetSizeTypeY())) {
        // error: unit not same, just return lhs value
        return *this;
    }
    BackgroundImagePosition position;
    position.SetSizeValueX(lhsX - rhsX);
    position.SetSizeTypeX(GetSizeTypeX());
    position.SetSizeValueY(lhsY - rhsY);
    position.SetSizeTypeY(GetSizeTypeY());
    return position;
}

BackgroundImagePosition BackgroundImagePosition::operator*(double value) const
{
    BackgroundImagePosition position;
    position.SetSizeValueX(GetSizeValueX() * value);
    position.SetSizeTypeX(GetSizeTypeX());
    position.SetSizeValueY(GetSizeValueY() * value);
    position.SetSizeTypeY(GetSizeTypeY());
    return position;
}

bool BackgroundImagePosition::operator==(const BackgroundImagePosition& backgroundImagePosition) const
{
    bool isXAxisEqual = (GetSizeX() == backgroundImagePosition.GetSizeX()) &&
                        GetSizeTypeX() == backgroundImagePosition.GetSizeTypeX();
    bool isYAxisEqual = (GetSizeY() == backgroundImagePosition.GetSizeY()) &&
                        GetSizeTypeY() == backgroundImagePosition.GetSizeTypeY();
    return isXAxisEqual && isYAxisEqual;
}

bool BackgroundImagePosition::operator!=(const BackgroundImagePosition& backgroundImagePosition) const
{
    return !operator==(backgroundImagePosition);
}

CanvasPath2D::CanvasPath2D(const std::string& cmds)
{
    PathArgs args;
    args.cmds = cmds;
    caches_.emplace_back(PathCmd::CMDS, args);
}

CanvasPath2D::CanvasPath2D(const RefPtr<CanvasPath2D>& path)
{
    if (path != nullptr) {
        auto caches = path->GetCaches();
        caches_.swap(caches);
    }
}

void CanvasPath2D::AddPath(const RefPtr<CanvasPath2D>& path)
{
    if (path != nullptr) {
        auto caches = path->GetCaches();
        caches_.insert(caches_.end(), caches.begin(), caches.end());
    }
}

void CanvasPath2D::SetTransform(double a, double b, double c, double d, double e, double f)
{
    PathArgs args;
    args.para1 = a;
    args.para2 = b;
    args.para3 = c;
    args.para4 = d;
    args.para5 = e;
    args.para6 = f;
    caches_.emplace_back(PathCmd::TRANSFORM, args);
}

void CanvasPath2D::MoveTo(double x, double y)
{
    PathArgs args;
    args.para1 = x;
    args.para2 = y;
    caches_.emplace_back(PathCmd::MOVE_TO, args);
}

void CanvasPath2D::LineTo(double x, double y)
{
    PathArgs args;
    args.para1 = x;
    args.para2 = y;
    caches_.emplace_back(PathCmd::LINE_TO, args);
}

void CanvasPath2D::Arc(double x, double y, double radius, double startAngle, double endAngle, double ccw)
{
    PathArgs args;
    args.para1 = x;
    args.para2 = y;
    args.para3 = radius;
    args.para4 = startAngle;
    args.para5 = endAngle;
    args.para6 = ccw;
    caches_.emplace_back(PathCmd::ARC, args);
}

void CanvasPath2D::ArcTo(double x1, double y1, double x2, double y2, double radius)
{
    PathArgs args;
    args.para1 = x1;
    args.para2 = y1;
    args.para3 = x2;
    args.para4 = y2;
    args.para5 = radius;
    caches_.emplace_back(PathCmd::ARC_TO, args);
}

void CanvasPath2D::QuadraticCurveTo(double cpx, double cpy, double x, double y)
{
    PathArgs args;
    args.para1 = cpx;
    args.para2 = cpy;
    args.para3 = x;
    args.para4 = y;
    caches_.emplace_back(PathCmd::QUADRATIC_CURVE_TO, args);
}

void CanvasPath2D::BezierCurveTo(double cp1x, double cp1y, double cp2x, double cp2y, double x, double y)
{
    PathArgs args;
    args.para1 = cp1x;
    args.para2 = cp1y;
    args.para3 = cp2x;
    args.para4 = cp2y;
    args.para5 = x;
    args.para6 = y;
    caches_.emplace_back(PathCmd::BEZIER_CURVE_TO, args);
}

void CanvasPath2D::Ellipse(
    double x, double y, double radiusX, double radiusY, double rotation, double startAngle, double endAngle, double ccw)
{
    PathArgs args;
    args.para1 = x;
    args.para2 = y;
    args.para3 = radiusX;
    args.para4 = radiusY;
    args.para5 = rotation;
    args.para6 = startAngle;
    args.para7 = endAngle;
    args.para8 = ccw;
    caches_.emplace_back(PathCmd::ELLIPSE, args);
}

void CanvasPath2D::Rect(double x, double y, double width, double height)
{
    PathArgs args;
    args.para1 = x;
    args.para2 = y;
    args.para3 = width;
    args.para4 = height;
    caches_.emplace_back(PathCmd::RECT, args);
}

void CanvasPath2D::ClosePath()
{
    PathArgs args;
    caches_.emplace_back(PathCmd::CLOSE_PATH, args);
}

const std::vector<std::pair<PathCmd, PathArgs>>& CanvasPath2D::GetCaches() const
{
    return caches_;
}

std::string CanvasPath2D::ToString() const
{
    std::string str;
    for (const auto& cache : caches_) {
        switch (cache.first) {
            case PathCmd::CMDS: {
                str.append("CMDS:");
                str.append(cache.second.cmds).append(" ");
                break;
            }
            case PathCmd::TRANSFORM: {
                str.append("TRANSFORM:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(",");
                str.append(std::to_string(cache.second.para5)).append(",");
                str.append(std::to_string(cache.second.para6)).append(" ");
                break;
            }
            case PathCmd::MOVE_TO: {
                str.append("MOVE_TO:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(" ");
                break;
            }
            case PathCmd::LINE_TO: {
                str.append("LINE_TO:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(" ");
                break;
            }
            case PathCmd::ARC: {
                str.append("ARC:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(",");
                str.append(std::to_string(cache.second.para5)).append(",");
                str.append(std::to_string(cache.second.para6)).append(" ");
                break;
            }
            case PathCmd::ARC_TO: {
                str.append("ARC_TO:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(",");
                str.append(std::to_string(cache.second.para5)).append(" ");
                break;
            }
            case PathCmd::QUADRATIC_CURVE_TO: {
                str.append("QUADRATIC_CURVE_TO:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(" ");
                break;
            }
            case PathCmd::BEZIER_CURVE_TO: {
                str.append("BEZIER_CURVE_TO:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(",");
                str.append(std::to_string(cache.second.para5)).append(",");
                str.append(std::to_string(cache.second.para6)).append(" ");
                break;
            }
            case PathCmd::ELLIPSE: {
                str.append("ELLIPSE:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(",");
                str.append(std::to_string(cache.second.para5)).append(",");
                str.append(std::to_string(cache.second.para6)).append(",");
                str.append(std::to_string(cache.second.para7)).append(",");
                str.append(std::to_string(cache.second.para8)).append(" ");
                break;
            }
            case PathCmd::RECT: {
                str.append("RECT:");
                str.append(std::to_string(cache.second.para1)).append(",");
                str.append(std::to_string(cache.second.para2)).append(",");
                str.append(std::to_string(cache.second.para3)).append(",");
                str.append(std::to_string(cache.second.para4)).append(" ");
                break;
            }
            case PathCmd::CLOSE_PATH: {
                str.append("CLOSE_PATH").append(" ");
                break;
            }
            default: {
                break;
            }
        }
    }
    return str;
}

} // namespace OHOS::Ace
