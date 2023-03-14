/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BASE_DEF_H
#define BASE_DEF_H

#include <memory>
#include <string>

#if (__cplusplus < 201703L)
#error "CXX Version Error!!!"
#endif

#ifdef DEBUG
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif

#ifndef BT_DISALLOW_COPY
#define BT_DISALLOW_COPY(TypeName) TypeName(const TypeName &) = delete
#endif

#ifndef BT_DISALLOW_ASSIGN
#define BT_DISALLOW_ASSIGN(TypeName) TypeName &operator=(const TypeName &) = delete
#endif

#ifndef BT_DISALLOW_COPY_AND_ASSIGN
#define BT_DISALLOW_COPY_AND_ASSIGN(TypeName) \
    BT_DISALLOW_COPY(TypeName);               \
    BT_DISALLOW_ASSIGN(TypeName)
#endif

#ifndef BT_DISALLOW_MOVE_AND_ASSIGN
#define BT_DISALLOW_MOVE_AND_ASSIGN(TypeName)   \
    TypeName(TypeName &&) noexcept = delete; \
    TypeName &operator=(TypeName &&) noexcept = delete
#endif

#ifndef DECLARE_IMPL
#define DECLARE_IMPL() \
    struct impl;       \
    std::unique_ptr<impl> pimpl
#endif

/**
 * @brief Bluetooth system config path.
 */
const std::string BT_CONFIG_PATH_BASE = "/system/etc/bluetooth/";
const std::string BT_CONFIG_PATH = "/data/service/el1/public/bluetooth/";


#ifndef CHECK_EXCEPT_INTR
#define CHECK_EXCEPT_INTR(fn) \
    do {                      \
    } while ((fn) == -1 && errno == EINTR)
#endif

#endif  // BASE_DEF_H