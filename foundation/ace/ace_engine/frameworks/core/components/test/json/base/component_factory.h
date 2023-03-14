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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_BASE_COMPONENT_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_BASE_COMPONENT_FACTORY_H

#include <cstdint>
#include <list>

#include "base/memory/referenced.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

// ComponentFactory is for create a Component using dsl data.
class ComponentFactory {
public:
    virtual ~ComponentFactory() = default;
    virtual RefPtr<Component> CreateComponentFromDsl(const uint8_t* dslBuf, size_t dslLen) = 0;
    virtual std::list<RefPtr<Component>> CreateComponentsFromDsl(const uint8_t* dslBuf, size_t dslLen) = 0;

    static ComponentFactory& GetInstance();
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_BASE_COMPONENT_FACTORY_H
