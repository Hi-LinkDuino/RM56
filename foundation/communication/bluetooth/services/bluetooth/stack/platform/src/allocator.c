/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "platform/include/allocator.h"
#include <stdlib.h>
#include <string.h>

#define MEM_ALIGN 16

static inline void *AllocatorMalloc(size_t size);
static inline void *AllocatorCalloc(size_t size);
static inline void AllocatorFree(void *ptr);

const Allocator MEM_MALLOC = {
    .alloc = AllocatorMalloc,
    .free = AllocatorFree,
};
const Allocator MEM_CALLOC = {
    .alloc = AllocatorCalloc,
    .free = AllocatorFree,
};

void *SysMalloc(size_t size)
{
    return malloc(size);
}

void *SysCalloc(size_t size)
{
    return calloc(1, size);
}

void SysFree(void *ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}

static void *AllocatorMalloc(size_t size)
{
    return malloc(size);
}

static void *AllocatorCalloc(size_t size)
{
    return calloc(1, size);
}

static void AllocatorFree(void *ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}