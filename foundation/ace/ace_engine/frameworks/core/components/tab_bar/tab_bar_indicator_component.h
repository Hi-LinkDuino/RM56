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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_INDICATOR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_INDICATOR_COMPONENT_H

#include "core/components/box/box_component.h"

namespace OHOS::Ace {

inline const Dimension DEFAULT_INDICATOR_WIDTH = Dimension(2.0, DimensionUnit::VP);
inline const Color DEFAULT_INDICATOR_COLOR = Color(0xFF254ff7);
inline const Edge DEFAULT_INDICATOR_PADDING = Edge(0.0);

enum class TabBarIndicatorStyle {
    DEFAULT,
    CUSTOM,
};

class ACE_EXPORT TabBarIndicatorComponent : public BoxComponent {
    DECLARE_ACE_TYPE(TabBarIndicatorComponent, BoxComponent);

public:
    explicit TabBarIndicatorComponent(const Edge& padding = DEFAULT_INDICATOR_PADDING,
        const Color& color = DEFAULT_INDICATOR_COLOR, const Dimension& borderWidth = DEFAULT_INDICATOR_WIDTH);
    explicit TabBarIndicatorComponent(const RefPtr<Decoration>& indicatorDecoration);
    ~TabBarIndicatorComponent() override = default;

    TabBarIndicatorStyle GetIndicatorStyle() const
    {
        return indicatorStyle_;
    }

    const Edge& GetIndicatorPadding() const
    {
        return indicatorPadding_;
    }

    const Color& GetIndicatorColor() const
    {
        return indicatorColor_;
    }

    Dimension GetIndicatorWidth() const
    {
        return indicatorWidth_;
    }

    const RefPtr<Decoration>& GetIndicatorDecoration() const
    {
        return indicatorDecoration_;
    }

private:
    void Initialize();

    TabBarIndicatorStyle indicatorStyle_ { TabBarIndicatorStyle::DEFAULT };
    Edge indicatorPadding_;
    Color indicatorColor_ { DEFAULT_INDICATOR_COLOR };
    Dimension indicatorWidth_ { DEFAULT_INDICATOR_WIDTH };
    RefPtr<Decoration> indicatorDecoration_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_INDICATOR_COMPONENT_H
