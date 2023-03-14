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


#include "core/components/test/unittest/mock/transition_property_mock.h"

namespace OHOS::Ace {
namespace {

MockTransitionPropertyComponent::HookTransitionElement g_hookTransitionElement;

} // namespace

RefPtr<Element> MockTransitionPropertyComponent::CreateElement()
{
    LOGD("MockTransitionComponent CreateElement");
    auto transitionElement = AceType::MakeRefPtr<TransitionElement>(GetId());
    if (g_hookTransitionElement) {
        g_hookTransitionElement(transitionElement);
    }
    return transitionElement;
}

void MockTransitionPropertyComponent::SetMockHook(const HookTransitionElement& hook)
{
    LOGD("set hook.");
    g_hookTransitionElement = hook;
}

} // namespace OHOS::Ace
