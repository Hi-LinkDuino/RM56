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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_TRANSITION_MOCK_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_TRANSITION_MOCK_H

#include "core/components/page_transition/page_transition_component.h"
#include "core/components/page_transition/page_transition_element.h"

namespace OHOS::Ace {

class MockTransitionElement : public PageTransitionElement {
    DECLARE_ACE_TYPE(MockTransitionElement, PageTransitionElement);

public:
    using HookTransitionElement = std::function<void(const RefPtr<PageTransitionElement>&)>;
    static void SetMockHook(const HookTransitionElement& hook);
};

class MockTransitionComponent : public PageTransitionComponent {
    DECLARE_ACE_TYPE(MockTransitionComponent, PageTransitionComponent);

public:
    RefPtr<Element> CreateElement() override;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_TEST_UNITTEST_MOCK_TRANSITION_MOCK_H
