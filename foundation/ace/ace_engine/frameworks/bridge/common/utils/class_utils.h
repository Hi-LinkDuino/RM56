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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_CLASS_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_CLASS_UTILS_H

#define DEFINE_HAS_METHOD(method)                                                             \
    template<typename ToCheck>                                                                \
    class internal_has_##method {                                                             \
        template<typename ClassTest>                                                          \
        static std::true_type checkHas(decltype(&ClassTest::method));                         \
        template<typename ClassTest>                                                          \
        static std::false_type checkHas(...);                                                 \
                                                                                              \
    public:                                                                                   \
        enum { value = std::is_same<decltype(checkHas<ToCheck>(0)), std::true_type>::value }; \
    };                                                                                        \
                                                                                              \
    template<typename ToCheck>                                                                \
    static inline constexpr const bool has_##method = internal_has_##method<ToCheck>::value;

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_COMMON_UTILS_CLASS_UTILS_H