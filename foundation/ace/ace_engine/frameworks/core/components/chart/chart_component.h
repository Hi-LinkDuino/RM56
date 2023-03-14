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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHART_CHART_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHART_CHART_COMPONENT_H

#include <vector>

#include "base/geometry/point.h"
#include "core/components/chart/chart_element.h"
#include "core/components/common/properties/color.h"
#include "core/components/common/properties/placement.h"
#include "core/pipeline/base/component.h"
#include "core/pipeline/base/render_node.h"

namespace OHOS::Ace {

enum class PointShape {
    CIRCLE,
    SQUARE,
    TRIANGLE,
};

enum class ChartType {
    LINE,
    BAR,
    GAUGE,
    PROGRESS,
    RAINBOW,
    LOADING,
};

struct AxisOption {
    double min = 0.0;
    double max = 100.0;
    int32_t tickNumber = 10;
    bool display = false;
    Color color = Color::GRAY;
    std::string colorString;
};

class TextInfo final {
public:
    void SetTextValue(const std::string& value)
    {
        value_ = value;
    }

    const std::string& GetTextValue() const
    {
        return value_;
    }

    void SetPlacement(Placement placement)
    {
        placement_ = placement;
    }

    Placement GetPlacement() const
    {
        return placement_;
    }

    void SetColor(const Color& color)
    {
        color_ = color;
    }

    const Color& GetColor() const
    {
        return color_;
    }

    void SetColorString(std::string color)
    {
        colorString_ = color;
    }

    std::string GetColorString() const
    {
        return colorString_;
    }

private:
    std::string value_ = "";
    Placement placement_ = Placement::BOTTOM;
    Color color_ = Color::WHITE;
    std::string colorString_;
};

enum class LineType {
    SOLID,
    DASHED,
};

class SegmentInfo final {
public:
    void SetSegmentColor(const Color& color)
    {
        color_ = color;
    }

    const Color& GetSegmentColor() const
    {
        return color_;
    }

    double GetSolidWidth() const
    {
        return solidWidth_;
    }

    void SetSolidWidth(double width)
    {
        solidWidth_ = width;
    }

    double GetSpaceWidth() const
    {
        return spaceWidth_;
    }

    void SetSpaceWidth(double width)
    {
        spaceWidth_ = width;
    }

    LineType GetLineType() const
    {
        return type_;
    }

    void SetLineType(LineType type)
    {
        type_ = type;
    }

    bool operator==(const SegmentInfo& segmentInfo) const
    {
        return color_ == segmentInfo.color_ && NearEqual(solidWidth_, segmentInfo.solidWidth_) &&
               NearEqual(spaceWidth_, segmentInfo.spaceWidth_);
    }

    bool operator!=(const SegmentInfo& segmentInfo) const
    {
        return !operator==(segmentInfo);
    }

    std::string GetColorString() const
    {
        return colorString_;
    }

    void SetColorString(const std::string color)
    {
        colorString_ = color;
    }

private:
    Color color_ = Color::TRANSPARENT;
    std::string colorString_;
    double solidWidth_ = 5.0;
    double spaceWidth_ = 5.0;
    LineType type_ = LineType::SOLID;
};

class PointInfo final {
public:
    PointInfo() = default;
    explicit PointInfo(const Point& point)
    {
        coordinate_ = point;
    }
    ~PointInfo() = default;

    PointShape GetPointShape() const
    {
        return pointShape_;
    }

    void SetPointShape(PointShape shape)
    {
        pointShape_ = shape;
    }

    const Dimension& GetPointSize() const
    {
        return pointSize_;
    }

    void SetPointSize(const Dimension& size)
    {
        pointSize_ = size;
    }

    const Color& GetStrokeColor() const
    {
        return strokeColor_;
    }

    void SetStrokeColor(const Color& color)
    {
        strokeColor_ = color;
    }

    const Color& GetFillColor() const
    {
        return fillColor_;
    }

    void SetFillColor(const Color& color)
    {
        fillColor_ = color;
    }

    bool GetDisplay() const
    {
        return display_;
    }

