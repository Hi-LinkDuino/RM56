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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_HYPERLINK_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_HYPERLINK_COMPONENT_H

#include <string>

#include "base/utils/macros.h"
#include "core/components/flex/flex_component.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/component_group.h"

namespace OHOS::Ace {

class ACE_EXPORT HyperlinkComponent : public ComponentGroup {
    DECLARE_ACE_TYPE(HyperlinkComponent, ComponentGroup);

public:
    explicit HyperlinkComponent(const std::list<RefPtr<Component>>& children) : ComponentGroup(children) {}
    ~HyperlinkComponent() override = default;

    RefPtr<RenderNode> CreateRenderNode() override;
    RefPtr<Element> CreateElement() override;

    ACE_DEFINE_COMPONENT_PROP(Address, std::string);
    ACE_DEFINE_COMPONENT_PROP(Summary, std::string);
    ACE_DEFINE_COMPONENT_PROP(Color, Color, Color::BLACK);
    void SetImageChildColor(RefPtr<Component>& node);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_HYPERLINK_HYPERLINK_COMPONENT_H