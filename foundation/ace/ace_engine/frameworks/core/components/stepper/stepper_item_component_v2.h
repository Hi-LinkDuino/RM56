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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ITEM_COMPONENT_V2_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ITEM_COMPONENT_V2_H

#include "core/components/stepper/stepper_item_component.h"
#include "frameworks/bridge/common/dom/dom_node.h"

namespace OHOS::Ace {
class ACE_EXPORT StepperItemComponentV2 : public FlexComponent {
    DECLARE_ACE_TYPE(StepperItemComponentV2, FlexComponent);

public:
    StepperItemComponentV2(FlexDirection direction, FlexAlign mainAxisAlign, FlexAlign crossAxisAlign,
        const std::list<RefPtr<Component>>& children);
    ~StepperItemComponentV2() override = default;
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_STEPPER_STEPPER_ITEM_COMPONENT_V2_H
