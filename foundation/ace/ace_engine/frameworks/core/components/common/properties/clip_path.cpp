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

#include "core/components/common/properties/clip_path.h"

#include "base/utils/string_utils.h"

namespace OHOS::Ace {

bool Inset::SetLength(const std::vector<Dimension>& lengths)
{
    std::string::size_type size = lengths.size();
    bool ret = true;
    switch (size) {
        case 1:
            SetLength(lengths.at(0), lengths.at(0), lengths.at(0), lengths.at(0));
            break;
        case 2:
            SetLength(lengths.at(0), lengths.at(1), lengths.at(0), lengths.at(1));
            break;
        case 3:
            SetLength(lengths.at(0), lengths.at(1), lengths.at(2), lengths.at(1));
            break;
        case 4:
            SetLength(lengths.at(0), lengths.at(1), lengths.at(2), lengths.at(3));
            break;
        default:
            LOGE("invalid length value");
            ret = false;
            break;
    }
    return ret;
}

void Inset::SetLength(const Dimension& top, const Dimension& right, const Dimension& bottom, const Dimension& left)
{
    SetTop(top);
    SetRight(right);
    SetBottom(bottom);
    SetLeft(left);
}

void Inset::SetRadius(const std::vector<Dimension>& rounds, bool isX)
{
    std::string::size_type size = rounds.size();
    switch (size) {
        case 1:
            SetRadius(rounds.at(0), rounds.at(0), rounds.at(0), rounds.at(0), isX);
            break;
        case 2:
            SetRadius(rounds.at(0), rounds.at(1), rounds.at(0), rounds.at(1), isX);
            break;
        case 3:
            SetRadius(rounds.at(0), rounds.at(1), rounds.at(2), rounds.at(1), isX);
            break;
        case 4:
            SetRadius(rounds.at(0), rounds.at(1), rounds.at(2), rounds.at(3), isX);
            break;
        default:
            LOGE("invalid radius value");
            break;
    }
}

void Inset::SetRadius(const Dimension& top, const Dimension& right, const Dimension& bottom, const Dimension& left,
    bool isX)
{
    SetTopLeftRadius(top, isX);
    SetTopRightRadius(right, isX);
    SetBottomRightRadius(bottom, isX);
    SetBottomLeftRadius(left, isX);
}

GeometryBoxType ClipPath::GetGeometryBoxType(const std::string& value)
{
    GeometryBoxType geometryBoxType = GeometryBoxType::NONE;
    std::string::size_type boxPosition = 0;
    std::string::size_type tmp = value.find("margin-box");
    if (tmp != std::string::npos && tmp >= boxPosition) {
        geometryBoxType = GeometryBoxType::MARGIN_BOX;
        boxPosition = tmp;
    }
    tmp = value.find("border-box");
    if (tmp != std::string::npos && tmp >= boxPosition) {
        geometryBoxType = GeometryBoxType::BORDER_BOX;
        boxPosition = tmp;
    }
    tmp = value.find("padding-box");
    if (tmp != std::string::npos && tmp >= boxPosition) {
        geometryBoxType = GeometryBoxType::PADDING_BOX;
        boxPosition = tmp;
    }
    tmp = value.find("content-box");
    if (tmp != std::string::npos && tmp >= boxPosition) {
        geometryBoxType = GeometryBoxType::CONTENT_BOX;
    }
    return geometryBoxType;
}

void ClipPath::GetBasicShapeInfo(const std::string& value, BasicShapeType& basicShapeType, std::string& data)
{
    std::string::size_type first = 0;
    std::string::size_type tmp = value.find("inset(");
    if (tmp != std::string::npos && tmp >= first) {
        first = tmp + std::strlen("inset(");
        data = value.substr(first, (value.find_first_of(')', first) - first));
        basicShapeType = BasicShapeType::INSET;
    }
    tmp = value.find("circle(");
    if (tmp != std::string::npos && tmp >= first) {
        first = tmp + std::strlen("circle(");
        data = value.substr(first, (value.find_first_of(')', first) - first));
        basicShapeType = BasicShapeType::CIRCLE;
    }
    tmp = value.find("ellipse(");
    if (tmp != std::string::npos && tmp >= first) {
        first = tmp + std::strlen("ellipse(");
        data = value.substr(first, (value.find_first_of(')', first) - first));
        basicShapeType = BasicShapeType::ELLIPSE;
    }
    tmp = value.find("polygon(");
    if (tmp != std::string::npos && tmp >= first) {
        first = tmp + std::strlen("polygon(");
        data = value.substr(first, (value.find_first_of(')', first) - first));
        basicShapeType = BasicShapeType::POLYGON;
    }
    tmp = value.find("path('");
    if (tmp != std::string::npos && tmp >= first) {
        first = tmp + std::strlen("path('");
        data = value.substr(first, (value.find("')", first) - first));
        basicShapeType = BasicShapeType::PATH;
    }
    tmp = value.find("path(\"");
    if (tmp != std::string::npos && tmp >= first) {
        first = tmp + std::strlen("path(\"");
        data = value.substr(first, (value.find("\")", first) - first));
        basicShapeType = BasicShapeType::PATH;
    }
}

RefPtr<Circle> ClipPath::CreateCircle(const std::string& data)
{
    std::string::size_type atIndex = data.find("at");
    if (atIndex == std::string::npos) {
        Dimension radius = StringUtils::StringToDimension(StringUtils::TrimStr(data));
        if (!radius.IsValid()) {
            return nullptr;
        }
        auto circle = AceType::MakeRefPtr<Circle>();
        circle->SetRadius(radius);
        return circle;
    }
    Dimension radius = StringUtils::StringToDimension(StringUtils::TrimStr(data.substr(0, atIndex)));
    if (!radius.IsValid()) {
        return nullptr;
    }
    auto circle = AceType::MakeRefPtr<Circle>();
    circle->SetRadius(radius);
    std::vector<Dimension> axis;
    StringUtils::SplitStr(StringUtils::TrimStr(data.substr(atIndex + 2)), " ", axis);
    if (axis.size() == 1) {
        circle->SetAxisX(axis.at(0));
    }
    if (axis.size() >= 2) {
        circle->SetAxisX(axis.at(0));
        circle->SetAxisY(axis.at(1));
    }
    return circle;
}

RefPtr<Ellipse> ClipPath::CreateEllipse(const std::string& data)
{
    std::string::size_type atIndex = data.find("at");
    if (atIndex == std::string::npos) {
        return CreateEllipseSize(data);
    }
    auto ellipse = CreateEllipseSize(StringUtils::TrimStr(data.substr(0, atIndex)));
    if (!ellipse) {
        return nullptr;
    }
    std::vector<Dimension> axis;
    StringUtils::SplitStr(StringUtils::TrimStr(data.substr(atIndex + 2)), " ", axis);
    if (axis.size() == 1) {
        ellipse->SetAxisX(axis.at(0));
    }
    if (axis.size() >= 2) {
        ellipse->SetAxisX(axis.at(0));
        ellipse->SetAxisY(axis.at(1));
    }
    return ellipse;
}

RefPtr<Ellipse> ClipPath::CreateEllipseSize(const std::string& data)
{
    std::vector<Dimension> lengths;
    StringUtils::SplitStr(data, " ", lengths);
    if (lengths.size() != 2) {
        return nullptr;
    }
    if (!lengths.at(0).IsValid() || !lengths.at(1).IsValid()) {
        return nullptr;
    }
    auto ellipse = AceType::MakeRefPtr<Ellipse>();
    ellipse->SetRadiusX(lengths.at(0));
    ellipse->SetRadiusY(lengths.at(1));
    return ellipse;
}

RefPtr<Inset> ClipPath::CreateInset(const std::string& data)
{
    std::string::size_type roundIndex = data.find("round");
    if (roundIndex == std::string::npos) {
        return CreateInsetSize(data);
    }
    auto inset = CreateInsetSize(StringUtils::TrimStr(data.substr(0, roundIndex)));
    if (!inset) {
        return nullptr;
    }
    std::string roundData = StringUtils::TrimStr(data.substr(roundIndex + 5));
    std::string::size_type roundDataIndex = roundData.find('/');
    if (roundDataIndex == std::string::npos) {
        std::vector<Dimension> rounds;
        StringUtils::SplitStr(roundData, " ", rounds);
        if (rounds.empty()) {
            return inset;
        }
        inset->SetRadius(rounds);
    } else {
        std::vector<Dimension> rounds1;
        StringUtils::SplitStr(StringUtils::TrimStr(roundData.substr(0, roundDataIndex)), " ", rounds1);
        if (!rounds1.empty()) {
            inset->SetRadius(rounds1, true);
        }
        std::vector<Dimension> rounds2;
        StringUtils::SplitStr(StringUtils::TrimStr(roundData.substr(roundDataIndex + 1)), " ", rounds2);
        if (!rounds2.empty()) {
            inset->SetRadius(rounds2, false);
        }
    }
    return inset;
}

RefPtr<Inset> ClipPath::CreateInsetSize(const std::string& data)
{
    std::vector<Dimension> lengths;
    StringUtils::SplitStr(data, " ", lengths);
    if (lengths.empty()) {
        return nullptr;
    }
    auto inset = AceType::MakeRefPtr<Inset>();
    if (inset->SetLength(lengths)) {
        return inset;
    }
    return nullptr;
}

RefPtr<Polygon> ClipPath::CreatePolygon(const std::string& data)
{
    std::vector<std::string> points;
    StringUtils::StringSpliter(StringUtils::TrimStr(data), ',', points);
    if (points.empty()) {
        return nullptr;
    }
    auto polygon = AceType::MakeRefPtr<Polygon>();
    for (const auto& item : points) {
        std::vector<Dimension> point;
        StringUtils::SplitStr(StringUtils::TrimStr(item), " ", point);
        if (point.size() != 2) {
            return nullptr;
        }
        polygon->PushPoint(point[0], point[1]);
    }
    if (polygon->IsValid()) {
        return polygon;
    }
    return nullptr;
}

RefPtr<Path> ClipPath::CreatePath(const std::string& data)
{
    if (data.empty()) {
        return nullptr;
    }
    auto path = AceType::MakeRefPtr<Path>();
    path->SetValue(StringUtils::TrimStr(data));
    return path;
}

RefPtr<ClipPath> ClipPath::CreateShape(const std::string& value)
{
    BasicShapeType basicShapeType = BasicShapeType::NONE;
    std::string data;
    GetBasicShapeInfo(value, basicShapeType, data);
    if (basicShapeType == BasicShapeType::NONE || data.empty()) {
        return nullptr;
    }
    RefPtr<BasicShape> basicShape = nullptr;
    switch (basicShapeType) {
        case BasicShapeType::CIRCLE: {
            basicShape = CreateCircle(data);
            break;
        }
        case BasicShapeType::ELLIPSE: {
            basicShape = CreateEllipse(data);
            break;
        }
        case BasicShapeType::INSET: {
            basicShape = CreateInset(data);
            break;
        }
        case BasicShapeType::PATH: {
            basicShape = CreatePath(data);
            break;
        }
        case BasicShapeType::POLYGON: {
            basicShape = CreatePolygon(data);
            break;
        }
        default: {
            LOGE("basic shape type is none or invalid");
            break;
        }
    }
    if (basicShape) {
        return AceType::MakeRefPtr<ClipPath>(basicShape);
    }
    return nullptr;
}

} // namespace OHOS::Ace
