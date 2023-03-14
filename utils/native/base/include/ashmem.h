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

#ifndef UTILS_BASE_ASHMEM_H
#define UTILS_BASE_ASHMEM_H

#include <stddef.h>
#include <linux/ashmem.h>
#include "refbase.h"
#include "parcel.h"

namespace OHOS {
int AshmemCreate(const char *name, size_t size);
int AshmemSetProt(int fd, int prot);
int AshmemGetSize(int fd);

class Ashmem : public virtual RefBase {
public:
    static sptr<Ashmem> CreateAshmem(const char *name, int32_t size);
    void CloseAshmem();
    bool MapAshmem(int mapType);
    bool MapReadAndWriteAshmem();
    bool MapReadOnlyAshmem();
    void UnmapAshmem();
    bool SetProtection(int protectionType);
    int GetProtection();
    int32_t GetAshmemSize();
    bool WriteToAshmem(const void *data, int32_t size, int32_t offset);
    const void *ReadFromAshmem(int32_t size, int32_t offset);
    Ashmem(int fd, int32_t size);
    ~Ashmem();
    int GetAshmemFd() const
    {
        return memoryFd_;
    };
    
private:
    int memoryFd_;
    int32_t memorySize_;
    int flag_;
    void *startAddr_;
    bool CheckValid(int32_t size, int32_t offset, int cmd);
};
} // namespace OHOS
#endif