    void SetDisplay(bool display)
    {
        display_ = display;
    }

    double GetX() const
    {
        return coordinate_.GetX();
    }

    double GetY() const
    {
        return coordinate_.GetY();
    }

    void SetX(double x)
    {
        coordinate_.SetX(x);
    }

    void SetY(double y)
    {
        coordinate_.SetY(y);
    }

    void SetPointStrokeWidth(const Dimension& dimension)
    {
        pointStrokeWidth_ = dimension;
    }

    const Dimension& GetPointStrokeWidth() const
    {
        return pointStrokeWidth_;
    }

    void SetStrokeColorString(const std::string colorString)
    {
        strokeColorString_ = colorString;
    }

    std::string GetStrokeColorString() const
    {
        return strokeColorString_;
    }

    void SetFillColorString(const std::string colorString)
    {
        fillColorString_ = colorString;
    }

    std::string GetFillColorString() const
    {
        return fillColorString_;
    }

private:
    PointShape pointShape_ = PointShape::CIRCLE;
    Dimension pointStrokeWidth_ = Dimension(1.0, DimensionUnit::PX);
    Dimension pointSize_ = Dimension(5.0, DimensionUnit::PX);
    Color strokeColor_ = Color::FromString("#ff0000");
    std::string strokeColorString_;
    Color fillColor_ = Color::FromString("#ff0000");
    std::string fillColorString_;
    Point coordinate_;
    bool display_ = false;
};

class LineInfo final {
public:
    LineInfo() = default;

    explicit LineInfo(const TextInfo& textInfo)
    {
        textInfo_ = textInfo;
    }

    explicit LineInfo(const PointInfo& pointInfo)
    {
        pointInfo_ = pointInfo;
    }

    explicit LineInfo(const SegmentInfo& segmentInfo)
    {
        segmentInfo_ = segmentInfo;
    }

    ~LineInfo() = default;

    void SetTextInfo(const TextInfo& textInfo)
    {
        textInfo_ = textInfo;
    }

    TextInfo GetTextInfo() const
    {
        return textInfo_;
    }

    void SetPointInfo(const PointInfo& pointInfo)
    {
        pointInfo_ = pointInfo;
    }

    PointInfo GetPointInfo() const
    {
        return pointInfo_;
    }

    void SetSegmentInfo(const SegmentInfo& segmentInfo)
    {
        segmentInfo_ = segmentInfo;
    }

    SegmentInfo GetSegmentInfo() const
    {
        return segmentInfo_;
    }

private:
    TextInfo textInfo_;
    PointInfo pointInfo_;
    SegmentInfo segmentInfo_;
};

class MainChart final {
public:
    MainChart() = default;
    ~MainChart() = default;

    // add data into an exist dataset
    void AppendData(const LineInfo& lineInfo)
    {
        dataSets_.emplace_back(lineInfo);
    }

    // clear data in data set
    void ClearData()
    {
        dataSets_.clear();
    }

    // create and add a new data set
    void SetData(const std::vector<LineInfo>& lineInfo)
    {
        dataSets_ = lineInfo;
    }

    bool ReplaceData(uint32_t index, const LineInfo& lineInfo)
    {
        if (index >= dataSets_.size()) {
            return false;
        }
        dataSets_[index] = lineInfo;
        return true;
    }

    std::vector<LineInfo> GetData() const
    {
        return dataSets_;
    }

    const Color& GetFillColor() const
    {
        return fillColor_;
    }

    void SetFillColor(const Color& color)
    {
        fillColor_ = color;
    }

    const Color& GetStrokeColor() const
    {
        return strokeColor_;
    }

    void SetStrokeColor(const Color& color)
    {
        strokeColor_ = color;
    }

    bool GetGradient() const
    {
        return gradient_;
    }

    void SetGradient(bool gradient)
    {
        gradient_ = gradient;
    }

    void SetHeadPoint(const PointInfo& point)
    {
        headPoint_ = point;
    }

    const PointInfo& GetHeadPoint() const
    {
        return headPoint_;
    }

