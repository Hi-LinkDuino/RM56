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

#include "ashmem.h"

#include <cerrno>
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <linux/ashmem.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include "securec.h"
#include "utils_log.h"

namespace OHOS {
static pthread_mutex_t g_ashmemLock = PTHREAD_MUTEX_INITIALIZER;

using openFdFunction = int (*)();
static openFdFunction g_openFdApi = nullptr;

openFdFunction ProbeAshmemFdFunction()
{
    auto handle = dlopen("libashmemd_client.so", RTLD_NOW);
    if (!handle) {
        return nullptr;
    }
    openFdFunction func = reinterpret_cast<openFdFunction>(dlsym(handle, "openAshmemdFd"));
    if (!func) {
        dlclose(handle);
    }
    return func;
}

static inline void TrimFromStart(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {return !std::isspace(ch);}));
}

static inline void TrimFromEnd(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {return !std::isspace(ch);}).base(), s.end());
}

static void Trim(std::string &s)
{
    TrimFromStart(s);
    TrimFromEnd(s);
}

static int GetAshmemDeviceFd()
{
    static const std::string boot_id_path = "/proc/sys/kernel/random/boot_id";
    std::string boot_id;
    char buf[BUFSIZ];
    ssize_t n;
    int fdBoot = -1;
    int fdAshmem = -1;

    fdBoot = TEMP_FAILURE_RETRY(open(boot_id_path.c_str(), O_RDONLY | O_CLOEXEC));
    if (fdBoot < 0) {
        return -1;
    }

    if ((n = TEMP_FAILURE_RETRY(read(fdBoot, &buf[0], sizeof(buf)))) > 0) {
        boot_id.append(buf, n);
    } else {
        close(fdBoot);
        return -1;
    }
    close(fdBoot);

    Trim(boot_id);

    fdAshmem = TEMP_FAILURE_RETRY(open(("/dev/ashmem" + boot_id).c_str(), O_RDWR | O_CLOEXEC));
    return fdAshmem;
}


static int AshmemOpenLocked()
{
    int fd = -1;

    if (g_openFdApi == nullptr) {
        g_openFdApi = ProbeAshmemFdFunction();
    }

    if (g_openFdApi != nullptr) {
        fd = g_openFdApi();
    } else {
        fd = GetAshmemDeviceFd();
    }

    if (fd < 0) {
        fd = TEMP_FAILURE_RETRY(open("/dev/ashmem", O_RDWR | O_CLOEXEC));
    }

    if (fd < 0) {
        UTILS_LOGE("%{public}s: fd is invalid, fd = %{public}d", __func__, fd);
        return fd;
    }

    struct stat st;
    int ret = TEMP_FAILURE_RETRY(fstat(fd, &st));
    if (ret < 0) {
        UTILS_LOGE("%{public}s: Failed to exec fstat, ret = %{public}d", __func__, ret);
        close(fd);
        return ret;
    }

    if (!S_ISCHR(st.st_mode) || !st.st_rdev) {
        UTILS_LOGE("%{public}s: stat status is invalid, st_mode = %{public}u", __func__, st.st_mode);
        close(fd);
        return -1;
    }
    return fd;
}

static int AshmemOpen()
{
    pthread_mutex_lock(&g_ashmemLock);
    int fd = AshmemOpenLocked();
    pthread_mutex_unlock(&g_ashmemLock);
    return fd;
}

/*
 * AshmemCreate - create a new ashmem region and returns the file descriptor
 * fd < 0 means failed
 *
 */
int AshmemCreate(const char *name, size_t size)
{
    int ret;
    int fd = AshmemOpen();
    if (fd < 0) {
        UTILS_LOGE("%{public}s: Failed to exec AshmemOpen fd = %{public}d", __func__, fd);
        return fd;
    }

    if (name != nullptr) {
        char buf[ASHMEM_NAME_LEN] = {0};
        ret = strcpy_s(buf, sizeof(buf), name);
        if (ret != EOK) {
            UTILS_LOGE("%{public}s: Failed to exec strcpy_s, name= %{public}s, ret= %{public}d", __func__, name, ret);
            close(fd);
            return -1;
        }
        ret = TEMP_FAILURE_RETRY(ioctl(fd, ASHMEM_SET_NAME, buf));
        if (ret < 0) {
            UTILS_LOGE("%{public}s: Failed to set name, name= %{public}s, ret= %{public}d", __func__, name, ret);
            close(fd);
            return ret;
        }
    }

    ret = TEMP_FAILURE_RETRY(ioctl(fd, ASHMEM_SET_SIZE, size));
    if (ret < 0) {
        UTILS_LOGE("%{public}s: Failed to set size, size= %{public}zu", __func__, size);
        close(fd);
        return ret;
    }
    return fd;
}

