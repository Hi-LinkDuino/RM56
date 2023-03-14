/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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
#include "ace_mem_base.h"

#include <cstdlib>
#include "cache_manager.h"

namespace OHOS {
namespace ACELite {
/**
 * @brief g_customHookSet
 * Record the memory hooks used for ACE, only can be setup once when system start-up.
 * The standard memory allocating methods are used as default.
 */
static ACEMemHooks g_memoryHooks = {malloc, free, calloc};
/**
 * @brief g_customHookSet flag for representing if the customer hooks are set
 */
static bool g_customHookSet = false;

void InitMemHooks(const ACEMemHooks &hooks)
{
    if (g_customHookSet) {
        // only can be configured once
        return;
    }
    if ((hooks.malloc_func == nullptr) || (hooks.free_func == nullptr)) {
        // the malloc and free must be given
        return;
    }
    g_memoryHooks.malloc_func = hooks.malloc_func;
    g_memoryHooks.free_func = hooks.free_func;
    g_memoryHooks.calloc_func = hooks.calloc_func;
    // set the flag
    g_customHookSet = true;
}

void InitCacheBuf(uintptr_t bufAddress, size_t bufSize)
{
    CacheManager::GetInstance().SetupCacheMemInfo(bufAddress, bufSize);
}

void *ace_malloc(size_t size)
{
    return g_memoryHooks.malloc_func(size);
}

void *ace_calloc(size_t num, size_t size)
{
    return (g_memoryHooks.calloc_func != nullptr) ? (g_memoryHooks.calloc_func(num, size)) : nullptr;
}

void ace_free(void *ptr)
{
    g_memoryHooks.free_func(ptr);
}
} // namespace ACELite
} // namespace OHOS
