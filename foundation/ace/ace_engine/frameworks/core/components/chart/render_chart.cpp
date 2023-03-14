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

#include "core/components/chart/render_chart.h"

#include "core/common/font_manager.h"

namespace OHOS::Ace {

void RenderChart::Update(const RefPtr<Component>& component)
{
    auto chartComponent = AceType::DynamicCast<ChartComponent>(component);
    if (!chartComponent) {
        LOGI("convert to chart component fail");
        return;
    }
    type_ = chartComponent->GetChartType();
    horizontal_ = chartComponent->GetHorizontalOption();
    vertical_ = chartComponent->GetVerticalOption();
    mainCharts_ = chartComponent->GetMainCharts();

    auto context = context_.Upgrade();
    if (!context) {
        return;
    }
    auto fontManager = context->GetFontManager();
    if (!fontManager) {
        return;
    }
    auto finishLoadFontCallback = [weakchart = AceType::WeakClaim(this)] {
        auto chart = weakchart.Upgrade();
        if (chart) {
            chart->MarkNeedLayout();
        }
    };
    if (mainCharts_.empty()) {
        return;
    }
    for (const auto& familyName : mainCharts_[0].GetFontFamily()) {
        fontManager->RegisterCallback(AceType::WeakClaim(this), familyName, finishLoadFontCallback);
    }
}

void RenderChart::PerformLayout()
{
    SetLayoutSize(GetLayoutParam().GetMaxSize());
}

void RenderChart::OnPaintFinish()
{
    UpdateAccessibilityAttr();
}

void RenderChart::UpdateAccessibilityAttr()
{
    auto accessibilityNode = GetAccessibilityNode().Upgrade();
    if (!accessibilityNode) {
        return;
    }
    int index = 0;
    for (const auto& data : mainCharts_) {
        std::vector<std::pair<std::string, double>> info;
        for (const auto& pointInfo : data.GetData()) {
            const auto& point = pointInfo.GetPointInfo();
            info.emplace_back(std::make_pair<std::string, double>(std::to_string(point.GetX()), point.GetY()));
        }
        accessibilityNode->PutChartValue(std::to_string(index), info);
        index += 1;
    }
}

} // namespace OHOS::Ace