int AshmemSetProt(int fd, int prot)
{
    return TEMP_FAILURE_RETRY(ioctl(fd, ASHMEM_SET_PROT_MASK, prot));
}

int AshmemGetSize(int fd)
{
    return TEMP_FAILURE_RETRY(ioctl(fd, ASHMEM_GET_SIZE, NULL));
}

Ashmem::Ashmem(int fd, int size) : memoryFd_(fd), memorySize_(size), flag_(0), startAddr_(nullptr)
{
}

Ashmem::~Ashmem()
{
}

sptr<Ashmem> Ashmem::CreateAshmem(const char *name, int32_t size)
{
    if ((name == nullptr) || (size <= 0)) {
        UTILS_LOGE("%{public}s: Parameter is invalid, size= %{public}d", __func__, size);
        return nullptr;
    }

    int fd = AshmemCreate(name, size);
    if (fd < 0) {
        UTILS_LOGE("%{public}s: Failed to exec AshmemCreate, fd= %{public}d", __func__, size);
        return nullptr;
    }

    return new Ashmem(fd, size);
}

void Ashmem::CloseAshmem()
{
    if (memoryFd_ > 0) {
        ::close(memoryFd_);
        memoryFd_ = -1;
    }
    memorySize_ = 0;
    flag_ = 0;
    startAddr_ = nullptr;
}

bool Ashmem::MapAshmem(int mapType)
{
    void *startAddr = ::mmap(nullptr, memorySize_, mapType, MAP_SHARED, memoryFd_, 0);
    if (startAddr == MAP_FAILED) {
        UTILS_LOGE("Failed to exec mmap");
        return false;
    }

    startAddr_ = startAddr;
    flag_ = mapType;

    return true;
}

bool Ashmem::MapReadAndWriteAshmem()
{
    return MapAshmem(PROT_READ | PROT_WRITE);
}

bool Ashmem::MapReadOnlyAshmem()
{
    return MapAshmem(PROT_READ);
}

void Ashmem::UnmapAshmem()
{
    if (startAddr_ != nullptr) {
        ::munmap(startAddr_, memorySize_);
        startAddr_ = nullptr;
    }
    flag_ = 0;
}

bool Ashmem::SetProtection(int protectionType)
{
    int result = AshmemSetProt(memoryFd_, protectionType);
    return result >= 0;
}

int Ashmem::GetProtection()
{
    return TEMP_FAILURE_RETRY(ioctl(memoryFd_, ASHMEM_GET_PROT_MASK));
}

int32_t Ashmem::GetAshmemSize()
{
    return AshmemGetSize(memoryFd_);
}

bool Ashmem::WriteToAshmem(const void *data, int32_t size, int32_t offset)
{
    if (data == nullptr) {
        return false;
    }

    if (!CheckValid(size, offset, PROT_WRITE)) {
        UTILS_LOGE("%{public}s: invalid input or not map", __func__);
        return false;
    }

    auto tmpData = reinterpret_cast<char *>(startAddr_);
    int ret = memcpy_s(tmpData + offset, memorySize_ - offset, reinterpret_cast<const char *>(data), size);
    if (ret != EOK) {
        UTILS_LOGE("%{public}s: Failed to memcpy, ret = %{public}d", __func__, ret);
        return false;
    }

    return true;
}

const void *Ashmem::ReadFromAshmem(int32_t size, int32_t offset)
{
    if (!CheckValid(size, offset, PROT_READ)) {
        UTILS_LOGE("%{public}s: invalid input or not map", __func__);
        return nullptr;
    }

    return reinterpret_cast<const char *>(startAddr_) + offset;
}

bool Ashmem::CheckValid(int32_t size, int32_t offset, int cmd)
{
    if (startAddr_ == nullptr) {
        return false;
    }
    if ((size < 0) || (size > memorySize_) || (offset < 0) || (offset > memorySize_)) {
        UTILS_LOGE("%{public}s: , invalid parameter, size = %{public}d, memorySize_ = %{public}d, offset = %{public}d",
            __func__, size, memorySize_, offset);
        return false;
    }
    if (offset + size > memorySize_) {
        UTILS_LOGE("%{public}s: , invalid parameter, size = %{public}d, memorySize_ = %{public}d, offset = %{public}d",
            __func__, size, memorySize_, offset);
        return false;
    }
    if (!(static_cast<uint32_t>(GetProtection()) & static_cast<uint32_t>(cmd)) ||
        !(static_cast<uint32_t>(flag_) & static_cast<uint32_t>(cmd))) {
        return false;
    }

    return true;
}
}
