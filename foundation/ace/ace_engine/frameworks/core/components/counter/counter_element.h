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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_ELEMENT_H

#include "core/components/counter/counter_component.h"
#include "core/components/counter/render_counter.h"
#include "core/focus/focus_node.h"

namespace OHOS::Ace {

class CounterElement : public RenderElement {
    DECLARE_ACE_TYPE(CounterElement, RenderElement);

public:
    CounterElement() = default;
    ~CounterElement() override = default;

    void PerformBuild() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_COUNTER_COUNTER_ELEMENT_H
