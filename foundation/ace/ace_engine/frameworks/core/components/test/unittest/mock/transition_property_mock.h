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

#ifndef FOUNDATION_ACE_CORE_ELEMENTS_TEST_UNITTEST_MOCK_TRANSITION_MOCK_H
#define FOUNDATION_ACE_CORE_ELEMENTS_TEST_UNITTEST_MOCK_TRANSITION_MOCK_H

#include <utility>

#include "core/components/transition/transition_component.h"
#include "core/components/transition/transition_element.h"

namespace OHOS::Ace {

class MockTransitionPropertyComponent : public TransitionComponent {
public:
    MockTransitionPropertyComponent(const ComposeId& id, const std::string& name, const RefPtr<Component>& child)
        : TransitionComponent(id, name, child)
    {}
    ~MockTransitionPropertyComponent() override = default;

    using HookTransitionElement = std::function<void(const RefPtr<TransitionElement>&)>;
    RefPtr<Element> CreateElement() override;

    static void SetMockHook(const HookTransitionElement& hook);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_CORE_ELEMENTS_TEST_UNITTEST_MOCK_TRANSITION_MOCK_H
