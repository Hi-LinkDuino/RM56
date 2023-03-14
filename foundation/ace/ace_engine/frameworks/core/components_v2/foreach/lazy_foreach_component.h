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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_FOREACH_LAZY_FOREACH_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_FOREACH_LAZY_FOREACH_COMPONENT_H

#include "base/memory/ace_type.h"
#include "base/utils/macros.h"
#include "core/components/foreach/for_each_component.h"
#include "core/components_v2/common/common_def.h"

namespace OHOS::Ace::V2 {

class DataChangeListener : virtual public Referenced {
public:
    virtual void OnDataReloaded() = 0;
    virtual void OnDataAdded(size_t index) = 0;
    virtual void OnDataDeleted(size_t index) = 0;
    virtual void OnDataChanged(size_t index) = 0;
    virtual void OnDataMoved(size_t from, size_t to) = 0;
};

class ACE_EXPORT LazyForEachComponent : public V1::ForEachComponent {
    DECLARE_ACE_TYPE(V2::LazyForEachComponent, V1::ForEachComponent);

public:
    explicit LazyForEachComponent(const std::string& id) : V1::ForEachComponent(id, "LazyForEach") {}
    ~LazyForEachComponent() override = default;

    RefPtr<Element> CreateElement() override;

    size_t Count() override
    {
        return TotalCount();
    }

    size_t TotalCount();
    RefPtr<Component> GetChildByIndex(size_t index);

    virtual void ReleaseChildGroupByComposedId(const std::string& composedId) {}
    virtual void RegisterDataChangeListener(const RefPtr<DataChangeListener>& listener) = 0;
    virtual void UnregisterDataChangeListener(const RefPtr<DataChangeListener>& listener) = 0;

protected:
    virtual size_t OnGetTotalCount() = 0;
    virtual RefPtr<Component> OnGetChildByIndex(size_t index) = 0;

    std::list<RefPtr<Component>>& ExpandChildren() override;

    bool Expanded() const
    {
        return expanded_;
    }

private:
    bool expanded_ = false;

    ACE_DISALLOW_COPY_AND_MOVE(LazyForEachComponent);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_FOREACH_LAZY_FOREACH_COMPONENT_H
