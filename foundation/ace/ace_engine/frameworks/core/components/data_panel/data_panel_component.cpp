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

#include "core/components/data_panel/data_panel_component.h"

#include "core/components/data_panel/data_panel_element.h"
#include "core/components/data_panel/data_panel_theme.h"
#include "core/components/data_panel/render_data_panel.h"

namespace OHOS::Ace {
namespace {

const std::string DATA_PANEL_COLOR_PREFIX = "DATA_PANEL_LIGHT_COLOR_";

}

RefPtr<RenderNode> ProgressDataPanelComponent::CreateRenderNode()
{
    return RenderProgressDataPanel::Create();
}

RefPtr<RenderNode> PercentageDataPanelComponent::CreateRenderNode()
{
    return RenderPercentageDataPanel::Create();
}

RefPtr<Element> DataPanelComponent::CreateElement()
{
    return AceType::MakeRefPtr<DataPanelElement>();
}

void DataPanelComponent::InitalStyle(const RefPtr<ThemeManager>& themeManager)
{
    auto theme = themeManager->GetTheme<DataPanelTheme>();
    if (!theme) {
        LOGE("fail to initialize theme because theme is null");
        return;
    }
    SetTrackColor(theme->GetBackgroundColor());
    SetDefaultHeight(theme->GetDefaultHeight());
    SetDefaultWidth(theme->GetDefaultWidth());
    SetThickness(theme->GetThickness());
};

void PercentageDataPanelComponent::InitalStyle(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        LOGE("fail to initialize, themeManager is null");
        return;
    }
    DataPanelComponent::InitalStyle(themeManager);
    auto theme = themeManager->GetTheme<DataPanelTheme>();
    if (!theme) {
        LOGE("fail to initialize theme because theme is null");
        return;
    }
    uint32_t index = 0;
    auto colors = theme->GetColorsArray();
    for (auto& segment : segments_) {
        if (segment.GetColorType() == SegmentStyleType::NONE && index < colors.size()) {
            segment.SetStartColor(colors[index].first);
            segment.SetEndColor(colors[index].second);
        }
        index += 1;
    }
}

void ProgressDataPanelComponent::InitalStyle(const RefPtr<ThemeManager>& themeManager)
{
    if (!themeManager) {
        LOGE("fail to initialize, themeManager is null");
        return;
    }
    DataPanelComponent::InitalStyle(themeManager);
    auto theme = themeManager->GetTheme<DataPanelTheme>();
    if (!theme) {
        LOGE("fail to initialize theme because theme is null");
        return;
    }
    if (type_ == ChartType::LOADING) {
        startColor_ = theme->GetLoadingColor().first;
        endColor_ = theme->GetLoadingColor().second;
    } else {
        startColor_ = theme->GetProgressColor().first;
        endColor_ = theme->GetProgressColor().second;
    }
}

} // namespace OHOS::Ace