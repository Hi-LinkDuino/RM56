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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASED_COMPOSED_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASED_COMPOSED_COMPONENT_H

#include <string>

#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

using ComposeId = std::string;

// A component can compose others components.
class ACE_EXPORT BaseComposedComponent : public Component {
    DECLARE_ACE_TYPE(BaseComposedComponent, Component);

public:
    BaseComposedComponent(const ComposeId& id, const std::string& name) : id_(id), name_(name) {}
    ~BaseComposedComponent() override = default;

    const std::string& GetName() const
    {
        return name_;
    }

    bool NeedUpdate() const
    {
        return needUpdate_;
    }

    void MarkNeedUpdate()
    {
        needUpdate_ = true;
    }

    void ClearNeedUpdate()
    {
        needUpdate_ = false;
    }

    const ComposeId& GetId() const
    {
        return id_;
    }

protected:
    ComposeId id_;

private:
    std::string name_;
    bool needUpdate_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_COMPOSED_COMPONENT_H
