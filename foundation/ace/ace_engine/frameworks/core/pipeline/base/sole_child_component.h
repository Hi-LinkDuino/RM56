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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SOLE_CHILD_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SOLE_CHILD_COMPONENT_H

#include "core/pipeline/base/render_component.h"
#include "core/pipeline/base/single_child.h"
#include "core/pipeline/base/sole_child_element.h"

namespace OHOS::Ace {

class SoleChildComponent : public RenderComponent, public SingleChild {
    DECLARE_ACE_TYPE(SoleChildComponent, RenderComponent, SingleChild);

public:
    SoleChildComponent() = default;
    explicit SoleChildComponent(const RefPtr<Component>& child) : SingleChild(child) {}
    ~SoleChildComponent() override = default;

    RefPtr<Element> CreateElement() override
    {
        return AceType::MakeRefPtr<SoleChildElement>();
    }

    void SetUpdateType(UpdateType updateType) override
    {
        RenderComponent::SetUpdateType(updateType);
        auto child = GetChild();
        if (child) {
            child->SetUpdateType(updateType);
        }
    }

    void SetDisabledStatus(bool disabledStatus) override
    {
        RenderComponent::SetDisabledStatus(disabledStatus);
        auto child = GetChild();
        if (child) {
            child->SetDisabledStatus(disabledStatus);
        }
    }

    void SetTextDirection(TextDirection direction) override
    {
        RenderComponent::SetTextDirection(direction);
        auto child = GetChild();
        if (child) {
            child->SetTextDirection(direction);
        }
    }
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SOLE_CHILD_COMPONENT_H
