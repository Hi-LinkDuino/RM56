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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_COMPONENT_FACTORY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_COMPONENT_FACTORY_H

#include <cstdint>
#include <list>

#include "base/json/json_util.h"
#include "core/components/test/json/base/component_factory.h"
#include "core/pipeline/base/component.h"

namespace OHOS::Ace {

class JsonComponentFactory : public ComponentFactory {
public:
    virtual RefPtr<Component> CreateComponent(const JsonValue& jsonValue) const = 0;
    virtual std::list<RefPtr<Component>> CreateChildComponent(const JsonValue& jsonValue) const = 0;
    virtual std::list<RefPtr<Component>> CreateComponents(const JsonValue& jsonValue) const = 0;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_JSON_COMPONENT_FACTORY_H
