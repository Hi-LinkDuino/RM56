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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_MEMORY_MONITOR_DEF_H
#define FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_MEMORY_MONITOR_DEF_H

#include <string>

#include "base/utils/macros.h"

#ifdef ACE_DEBUG
#define ACE_MEMORY_MONITOR
#endif

#ifdef ACE_MEMORY_MONITOR
#define VERIFY_CLASSNAME(classname, p) \
    VerifyClass<std::is_same_v<classname, std::remove_cv_t<std::remove_pointer_t<decltype(p)>>>>::Success()

#define VERIFY_DECLARED_CLASS(classname) \
    VerifyClass<std::is_same_v<classname, typename classname::SelfType>>::Success()

#define DECLARE_CLASS_TYPE_SIZE(classname)                              \
    typedef classname SelfType;                                         \
    static size_t TypeSize()                                            \
    {                                                                   \
        return sizeof(classname);                                       \
    }

#define DECLARE_CLASS_GET_TYPE_SIZE(classname)                          \
    size_t GetTypeSize() const override                                 \
    {                                                                   \
        VERIFY_CLASSNAME(classname, this);                              \
        return TypeSize();                                              \
    }

namespace OHOS::Ace {

template<bool>
struct VerifyClass;

template<>
struct VerifyClass<true> {
    static void Success() {}
};

} // namespace OHOS::Ace

#else // ACE_MEMORY_MONITOR
#define DECLARE_CLASS_TYPE_SIZE(classname)
#define DECLARE_CLASS_GET_TYPE_SIZE(classname)
#define VERIFY_DECLARED_CLASS(classname)
#endif // ACE_MEMORY_MONITOR

#endif // FOUNDATION_ACE_FRAMEWORKS_BASE_MEMORY_MEMORY_MONITOR_DEF_H