    void SetTopPoint(const PointInfo& point)
    {
        topPoint_ = point;
    }

    const PointInfo& GetTopPoint() const
    {
        return topPoint_;
    }

    void SetBottomPoint(const PointInfo& point)
    {
        bottomPoint_ = point;
    }

    const PointInfo& GetBottomPoint() const
    {
        return bottomPoint_;
    }

    bool GetSmoothFlag() const
    {
        return smoothFlag_;
    }

    void SetSmoothFlag(bool flag)
    {
        smoothFlag_ = flag;
    }

    double GetLineWidth() const
    {
        return lineWidth_;
    }

    void SetLineWidth(double lineWidth)
    {
        lineWidth_ = lineWidth;
    }

    void SetErasePointNumber(int32_t number)
    {
        erasePointNumber_ = number;
    }

    int32_t GetErasePointNumber() const
    {
        return erasePointNumber_;
    }

    void SetLineGradient(bool flag)
    {
        lineGradient_ = flag;
    }

    bool GetLineGradient() const
    {
        return lineGradient_;
    }

    void SetWholeLineGradient(bool flag)
    {
        wholeLineGradient_ = flag;
    }

    bool GetWholeLineGradient() const
    {
        return wholeLineGradient_;
    }

    const Color& GetTargetColor() const
    {
        return targetColor_;
    }

    void SetTargetColor(const Color& targetColor)
    {
        targetColor_ = targetColor;
    }

    void SetHeadPointIndex(int32_t index)
    {
        headPointIndex_ = index;
    }

    int32_t GetHeadPointIndex() const
    {
        return headPointIndex_;
    }

    const Color& GetTextColor() const
    {
        return textColor_;
    }

    void SetTextColor(const Color& textColor)
    {
        textColor_ = textColor;
    }

    double GetTextSize() const
    {
        return textSize_;
    }

    void SetTextSize(double textSize)
    {
        textSize_ = textSize;
    }

    std::vector<std::string> GetFontFamily() const
    {
        return fontFamily_;
    }

    void SetFontFamily(const std::vector<std::string>& fonts)
    {
        fontFamily_ = fonts;
    }

private:
    // data set name, vector of data
    std::vector<LineInfo> dataSets_;
    double lineWidth_ = 1.0;
    bool smoothFlag_ = false;
    bool gradient_ = false;
    PointInfo headPoint_;
    PointInfo topPoint_;
    PointInfo bottomPoint_;
    Color fillColor_ = Color::FromString("#ff6384");
    Color strokeColor_ = Color::FromString("#ff6384");
    int32_t erasePointNumber_ = 0;
    int32_t headPointIndex_ = -1;
    bool lineGradient_ = false;
    bool wholeLineGradient_ = false;

    Color textColor_;
    double textSize_ = 18.0;
    std::vector<std::string> fontFamily_;
    Color targetColor_ = Color::TRANSPARENT;
};

class ChartComponent : public RenderComponent {
    DECLARE_ACE_TYPE(ChartComponent, RenderComponent);

public:
    ChartComponent() = default;
    explicit ChartComponent(ChartType type)
    {
        type_ = type;
    }
    ~ChartComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    const AxisOption& GetHorizontalOption() const
    {
        return horizontal_;
    }

    void SetHorizontalOption(const AxisOption& xOption)
    {
        horizontal_ = xOption;
    }

    const AxisOption& GetVerticalOption()
    {
        return vertical_;
    }

    void SetVerticalOption(const AxisOption& yOption)
    {
        vertical_ = yOption;
    }

    std::vector<MainChart> GetMainCharts() const
    {
        return mainCharts_;
    }

    void SetMainCharts(const std::vector<MainChart>& mainCharts)
    {
        mainCharts_ = mainCharts;
    }

    ChartType GetChartType() const
    {
        return type_;
    }

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<ChartElement>();
    }

private:
    ChartType type_ = ChartType::LINE;
    AxisOption horizontal_;
    AxisOption vertical_;
    std::vector<MainChart> mainCharts_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_CHART_CHART_COMPONENT_H
