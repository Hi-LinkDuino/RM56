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

#include "frameworks/bridge/common/dom/dom_chart.h"

#include "frameworks/bridge/common/utils/utils.h"

#include "base/utils/linear_map.h"
#include "base/utils/string_utils.h"
#include "base/utils/utils.h"
#include "core/components/common/properties/color_factory.h"

namespace OHOS::Ace::Framework {
namespace {

const char LIGHT_RED_COLOR[] = "#EB4034";
const char LIGHT_GREEN_COLOR[] = "#AEEB34";
const char LIGHT_BLUE_COLOR[] = "#34EBD9";
constexpr double PROGRESS_DEFAULT_MAX_VALUE = 100.0;
constexpr uint32_t METHOD_APPEND_ARGS_SIZE = 1;

} // namespace

RefPtr<Component> DOMChart::GetSpecializedComponent()
{
    if (chartType_ == ChartType::BAR || chartType_ == ChartType::LINE) {
        return chartChild_;
    }
    if (chartType_ == ChartType::GAUGE) {
        return progressChild_;
    }
    return dataPanelChild_;
}

DOMChart::DOMChart(NodeId nodeId, const std::string& nodeName) : DOMNode(nodeId, nodeName) {}

void DOMChart::SetChartAttrOptions(const ChartOptions& chartOptions)
{
    chartOptions_ = chartOptions;
}

void DOMChart::SetChartAttrDatasets(const std::vector<MainChart>& datasets)
{
    chartDatasets_ = datasets;
    isResetPosition_ = false;
    position_ = 0;
    seriesNum_ = 0;
    lineData_.clear();
    isSetFirst_ = false;
}

void DOMChart::SetChartAttrSegments(const std::vector<Segment>& segments)
{
    segments_ = segments;
}

bool DOMChart::SetSpecializedAttr(const std::pair<std::string, std::string>& attr)
{
    // this map should be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, DOMChart&)> chartAttrsOperators[] = {
        { DOM_CHART_ANIMATION_DURATION,
            [](const std::string& val, DOMChart& chart) {
              chart.animationDuration_ = StringToDouble(val);
            } },
        { DOM_AUTO_SCALE,
            [](const std::string& val, DOMChart& chart) {
              chart.autoScale_ = StringToBool(val);
            } },
        { DOM_EFFECTS_ON,
            [](const std::string& val, DOMChart& chart) {
              chart.showEffect_ = StringToBool(val);
            } },
        { DOM_PROGRESS_PERCENT,
            [](const std::string& val, DOMChart& chart) {
                chart.percent_ = StringToDouble(val);
                if (chart.percent_ > chart.max_) {
                    chart.percent_ = chart.max_;
                }
                if (chart.percent_ < chart.min_) {
                    chart.percent_ = chart.min_;
                }
            } },
        { DOM_CHART_TYPE,
            [](const std::string& val, DOMChart& chart) {
                if (val == DOM_CHART_TYPE_GAUGE) {
                    chart.chartType_ = ChartType::GAUGE;
                } else if (val == DOM_CHART_TYPE_BAR) {
                    chart.chartType_ = ChartType::BAR;
                } else if (val == DOM_CHART_TYPE_LINE) {
                    chart.chartType_ = ChartType::LINE;
                } else if (val == DOM_CHART_TYPE_PROGRESS) {
                    chart.chartType_ = ChartType::PROGRESS;
                } else if (val == DOM_CHART_TYPE_RAINBOW) {
                    chart.chartType_ = ChartType::RAINBOW;
                } else if (val == DOM_CHART_TYPE_LOADING) {
                    chart.chartType_ = ChartType::LOADING;
                } else {
                    LOGI("chart type error %{public}s, now using loading", val.c_str());
                    chart.chartType_ = ChartType::LINE;
                }
            } },
    };
    auto operatorIter = BinarySearchFindIndex(chartAttrsOperators, ArraySize(chartAttrsOperators), attr.first.c_str());
    if (operatorIter != -1) {
        chartAttrsOperators[operatorIter].value(attr.second, *this);
        return true;
    }
    return false;
}

