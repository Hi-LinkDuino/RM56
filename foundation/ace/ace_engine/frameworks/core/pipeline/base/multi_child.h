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

#ifndef FOUNDATION_ACE_CORE_COMPONENTS_MULTI_CHILD_H
#define FOUNDATION_ACE_CORE_COMPONENTS_MULTI_CHILD_H

#include <list>

#include "base/memory/ace_type.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class MultiChild : public virtual AceType {
    DECLARE_ACE_TYPE(MultiChild, AceType);

public:
    MultiChild() = default;
    explicit MultiChild(const std::list<RefPtr<Component>>& children) : children_(children) {}
    ~MultiChild() override = default;

    const std::list<RefPtr<Component>>& GetChildren()
    {
        return ExpandChildren();
    }

    void AddChild(const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }

        auto& children = ExpandChildren();
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            return;
        }

        child->SetParent(AceType::WeakClaim(AceType::DynamicCast<Component>(this)));
        children.emplace_back(child);
    }

    void RemoveChild(const RefPtr<Component>& child)
    {
        if (!child) {
            return;
        }

        auto& children = ExpandChildren();
        auto it = std::find(children.begin(), children.end(), child);
        if (it == children.end()) {
            return;
        }

        child->SetParent(nullptr);
        children.erase(it);
    }

    void RemoveChildren()
    {
        auto& children = ExpandChildren();
        for (auto& child : children) {
            child->SetParent(nullptr);
        }
        children.clear();
    }

    virtual size_t Count()
    {
        const auto& children = GetChildren();
        size_t count = children.size();
        for (const auto& child : children) {
            auto multiChild = AceType::DynamicCast<MultiChild>(child);
            if (multiChild) {
                --count;
                count += multiChild->Count();
            }
        }
        return count;
    }

protected:
    virtual std::list<RefPtr<Component>>& ExpandChildren()
    {
        return children_;
    }

    std::list<RefPtr<Component>> children_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_CORE_COMPONENTS_MULTI_CHILD_H
