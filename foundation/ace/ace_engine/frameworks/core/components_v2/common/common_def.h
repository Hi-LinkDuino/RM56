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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_COMMON_COMMON_DEF_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_COMMON_COMMON_DEF_H

#include <functional>
#include <memory>

#include "base/memory/referenced.h"

#define ACE_DEFINE_COMPONENT_EVENT(name, func)                                                  \
    private:                                                                                    \
        std::unique_ptr<std::function<func>> event##name##_;                                    \
    public:                                                                                     \
        const std::unique_ptr<std::function<func>>& Get##name() const                           \
        {                                                                                       \
            return event##name##_;                                                              \
        }                                                                                       \
        void Set##name(std::function<func>&& event##name)                                       \
        {                                                                                       \
            if (event##name) {                                                                  \
                event##name##_ = std::make_unique<std::function<func>>(std::move(event##name)); \
            } else {                                                                            \
                event##name##_.reset();                                                         \
            }                                                                                   \
        }

#define ACE_DEFINE_COMPONENT_PROP(name, type, ...) \
    private:                                       \
        type prop##name##_ { __VA_ARGS__ };        \
    public:                                        \
        type Get##name() const                     \
        {                                          \
            return prop##name##_;                  \
        }                                          \
        void Set##name(const type& prop##name)     \
        {                                          \
            prop##name##_ = prop##name;            \
        }

namespace OHOS::Ace {
namespace V1 = ::OHOS::Ace;
namespace V2 {

template<class C, class R, class... Args>
R ResumeEventCallback(const RefPtr<C>& component,
    const std::unique_ptr<std::function<R(Args...)>>& (C::*getMethod)() const, R defValue, const Args&... args)
{
    const auto& funcPtr = ((*component).*getMethod)();
    return (funcPtr && (*funcPtr)) ? (*funcPtr)(args...) : defValue;
}

template<class C, class... Args>
void ResumeEventCallback(const RefPtr<C>& component,
    const std::unique_ptr<std::function<void(Args...)>>& (C::*getMethod)() const, const Args&... args)
{
    const auto& funcPtr = ((*component).*getMethod)();
    if (funcPtr && (*funcPtr)) {
        (*funcPtr)(args...);
    }
}

} // namespace V2
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_V2_COMMON_COMMON_DEF_H
