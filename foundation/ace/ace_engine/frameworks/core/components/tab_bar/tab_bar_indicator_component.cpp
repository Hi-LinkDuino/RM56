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

#include "core/components/tab_bar/tab_bar_indicator_component.h"

namespace OHOS::Ace {
namespace {

constexpr double DOUBLE_FACTOR = 2.0;

} // namespace

TabBarIndicatorComponent::TabBarIndicatorComponent(
    const Edge& padding, const Color& color, const Dimension& borderWidth)
    : indicatorStyle_(TabBarIndicatorStyle::DEFAULT), indicatorPadding_(padding), indicatorColor_(color),
      indicatorWidth_(borderWidth)
{
    // build default indicatorDecoration
    Border border;
    border.SetBorderRadius(Radius(borderWidth / DOUBLE_FACTOR));
    indicatorDecoration_ = AceType::MakeRefPtr<Decoration>();
    indicatorDecoration_->SetBorder(border);
    indicatorDecoration_->SetBackgroundColor(color);
    SetHeight(borderWidth.Value(), borderWidth.Unit());
    Initialize();
}

TabBarIndicatorComponent::TabBarIndicatorComponent(const RefPtr<Decoration>& indicatorDecoration)
    : indicatorStyle_(TabBarIndicatorStyle::CUSTOM), indicatorDecoration_(indicatorDecoration)
{
    Initialize();
}

void TabBarIndicatorComponent::Initialize()
{
    if (indicatorStyle_ == TabBarIndicatorStyle::DEFAULT) {
        SetFlex(BoxFlex::FLEX_X);
    } else {
        SetFlex(BoxFlex::FLEX_XY);
    }

    if (indicatorDecoration_) {
        SetBackDecoration(indicatorDecoration_);
    }
}

} // namespace OHOS::Ace