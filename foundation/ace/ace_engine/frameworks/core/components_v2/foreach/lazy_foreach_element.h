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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_FOREACH_LAZY_FOREACH_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_FOREACH_LAZY_FOREACH_ELEMENT_H

#include "base/utils/macros.h"
#include "core/components/foreach/for_each_element.h"
#include "core/components_v2/common/common_def.h"
#include "core/components_v2/foreach/lazy_foreach_component.h"

namespace OHOS::Ace::V2 {

class LazyForEachElement : public V1::ForEachElement {
    DECLARE_ACE_TYPE(V2::LazyForEachElement, V1::ForEachElement);

public:
    explicit LazyForEachElement(const std::string& id) : V1::ForEachElement(id) {}
    ~LazyForEachElement() override = default;

    bool CanUpdate(const RefPtr<Component>& newComponent) override;
    void Update() override;
    void SetNewComponent(const RefPtr<Component>& newComponent) override;

private:
    RefPtr<LazyForEachComponent> lazyForEachComponent_;

    ACE_DISALLOW_COPY_AND_MOVE(LazyForEachElement);
};

} // namespace OHOS::Ace::V2

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_FOREACH_LAZY_FOREACH_ELEMENT_H
