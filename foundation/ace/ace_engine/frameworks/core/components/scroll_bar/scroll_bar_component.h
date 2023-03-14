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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_SCROLL_BAR_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_SCROLL_BAR_COMPONENT_H

#include "base/geometry/axis.h"
#include "core/components/common/properties/scroll_bar.h"
#include "core/components/scroll_bar/scroll_bar_proxy.h"
#include "core/components_v2/common/common_def.h"
#include "core/pipeline/base/sole_child_component.h"

namespace OHOS::Ace {

class ACE_EXPORT ScrollBarComponent : public SoleChildComponent {
    DECLARE_ACE_TYPE(ScrollBarComponent, SoleChildComponent);

public:
    explicit ScrollBarComponent(const RefPtr<Component>& child) : SoleChildComponent(child) {}
    ~ScrollBarComponent() override = default;

    RefPtr<Element> CreateElement() override;
    RefPtr<RenderNode> CreateRenderNode() override;

    ACE_DEFINE_COMPONENT_PROP(Axis, Axis, Axis::VERTICAL);
    ACE_DEFINE_COMPONENT_PROP(DisplayMode, DisplayMode, DisplayMode::AUTO);
    ACE_DEFINE_COMPONENT_PROP(ScrollBarProxy, RefPtr<ScrollBarProxy>);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SCROLL_BAR_SCROLL_BAR_COMPONENT_H