bool DOMChart::SetSpecializedStyle(const std::pair<std::string, std::string>& style)
{
    // this map should be sorted by key.
    static const LinearMapNode<void (*)(const std::string&, DOMChart&)> chartStylesOperators[] = {
        { DOM_BACKGROUND_COLOR,
            [](const std::string& val, DOMChart& chart) {
                if (chart.chartType_ == ChartType::LINE || (chart.chartType_ == ChartType::BAR)) {
                    auto boxComponent = chart.GetBoxComponent();
                    if (boxComponent) {
                        boxComponent->SetColor(chart.ParseColor(val));
                        return;
                    }
                }
                chart.backgroundColor_ = chart.ParseColor(val);
                chart.trackColorSet_ = true;
            } },
        { DOM_CENTER_X,
            [](const std::string& val, DOMChart& chart) {
                chart.centerX_.first = StringToDouble(val);
                chart.centerX_.second = true;
            } },
        { DOM_CENTER_Y,
            [](const std::string& val, DOMChart& chart) {
                chart.centerY_.first = StringToDouble(val);
                chart.centerY_.second = true;
            } },
        { DOM_COLORS_ARRAY,
            [](const std::string& val, DOMChart& chart) {
                chart.colors_.clear();
                StringUtils::StringSpliter(val, ',', chart.colors_);
            } },
        { DOM_TEXT_FONT_FAMILY,
            [](const std::string& val, DOMChart& chart) {
              chart.fontFamily_ = ConvertStrToFontFamilies(val);
            } },
        { DOM_TEXT_FONT_SIZE,
            [](const std::string& val, DOMChart& chart) {
                chart.textSize_ = StringToDouble(val);
            } },
        { DOM_PROGRESS_RADIUS,
            [](const std::string& val, DOMChart& chart) {
                chart.radius_.first = StringToDouble(val);
                chart.radius_.second = true;
            } },
        { DOM_START_DEGREE,
            [](const std::string& val, DOMChart& chart) {
                chart.startAngle_.first = StringToDouble(val);
                chart.startAngle_.second = true;
            } },
        { DOM_PROGRESS_STROKE_WIDTH,
            [](const std::string& val, DOMChart& chart) {
                chart.strokeWidth_.first = chart.ParseDimension(val);
                chart.strokeWidth_.second = true;
            } },
        { DOM_SWEEP_DEGREE,
            [](const std::string& val, DOMChart& chart) {
                chart.totalAngle_.first = StringToDouble(val);
                chart.totalAngle_.second = true;
            } },
        { DOM_WEIGHTS_ARRAY,
            [](const std::string& val, DOMChart& chart) {
                chart.weights_.clear();
                StringUtils::StringSpliter(val, ',', chart.weights_);
            } },
    };
    auto operatorIter =
        BinarySearchFindIndex(chartStylesOperators, ArraySize(chartStylesOperators), style.first.c_str());
    if (operatorIter != -1) {
        chartStylesOperators[operatorIter].value(style.second, *this);
        return true;
    }
    return false;
}

void DOMChart::OnSetStyleFinished()
{
    // colors or weight are illegal, and set default color and weight
    if (colors_.empty() || weights_.empty() || colors_.size() != weights_.size()) {
        colors_.push_back(ParseColor(LIGHT_RED_COLOR));
        colors_.push_back(ParseColor(LIGHT_GREEN_COLOR));
        colors_.push_back(ParseColor(LIGHT_BLUE_COLOR));
        // equally seprate the range
        weights_.push_back(1);
        weights_.push_back(1);
        weights_.push_back(1);
    }
    if (!centerY_.second || !centerX_.second || !radius_.second) {
        centerY_.first = -1.0;
        centerX_.first = -1.0;
        radius_.first = -1.0;
        centerY_.second = false;
        centerX_.second = false;
        radius_.second = false;
    }
}

