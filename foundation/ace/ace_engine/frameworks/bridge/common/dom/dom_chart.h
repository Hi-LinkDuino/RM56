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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CHART_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CHART_H

#include "core/components/chart/chart_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/data_panel/data_panel_component.h"
#include "core/components/progress/progress_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace::Framework {

class ChartOptions {
public:
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

    const AxisOption& GetXAxis() const
    {
        return xAxis_;
    }

    void SetXAxis(const AxisOption& xAxis)
    {
        xAxis_ = xAxis;
    }

    const AxisOption& GetYAxis() const
    {
        return yAxis_;
    }

    void SetYAxis(const AxisOption& yAxis)
    {
        yAxis_ = yAxis;
    }

    int32_t GetErasePointNumber() const
    {
        return erasePointNumber_;
    }

    void SetErasePointNumber(int32_t erasePointNumber)
    {
        erasePointNumber_ = erasePointNumber;
    }

    bool GetLineGradient() const
    {
        return lineGradient_;
    }

    void SetLineGradient(bool lineGradient)
    {
        lineGradient_ = lineGradient;
    }

    bool GetLoop() const
    {
        return loop_;
    }

    void SetLoop(bool loop)
    {
        loop_ = loop;
    }

    bool GetWholeLineGradient() const
    {
        return wholeLineGradient_;
    }

    void SetWholeLineGradient(bool wholeLineGradient)
    {
        wholeLineGradient_ = wholeLineGradient;
    }

    const Color& GetTargetColor() const
    {
        return targetColor_;
    }

    void SetTargetColor(const Color& targetColor)
    {
        targetColor_ = targetColor;
    }

private:
    AxisOption xAxis_;
    AxisOption yAxis_;
    double lineWidth_ = 1.0;
    bool smoothFlag_ = false;
    PointInfo headPoint_;
    PointInfo topPoint_;
    PointInfo bottomPoint_;
    int32_t erasePointNumber_ = 0;
    bool lineGradient_ = false;
    bool wholeLineGradient_ = false;
    bool loop_ = false;
    Color targetColor_ = Color::TRANSPARENT;
};

class DOMChart : public DOMNode {
    DECLARE_ACE_TYPE(DOMChart, DOMNode);

public:
    DOMChart(NodeId nodeId, const std::string& nodeName);
    ~DOMChart() override = default;

    RefPtr<Component> GetSpecializedComponent() override;

    void SetChartAttrOptions(const ChartOptions& chartOptions);
    void SetChartAttrDatasets(const std::vector<MainChart>& datasets);
    void SetChartAttrSegments(const std::vector<Segment>& segments);
    void CallSpecializedMethod(const std::string& method, const std::string& args) override;

protected:
    bool SetSpecializedAttr(const std::pair<std::string, std::string>& attr) override;
    bool SetSpecializedStyle(const std::pair<std::string, std::string>& style) override;
    void OnSetStyleFinished() override;
    void PrepareSpecializedComponent() override;

private:
    void UpdateTopButtomPoint(std::vector<MainChart>& data);
    void UpdateChartData(int32_t coorY, std::vector<MainChart>& data);
    void SetPoint(PointInfo& pointInfo, PointInfo getPointInfo);
    void SetChart(MainChart& chartDataset);

    RefPtr<ProgressComponent> progressChild_;
    RefPtr<ChartComponent> chartChild_;
    RefPtr<DataPanelComponent> dataPanelChild_;

    ChartType chartType_ { ChartType::LINE };
    ChartOptions chartOptions_;
    std::vector<MainChart> chartDatasets_;
    std::vector<Segment> segments_;

    double min_ = 0.0;
    double max_ = 100.0;
    double percent_ = 0.0;
    std::pair<Dimension, bool> strokeWidth_ = { Dimension(32.0, DimensionUnit::PX), false };
    std::pair<double, bool> startAngle_ = { 240.0, false };
    std::pair<double, bool> totalAngle_ = { 240.0, false };

    std::pair<double, bool> radius_ = { -1.0, false };
    std::pair<double, bool> centerX_ = { -1.0, false };
    std::pair<double, bool> centerY_ = { -1.0, false };

    std::vector<Color> colors_;
    std::vector<double> weights_;
    std::vector<std::pair<int32_t, bool>> lineData_;

    bool isResetPosition_ = false;
    int32_t position_ = 0;
    int32_t seriesNum_ = 0;
    bool isSetFirst_ = false;

    Color textColor_;
    Color backgroundColor_ = Color::FromString("#08000000");
    bool trackColorSet_ = false;
    double textSize_ = 18.0;
    std::vector<std::string> fontFamily_;
    bool showEffect_ = false;
    bool autoScale_ = false;
    double animationDuration_ = -1.0;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_DOM_DOM_CHART_H
