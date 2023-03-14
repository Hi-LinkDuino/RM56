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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ITEM_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ITEM_COMPONENT_H

#include "core/components/box/box_component.h"
#include "core/components/image/image_component.h"
#include "core/components/text/text_component.h"

namespace OHOS::Ace {

class ACE_EXPORT TabBarItemComponent : public BoxComponent {
    DECLARE_ACE_TYPE(TabBarItemComponent, BoxComponent);

public:
    explicit TabBarItemComponent(const RefPtr<Component>& child);
    TabBarItemComponent(const std::string& text, const RefPtr<Component>& imageComponent);
    ~TabBarItemComponent() override = default;
    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;
    void UpdateStyle(const TextStyle& textStyle, const Color& color);

private:
    RefPtr<TextComponent> text_;
    RefPtr<ImageComponent> icon_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TAB_BAR_TAB_BAR_ITEM_COMPONENT_H
