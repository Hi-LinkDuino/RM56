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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SINGLE_CHILD_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SINGLE_CHILD_H

#include "base/memory/ace_type.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class SingleChild : public virtual AceType {
    DECLARE_ACE_TYPE(SingleChild, AceType);

public:
    SingleChild() = default;
    explicit SingleChild(const RefPtr<Component>& child) : child_(child) {}
    ~SingleChild() override = default;

    const RefPtr<Component>& GetChild() const
    {
        return child_;
    }

    void SetChild(const RefPtr<Component>& child)
    {
        SetChildDirectly(child);
        if (child) {
            OnChildAdded(child);
        }
    }

    void SetChildDirectly(const RefPtr<Component>& child)
    {
        // Clear preview child's parent.
        if (child_) {
            child_->SetParent(nullptr);
        }

        child_ = child;

        // Set current child's parent.
        if (child) {
            auto parent = AceType::DynamicCast<Component>(this);
            child->SetParent(WeakClaim(parent));
        }
    }

    virtual void OnChildAdded(const RefPtr<Component>& child) {}

private:
    RefPtr<Component> child_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_BASE_SINGLE_CHILD_H
