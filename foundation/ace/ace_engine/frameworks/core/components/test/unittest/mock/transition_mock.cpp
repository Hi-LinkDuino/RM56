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

#include "core/components/test/unittest/mock/transition_mock.h"

namespace OHOS::Ace {
namespace {

MockTransitionElement::HookTransitionElement g_hookTransitionElement;

} // namespace

void MockTransitionElement::SetMockHook(const HookTransitionElement& hook)
{
    LOGD("set hook.");
    g_hookTransitionElement = hook;
}

RefPtr<Element> MockTransitionComponent::CreateElement()
{
    LOGD("MockTransitionComponent CreateElement");
    auto transitionElement = AceType::MakeRefPtr<MockTransitionElement>();
    if (g_hookTransitionElement) {
        LOGD("execute hook.");
        g_hookTransitionElement(transitionElement);
    }
    return transitionElement;
}

} // namespace OHOS::Ace