// Sets other properties of a point, except coordinates.
void DOMChart::SetPoint(PointInfo& pointInfo, PointInfo getPointInfo)
{
    if (!getPointInfo.GetFillColorString().empty()) {
        getPointInfo.SetFillColor(ParseColor(getPointInfo.GetFillColorString()));
    }
    pointInfo.SetFillColor(getPointInfo.GetFillColor());
    if (!getPointInfo.GetStrokeColorString().empty()) {
        getPointInfo.SetStrokeColor(ParseColor(getPointInfo.GetStrokeColorString()));
    }
    pointInfo.SetStrokeColor(getPointInfo.GetStrokeColor());
    pointInfo.SetPointStrokeWidth(getPointInfo.GetPointStrokeWidth());
    pointInfo.SetPointSize(getPointInfo.GetPointSize());
    pointInfo.SetPointShape(getPointInfo.GetPointShape());
    pointInfo.SetDisplay(getPointInfo.GetDisplay());
}

void DOMChart::SetChart(MainChart& chartDataset)
{
    chartDataset.SetLineWidth(chartOptions_.GetLineWidth());
    chartDataset.SetSmoothFlag(chartOptions_.GetSmoothFlag());
    chartDataset.SetLineGradient(chartOptions_.GetLineGradient());
    chartDataset.SetWholeLineGradient(chartOptions_.GetWholeLineGradient());
    chartDataset.SetTargetColor(chartOptions_.GetTargetColor());
    chartDataset.SetErasePointNumber(chartOptions_.GetErasePointNumber());
    chartDataset.SetTextSize(textSize_);
    chartDataset.SetFontFamily(fontFamily_);

    auto points = chartDataset.GetData();
    if (points.empty()) {
        LOGD("points is empty, chart has no data to set.");
        return;
    }

    // parse color from color string
    for (auto& point : points) {
        auto pointInfo = point.GetPointInfo();
        if (!pointInfo.GetStrokeColorString().empty()) {
            pointInfo.SetStrokeColor(ParseColor(pointInfo.GetStrokeColorString()));
        }
        if (!pointInfo.GetFillColorString().empty()) {
            pointInfo.SetFillColor(ParseColor(pointInfo.GetFillColorString()));
        }
        auto segment = point.GetSegmentInfo();
        if (!segment.GetColorString().empty()) {
            segment.SetSegmentColor(ParseColor(segment.GetColorString()));
        }
        auto text = point.GetTextInfo();
        if (!text.GetColorString().empty()) {
            text.SetColor(ParseColor(text.GetColorString()));
        }
        point.SetPointInfo(pointInfo);
        point.SetSegmentInfo(segment);
        point.SetTextInfo(text);
    }

    // remove points out of range. get topPoint and bottomPoint.
    sort(points.begin(), points.end(),
        [](LineInfo a, LineInfo b) { return a.GetPointInfo().GetX() < b.GetPointInfo().GetX(); });

    PointInfo headPoint;
    PointInfo topPoint = points.begin()->GetPointInfo();
    PointInfo bottomPoint = points.begin()->GetPointInfo();

    for (auto pointInfo = points.begin(); pointInfo != points.end();) {
        auto point = pointInfo->GetPointInfo();
        auto segment = pointInfo->GetSegmentInfo();
        if (segment.GetSegmentColor() == Color::TRANSPARENT) {
            segment.SetSegmentColor(chartDataset.GetStrokeColor());
            pointInfo->SetSegmentInfo(segment);
        }
        if ((chartType_ == ChartType::LINE && point.GetX() < chartOptions_.GetXAxis().min) ||
            (chartType_ == ChartType::LINE && point.GetX() > chartOptions_.GetXAxis().max) ||
            ((chartType_ != ChartType::LINE && chartType_ != ChartType::BAR)
            && (point.GetY() < chartOptions_.GetYAxis().min || point.GetY() > chartOptions_.GetYAxis().max))) {
            points.erase(pointInfo);
        } else {
            if (point.GetY() > topPoint.GetY()) {
                topPoint = point;
            }
            if (point.GetY() < bottomPoint.GetY()) {
                bottomPoint = point;
            }
            ++pointInfo;
        }
    }
    chartDataset.SetData(points);

    if (!points.empty()) {
        headPoint = points[points.size() - 1].GetPointInfo();
        SetPoint(headPoint, chartOptions_.GetHeadPoint());
        chartDataset.SetHeadPoint(headPoint);
    }

    SetPoint(topPoint, chartOptions_.GetTopPoint());
    chartDataset.SetTopPoint(topPoint);

    SetPoint(bottomPoint, chartOptions_.GetBottomPoint());
    chartDataset.SetBottomPoint(bottomPoint);
}

