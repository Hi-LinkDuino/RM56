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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_COMPONENT_H

#include <string>

#include "base/utils/utils.h"
#include "core/components/box/box_component.h"
#include "core/components/button/button_component.h"
#include "core/components/common/properties/color.h"
#include "core/components/counter/counter_theme.h"
#include "core/components/display/display_component.h"
#include "core/components/divider/divider_component.h"
#include "core/components/flex/flex_component.h"
#include "core/components/padding/padding_component.h"
#include "core/components/text/text_component.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/component_group.h"
#include "core/pipeline/base/element.h"
#include "core/pipeline/base/sole_child_component.h"
#include "core/pipeline/pipeline_context.h"

namespace OHOS::Ace {

class ACE_EXPORT CounterComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(CounterComponent, ComponentGroup);

public:
    explicit CounterComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children) {}

    ~CounterComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;

    RefPtr<Element> CreateElement() override;

    RefPtr<Component> BuildChild(const RefPtr<ThemeManager>& themeManager);

    RefPtr<DividerComponent> BuildDivider();

    void BuildControl(const RefPtr<CounterTheme>& counterTheme, const RefPtr<RowComponent>& row,
        RefPtr<ButtonComponent>& button, std::string content, bool isLeft);

    void BuildContent(
        const RefPtr<CounterTheme>& counterTheme, const RefPtr<RowComponent>& row, RefPtr<BoxComponent>& box);

    ACE_DEFINE_COMPONENT_PROP(ControlRadius, Dimension, 4.0_vp);
    ACE_DEFINE_COMPONENT_PROP(ControlWidth, Dimension, 32.0_vp);
    ACE_DEFINE_COMPONENT_PROP(Width, Dimension, 100.0_vp);
    ACE_DEFINE_COMPONENT_PROP(Height, Dimension, 32.0_vp);
    ACE_DEFINE_COMPONENT_PROP(State, bool, true);
    ACE_DEFINE_COMPONENT_PROP(BackgroundColor, Color, Color::WHITE);

    ACE_DEFINE_COMPONENT_EVENT(OnInc, void());
    ACE_DEFINE_COMPONENT_EVENT(OnDec, void());

private:
    RefPtr<ButtonComponent> increButtonComponent_;
    RefPtr<ButtonComponent> decreButtonComponent_;
    RefPtr<TextComponent> textComponent_;
    RefPtr<DisplayComponent> displayComponent_;
    RefPtr<CounterTheme> counterTheme_;
    ACE_DISALLOW_COPY_AND_MOVE(CounterComponent);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_COMPONENT_H
