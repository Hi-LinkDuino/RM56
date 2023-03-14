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

#include "core/components/common/properties/color_factory.h"

namespace OHOS::Ace {

ColorFactory::ColorFactory()
{
    colorMap_["DATA_PANEL_LIGHT_COLOR_1"] =
        std::make_pair<Color, Color>(Color::FromString("#F7CE00"), Color::FromString("#F99B11"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_2"] =
        std::make_pair<Color, Color>(Color::FromString("#F76223"), Color::FromString("#F2400A"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_3"] =
        std::make_pair<Color, Color>(Color::FromString("#F772AC"), Color::FromString("#E65392"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_4"] =
        std::make_pair<Color, Color>(Color::FromString("#A575EB"), Color::FromString("#A12DF7"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_5"] =
        std::make_pair<Color, Color>(Color::FromString("#7B79F7"), Color::FromString("#4B48F7"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_6"] =
        std::make_pair<Color, Color>(Color::FromString("#4B8AF3"), Color::FromString("#007DFF"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_7"] =
        std::make_pair<Color, Color>(Color::FromString("#73C1E6"), Color::FromString("#4FB4E3"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_8"] =
        std::make_pair<Color, Color>(Color::FromString("#A5D61D"), Color::FromString("#69D14F"));
    colorMap_["DATA_PANEL_LIGHT_COLOR_9"] =
        std::make_pair<Color, Color>(Color::FromString("#A2A2B0"), Color::FromString("#8E8E93"));

    colorMap_["DATA_PANEL_DARK_COLOR_1"] =
        std::make_pair<Color, Color>(Color::FromString("#D1A738"), Color::FromString("#EB933D"));
    colorMap_["DATA_PANEL_DARK_COLOR_2"] =
        std::make_pair<Color, Color>(Color::FromString("#E67D50"), Color::FromString("#D9542B"));
    colorMap_["DATA_PANEL_DARK_COLOR_3"] =
        std::make_pair<Color, Color>(Color::FromString("#D5749E"), Color::FromString("#D6568D"));
    colorMap_["DATA_PANEL_DARK_COLOR_4"] =
        std::make_pair<Color, Color>(Color::FromString("#9973D1"), Color::FromString("#8639BF"));
    colorMap_["DATA_PANEL_DARK_COLOR_5"] =
        std::make_pair<Color, Color>(Color::FromString("#7977D9"), Color::FromString("#5552D9"));
    colorMap_["DATA_PANEL_DARK_COLOR_6"] =
        std::make_pair<Color, Color>(Color::FromString("#4C81D9"), Color::FromString("#217BD9"));
    colorMap_["DATA_PANEL_DARK_COLOR_7"] =
        std::make_pair<Color, Color>(Color::FromString("#5EA6D1"), Color::FromString("#4895C2"));
    colorMap_["DATA_PANEL_DARK_COLOR_8"] =
        std::make_pair<Color, Color>(Color::FromString("#91C23A"), Color::FromString("#70BA5D"));
    colorMap_["DATA_PANEL_DARK_COLOR_9"] =
        std::make_pair<Color, Color>(Color::FromString("#8C8C99"), Color::FromString("#6B6B76"));

    colorMap_["DATA_PANEL_PROGRESS_LIGHT_COLOR_1"] =
        std::make_pair<Color, Color>(Color::FromString("#86C1FF"), Color::FromString("#254FF7"));
    colorMap_["DATA_PANEL_PROGRESS_DARK_COLOR_1"] =
        std::make_pair<Color, Color>(Color::FromString("#72A4D9"), Color::FromString("#3B61F7"));
}

ColorFactory& ColorFactory::GetInstance()
{
    // don't need lock, because of no race condition.
    static auto instance = new ColorFactory();
    return *instance;
}

std::pair<Color, Color> ColorFactory::GetColorTuple(const std::string& colorName)
{
    auto colorPair = colorMap_.find(colorName);
    if (colorPair != colorMap_.end()) {
        return colorPair->second;
    }
    return std::make_pair<Color, Color>(Color(), Color());
}

} // namespace OHOS::Ace