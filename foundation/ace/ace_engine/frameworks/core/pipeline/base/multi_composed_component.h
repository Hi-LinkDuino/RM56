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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_MULTICOMPOSED_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_MULTICOMPOSED_COMPONENT_H

#include <string>

#include "core/pipeline/base/base_composed_component.h"
#include "core/pipeline/base/multi_child.h"

namespace OHOS::Ace {

// A component can compose others components.
class ACE_EXPORT MultiComposedComponent : public BaseComposedComponent, public MultiChild {
    DECLARE_ACE_TYPE(MultiComposedComponent, BaseComposedComponent, MultiChild);

public:
    MultiComposedComponent(const ComposeId& id, const std::string& name) : BaseComposedComponent(id, name) {}
    MultiComposedComponent(const ComposeId& id, const std::string& name, const std::list<RefPtr<Component>>& components)
        : BaseComposedComponent(id, name), MultiChild(components) {}
    ~MultiComposedComponent() override = default;

    RefPtr<Element> CreateElement() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPOSED_COMPONENT_H
