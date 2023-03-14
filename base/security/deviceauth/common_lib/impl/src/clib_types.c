/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "clib_types.h"
#include "securec.h"

void *ClibMalloc(uint32_t size, char val)
{
    if (size == 0 || size > CLIB_MAX_MALLOC_SIZE) {
        return NULL;
    }
    void *addr = malloc(size);
    if (addr != NULL) {
        (void)memset_s(addr, size, val, size);
    }
    return addr;
}

void ClibFree(void *addr)
{
    if (addr != NULL) {
        free(addr);
    }
}