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

#include "core/components/test/unittest/mock/tween_mock.h"

namespace OHOS::Ace {
namespace {

MockTweenComponent::HookTweenElement g_hookTweenElement;

} // namespace

RefPtr<Element> MockTweenComponent::CreateElement()
{
    LOGD("MockTweenComponent CreateElement");
    auto tweenElement = AceType::MakeRefPtr<TweenElement>(GetId());
    if (g_hookTweenElement) {
        LOGD("execute hook.");
        g_hookTweenElement(tweenElement);
    }
    return tweenElement;
}

void MockTweenComponent::SetMockHook(const HookTweenElement& hook)
{
    LOGD("set hook.");
    g_hookTweenElement = hook;
}

} // namespace OHOS::Ace