void DOMChart::PrepareSpecializedComponent()
{
    if (chartType_ == ChartType::GAUGE) {
        if (!progressChild_) {
            progressChild_ =
                AceType::MakeRefPtr<ProgressComponent>(0.0, 0.0, 0.0, PROGRESS_DEFAULT_MAX_VALUE, ProgressType::GAUGE);
        }
        progressChild_->SetValue(percent_);
        progressChild_->SetMaxValue(max_);
        progressChild_->SetMinValue(min_);
        progressChild_->GetTrack()->SetIndicatorFlag(true);
        progressChild_->GetTrack()->SetSectionsStyle(colors_, weights_);
        progressChild_->GetTrack()->SetTrackThickness(strokeWidth_.first);
        progressChild_->GetTrack()->GetTrackInfo()->SetStartDegree(startAngle_.first);
        progressChild_->GetTrack()->GetTrackInfo()->SetSweepDegree(totalAngle_.first);
        progressChild_->GetTrack()->SetCenterX(centerX_.first);
        progressChild_->GetTrack()->SetCenterY(centerY_.first);
        progressChild_->GetTrack()->SetRadius(radius_.first);
    } else if (chartType_ == ChartType::BAR || chartType_ == ChartType::LINE) {
        if (!chartChild_) {
            chartChild_ = AceType::MakeRefPtr<ChartComponent>(chartType_);
        }
        chartChild_->SetHorizontalOption(chartOptions_.GetXAxis());
        chartChild_->SetVerticalOption(chartOptions_.GetYAxis());
        // Convert the data in options to mainchart
        for (auto& charDataset : chartDatasets_) {
            SetChart(charDataset);
        }
        chartChild_->SetMainCharts(chartDatasets_);
    } else if (chartType_ == ChartType::PROGRESS || chartType_ == ChartType::LOADING) {
        dataPanelChild_ = AceType::MakeRefPtr<ProgressDataPanelComponent>(chartType_);
        dataPanelChild_->InitalStyle(GetThemeManager());
        dataPanelChild_->SetEffects(showEffect_);
        if (trackColorSet_) {
            dataPanelChild_->SetTrackColor(backgroundColor_);
        }
        dataPanelChild_->SetAutoScale(autoScale_);
        if (segments_.empty()) {
            LOGI("progress chart hasn't set segment");
            return;
        }
        auto progressDataPanel = AceType::DynamicCast<ProgressDataPanelComponent>(dataPanelChild_);
        if (segments_[0].GetColorType() == SegmentStyleType::USE_COLOR) {
            progressDataPanel->SetStartColor(segments_[0].GetStartColor());
            progressDataPanel->SetEndColor(segments_[0].GetEndColor());
        } else if (segments_[0].GetColorType() == SegmentStyleType::USE_GRADIENT) {
            auto& colorManager = ColorFactory::GetInstance();
            auto colorPair = colorManager.GetColorTuple(segments_[0].GetColorDescriptor());
            progressDataPanel->SetStartColor(colorPair.first);
            progressDataPanel->SetEndColor(colorPair.second);
        }
        // for else case, keep default color
        progressDataPanel->SetProgressValue(segments_[0].GetValue());
        if (strokeWidth_.second) {
            dataPanelChild_->SetThickness(strokeWidth_.first);
        }
    } else if (chartType_ == ChartType::RAINBOW) {
        dataPanelChild_ = AceType::MakeRefPtr<PercentageDataPanelComponent>(chartType_);
        dataPanelChild_->SetAnimationDuration(animationDuration_);
        dataPanelChild_->SetEffects(showEffect_);
        dataPanelChild_->SetAutoScale(autoScale_);
        auto percentageDataPanel = AceType::DynamicCast<PercentageDataPanelComponent>(dataPanelChild_);
        // the angle range is from 0 to 360.
        percentageDataPanel->SetStartDegree(startAngle_.second ? startAngle_.first : 0.0);
        percentageDataPanel->SetSweepDegree(totalAngle_.second ? totalAngle_.first : 360.0);
        percentageDataPanel->ClearSegment();
        if (segments_.empty()) {
            LOGI("progress chart hasn't set segment");
            return;
        }
        for (const auto& segment : segments_) {
            percentageDataPanel->AppendSegment(segment);
        }
        dataPanelChild_->InitalStyle(GetThemeManager());
        if (trackColorSet_) {
            dataPanelChild_->SetTrackColor(backgroundColor_);
        }
        if (strokeWidth_.second) {
            dataPanelChild_->SetThickness(strokeWidth_.first);
        }
    }
}

