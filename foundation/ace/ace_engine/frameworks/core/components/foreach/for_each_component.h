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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_FOR_EACH_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_DECLARATIVE_FRONTEND_COMPONENTS_FOR_EACH_COMPONENT_H

#include "base/utils/macros.h"
#include "core/pipeline/base/multi_composed_component.h"

namespace OHOS::Ace {

class ACE_EXPORT ForEachComponent : public MultiComposedComponent {
    DECLARE_ACE_TYPE(ForEachComponent, MultiComposedComponent);

public:
    ForEachComponent(const ComposeId& id, const std::string& name) : MultiComposedComponent(id, name)
    {
        SetUpdateType(UpdateType::REBUILD);
    }
    ~ForEachComponent() override = default;

    RefPtr<Element> CreateElement() override;
};

} // namespace OHOS::Ace

#endif
