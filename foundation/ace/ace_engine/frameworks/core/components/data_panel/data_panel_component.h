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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_COMPONENT_H

#include <vector>

#include "core/components/chart/chart_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/theme/theme_manager.h"
#include "core/pipeline/base/render_component.h"

namespace OHOS::Ace {

enum class SegmentStyleType {
    USE_GRADIENT,
    USE_COLOR,
    NONE, // user hasn't set color in css.
};

class ACE_EXPORT Segment final {
public:
    Segment() = default;
    ~Segment() = default;

    void SetSegmentName(const std::string& name)
    {
        name_ = name;
    }

    const std::string& GetSegmentName() const
    {
        return name_;
    }

    void SetStartColor(const Color& color)
    {
        startColor_ = color;
    }

    const Color& GetStartColor() const
    {
        return startColor_;
    }

    void SetEndColor(const Color& color)
    {
        endColor_ = color;
    }

    const Color& GetEndColor() const
    {
        return endColor_;
    }

    void SetColorDescriptor(const std::string& colorDescriptor)
    {
        colorDescriptor_ = colorDescriptor;
    }

    const std::string& GetColorDescriptor() const
    {
        return colorDescriptor_;
    }

    SegmentStyleType GetColorType() const
    {
        return style_;
    }

    void SetColorType(SegmentStyleType type)
    {
        style_ = type;
    }

    double GetValue() const
    {
        return value_;
    }

    void SetValue(double value)
    {
        value_ = value;
    }

private:
    std::string name_;
    std::string colorDescriptor_;
    Color startColor_;
    Color endColor_;
    double value_ = 0.0;
    SegmentStyleType style_ = SegmentStyleType::NONE;
};

class DataPanelComponent : public RenderComponent {
    DECLARE_ACE_TYPE(DataPanelComponent, RenderComponent);

public:
    RefPtr<Element> CreateElement() override;

    // determine the data panel type before call InitalStyle
    virtual void InitalStyle(const RefPtr<ThemeManager>& themeManager);

    void SetMeasureType(MeasureType type)
    {
        measureType_ = type;
    }

    MeasureType GetMeasureType() const
    {
        return measureType_;
    }

    void SetAutoScale(bool autoScale)
    {
        autoScale_ = autoScale;
    }

    bool GetAutoScale() const
    {
        return autoScale_;
    }

    virtual ChartType GetPanelType() const = 0;

    void SetPanelType(ChartType type)
    {
        type_ = type;
    }

    const Dimension& GetDefaultHeight() const
    {
        return height_;
    }

    const Dimension& GetDefaultWidth() const
    {
        return width_;
    }

    const Dimension& GetThickness() const
    {
        return thickness_;
    }

    void SetThickness(const Dimension& thickness)
    {
        thickness_ = thickness;
    }

    void SetEffects(bool effects)
    {
        showEffects_ = effects;
    }

    bool GetEffects() const
    {
        return showEffects_;
    }

    void SetTrackColor(const Color& backgroundTrackColor)
    {
        backgroundTrackColor_ = backgroundTrackColor;
    }

    Color GetTrackColor() const
    {
        return backgroundTrackColor_;
    }

    double GetAnimationDuration() const
    {
        return animationDuration_;
    }

    void SetAnimationDuration(double animationDuration)
    {
        animationDuration_ = animationDuration;
    }

protected:
    void SetDefaultHeight(const Dimension& height)
    {
        height_ = height;
    }

    void SetDefaultWidth(const Dimension& width)
    {
        width_ = width;
    }

    ChartType type_ = ChartType::PROGRESS;
    double animationDuration_ = -1.0;

private:
    MeasureType measureType_ = MeasureType::DEFAULT;
    Dimension thickness_;
    // data panel default height and width
    Dimension height_;
    Dimension width_;
    bool showEffects_ = false;
    Color backgroundTrackColor_;
    // autoScale is used to resize the thickness based on radius.
    bool autoScale_ = false;
};

class ProgressDataPanelComponent : public DataPanelComponent {
    DECLARE_ACE_TYPE(ProgressDataPanelComponent, DataPanelComponent);

public:
    ProgressDataPanelComponent() = default;
    ~ProgressDataPanelComponent() = default;

    explicit ProgressDataPanelComponent(ChartType type)
    {
        type_ = type;
    }

    RefPtr<RenderNode> CreateRenderNode() override;
    ChartType GetPanelType() const override
    {
        return type_;
    }

    // InitalStyle will assign UX design color.
    // call this method after type determined.
    // this method will override color.
    void InitalStyle(const RefPtr<ThemeManager>& themeManager) override;

    const Color& GetStartColor() const
    {
        return startColor_;
    }

    void SetStartColor(const Color& color)
    {
        startColor_ = color;
    }

    const Color& GetEndColor() const
    {
        return endColor_;
    }

    void SetEndColor(const Color& color)
    {
        endColor_ = color;
    }

    double GetProgressValue() const
    {
        return progress_;
    }

    void SetProgressValue(double progress)
    {
        progress_ = progress;
    }

    bool IsLoading() const
    {
        return isLoading_;
    }

    void SetIsLoadingType(bool isLoading)
    {
        isLoading_ = isLoading;
    }

private:
    Color startColor_;
    Color endColor_;
    double progress_ = 0.0;
    bool isLoading_ = false;
};

class ACE_EXPORT PercentageDataPanelComponent : public DataPanelComponent {
    DECLARE_ACE_TYPE(PercentageDataPanelComponent, DataPanelComponent);

public:
    PercentageDataPanelComponent() = default;
    ~PercentageDataPanelComponent() = default;

    explicit PercentageDataPanelComponent(ChartType type)
    {
        type_ = type;
    }

    RefPtr<RenderNode> CreateRenderNode() override;

    // InitalStyle will assign UX design color
    void InitalStyle(const RefPtr<ThemeManager>& themeManager) override;

    ChartType GetPanelType() const override
    {
        return type_;
    }

    void AppendSegment(const Segment& segment)
    {
        segments_.push_back(segment);
        totalValue_ += segment.GetValue();
    }

    void ClearSegment()
    {
        segments_.clear();
        totalValue_ = 0.0;
    }

    const std::vector<Segment>& GetSegments() const
    {
        return segments_;
    }

    double GetStartDegree() const
    {
        return startDegree_;
    }

    void SetStartDegree(double startDegree)
    {
        startDegree_ = startDegree;
    }

    double GetSweepDegree() const
    {
        return sweepDegree_;
    }

    void SetSweepDegree(double sweepDegree)
    {
        sweepDegree_ = sweepDegree;
    }

    double GetTotalValue() const
    {
        return totalValue_;
    }

    void SetMaxValue(double maxValue)
    {
        maxValue_ = maxValue;
    }

    double GetMaxValue() const
    {
        return maxValue_;
    }

private:
    double startDegree_ = 0.0;
    double sweepDegree_ = 360.0;
    std::vector<Segment> segments_;
    double totalValue_ = 0.0;
    double maxValue_ = 100.0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_DATA_PANEL_DATA_PANEL_COMPONENT_H