void DOMChart::CallSpecializedMethod(const std::string& method, const std::string& args)
{
    if (method != DOM_METHOD_APPEND) {
        LOGD("Not support method %{private}s yet!", method.c_str());
        return;
    }
    std::unique_ptr<JsonValue> argsValue = JsonUtil::ParseJsonString(args);
    if (!argsValue || !argsValue->IsArray() || argsValue->GetArraySize() != METHOD_APPEND_ARGS_SIZE) {
        LOGE("parse args error");
        return;
    }

    std::unique_ptr<JsonValue> serialValue = argsValue->GetArrayItem(0)->GetValue("serial");
    if (!serialValue || !serialValue->IsNumber()) {
        LOGE("get serial failed");
        return;
    }
    seriesNum_ = serialValue->GetInt();

    std::unique_ptr<JsonValue> dataValue = argsValue->GetArrayItem(0)->GetValue("data");
    if (!dataValue || !dataValue->IsArray()) {
        LOGE("get data failed");
        return;
    }
    int32_t arraySize = dataValue->GetArraySize();
    auto chartDatas = chartChild_->GetMainCharts();

    if (seriesNum_ < 0 || (static_cast<size_t>(seriesNum_) >= chartDatas.size())) {
        LOGE("series number is greater or equal to the size of chart");
        return;
    }

    if (!isSetFirst_) {
        isSetFirst_ = true;
        for (int32_t i = 0; i < static_cast<int32_t>(chartDatas.size()); i++) {
            lineData_.emplace_back(chartOptions_.GetXAxis().min, false);
        }
    }

    // get the head point position of the series data to be updated.
    position_ = lineData_[seriesNum_].first;
    isResetPosition_ = lineData_[seriesNum_].second;

    for (int32_t i = 0; i < arraySize; i++) {
        std::unique_ptr<JsonValue> coorVal = dataValue->GetArrayItem(i);
        if (!coorVal || !coorVal->IsNumber()) {
            LOGE("get coorVal failed");
            return;
        }
        int32_t coorY = coorVal->GetInt();
        if (coorY <= chartOptions_.GetYAxis().max && coorY >= chartOptions_.GetYAxis().min) {
            UpdateChartData(coorY, chartDatas);
        }
    }

    // save the head point position of the updated series data.
    lineData_[seriesNum_].first = position_;
    lineData_[seriesNum_].second = isResetPosition_;

    UpdateTopButtomPoint(chartDatas);

    chartChild_->SetMainCharts(chartDatas);
    auto node = DOMNode::GetRootComponent();
    node->MarkNeedUpdate();
    auto pipelineContext = pipelineContext_.Upgrade();
    if (!pipelineContext) {
        LOGE("pipelineContext_ is nullptr");
        return;
    }
    pipelineContext->ScheduleUpdate(node);
}

