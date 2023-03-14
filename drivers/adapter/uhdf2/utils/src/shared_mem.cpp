/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "shared_mem.h"
#include <ashmem.h>

int SharedMemCreate(const char *name, size_t size)
{
    return OHOS::AshmemCreate(name, size);
}

int SharedMemSetProt(int fd, int prot)
{
    return OHOS::AshmemSetProt(fd, prot);
}

int SharedMemGetSize(int fd)
{
    return OHOS::AshmemGetSize(fd);
}