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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_ROW_SPLIT_ELEMENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_ROW_SPLIT_ELEMENT_H

#include "core/pipeline/base/component_group_element.h"
#include "core/components/split_container/split_container_element.h"

namespace OHOS::Ace {

class RowSplitElement : public SplitContainerElement {
    DECLARE_ACE_TYPE(RowSplitElement, SplitContainerElement)

public:
    RowSplitElement() = default;
    ~RowSplitElement() override = default;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_SPLIT_CONTAINER_ROW_SPLIT_ELEMENT_H