void DOMChart::UpdateTopButtomPoint(std::vector<MainChart>& data)
{
    Point bottomPt = Point(0, 0);
    Point topPt = Point(0, 0);
    auto pointVec = data[seriesNum_].GetData();
    double minY = pointVec[0].GetPointInfo().GetY();
    double maxY = pointVec[0].GetPointInfo().GetY();

    for (auto iter : pointVec) {
        if (iter.GetPointInfo().GetY() <= minY) {
            bottomPt.SetX(iter.GetPointInfo().GetX());
            bottomPt.SetY(iter.GetPointInfo().GetY());
            minY = iter.GetPointInfo().GetY();
        }
        if (iter.GetPointInfo().GetY() >= maxY) {
            topPt.SetX(iter.GetPointInfo().GetX());
            topPt.SetY(iter.GetPointInfo().GetY());
            maxY = iter.GetPointInfo().GetY();
        }
    }
    auto bottomPoint = data[seriesNum_].GetBottomPoint();
    bottomPoint.SetX(bottomPt.GetX());
    bottomPoint.SetY(bottomPt.GetY());
    data[seriesNum_].SetBottomPoint(bottomPoint);

    auto topPoint = data[seriesNum_].GetTopPoint();
    topPoint.SetX(topPt.GetX());
    topPoint.SetY(topPt.GetY());
    data[seriesNum_].SetTopPoint(topPoint);
}

void DOMChart::UpdateChartData(int32_t coorY, std::vector<MainChart>& data)
{
    if (isResetPosition_) {
        position_ = position_ + 1;
    } else {
        position_ = static_cast<int32_t>(data[seriesNum_].GetData().size());
    }
    if (position_ > chartOptions_.GetXAxis().max || position_ < chartOptions_.GetXAxis().min) {
        return;
    }
    if (chartOptions_.GetLoop() && position_ > chartOptions_.GetXAxis().max - 1) {
        isResetPosition_ = true;
        position_ = chartOptions_.GetXAxis().min;
        Point coor = Point(position_, coorY);
        PointInfo point = PointInfo(coor);
        SegmentInfo segment;
        segment.SetSegmentColor(data[seriesNum_].GetStrokeColor());
        LineInfo line = LineInfo(point);
        line.SetSegmentInfo(segment);
        data[seriesNum_].ReplaceData(position_, line);
        data[seriesNum_].SetErasePointNumber(chartOptions_.GetErasePointNumber());
        data[seriesNum_].SetHeadPointIndex(position_);
    } else {
        Point coor = Point(position_, coorY);
        PointInfo point = PointInfo(coor);
        SegmentInfo segment;
        segment.SetSegmentColor(data[seriesNum_].GetStrokeColor());
        LineInfo line = LineInfo(point);
        line.SetSegmentInfo(segment);
        if (!isResetPosition_) {
            data[seriesNum_].AppendData(line);
        } else {
            data[seriesNum_].ReplaceData(position_, line);
            data[seriesNum_].SetHeadPointIndex(position_);
        }
    }
    auto headPoint = data[seriesNum_].GetHeadPoint();
    headPoint.SetX(position_);
    headPoint.SetY(coorY);
    data[seriesNum_].SetHeadPoint(headPoint);
}

} // namespace OHOS::Ace::Framework
