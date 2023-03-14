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
#include "semaphore_ex.h"

#include <pthread.h>   // pthread_mutex_lock
#include <sys/types.h> // pid_t
#include <unistd.h>    // getpid
#include <fcntl.h>     // O_CREAT
#include <semaphore.h> // sem_t

#include <sstream>     // ostringstream
#include <iomanip>     // setw/setfill

using namespace std;

namespace OHOS {

std::string GenUniqueName()
{
    static const uint64_t magic = 0xbcdecdeffedcedcbULL;
    static uint64_t count = 0;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&mutex);
    count++;
    std::ostringstream fmt;
    static const int outWidth = 20;
    fmt << "_unnamed_sema_" << getpid() << "_" << magic << "_"
        << std::setw(outWidth) << std::setfill('0') << count++;
    pthread_mutex_unlock(&mutex);
    return fmt.str();
}

NamedSemaphore::NamedSemaphore(size_t size)
    :name_(GenUniqueName()), maxCount_(size), sema_(nullptr), named_(false)
{
}

NamedSemaphore::NamedSemaphore(const std::string& name, size_t size)
    :name_(name), maxCount_(size), sema_(nullptr), named_(true)
{
}

NamedSemaphore::~NamedSemaphore()
{
    if (!named_) {
        Unlink();
    }
}

bool NamedSemaphore::Create()
{
    Unlink();
    sema_ = sem_open(name_.c_str(), O_CREAT,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, maxCount_); // mode: 0666
    return (sema_ != SEM_FAILED);
}

bool NamedSemaphore::Unlink()
{
    return sem_unlink(name_.c_str()) == 0;
}

bool NamedSemaphore::Open()
{
    sema_ = sem_open(name_.c_str(), 0);
    return (sema_ != SEM_FAILED);
}

bool NamedSemaphore::Close()
{
    if (sema_ == nullptr) {
        return false;
    }

    if (sem_close(reinterpret_cast<sem_t *>(sema_)) == 0) {
        sema_ = nullptr;
        return true;
    }
    return false;
}

bool NamedSemaphore::Wait()
{
    if (sema_ == nullptr) {
        return false;
    }

    return (sem_wait(reinterpret_cast<sem_t *>(sema_)) == 0);
}

bool NamedSemaphore::TimedWait(const struct timespec& ts)
{
    if (sema_ == nullptr) {
        return false;
    }

    return (sem_timedwait(reinterpret_cast<sem_t *>(sema_), &ts) == 0);
}

bool NamedSemaphore::TryWait()
{
    if (sema_ == nullptr) {
        return false;
    }

    return (sem_trywait(reinterpret_cast<sem_t *>(sema_)) == 0);

}

bool NamedSemaphore::Post()
{
    if ((sema_ == nullptr) || (GetValue() >= maxCount_)) {
        return false;
    }

    return (sem_post(reinterpret_cast<sem_t *>(sema_)) == 0);
}

int NamedSemaphore::GetValue() const
{
    if (sema_ == nullptr) {
        return INVALID_SEMA_VALUE;
    }

    int val = -1;
    if (sem_getvalue(reinterpret_cast<sem_t *>(sema_), &val) == 0) {
        return val;
    }
    return INVALID_SEMA_VALUE;
}

void Semaphore::Wait()
{
    std::unique_lock<std::mutex> lck(mutex_);
    if (--count_ < 0) {
        cv_.wait(lck);
    }
}

void Semaphore::Post()
{
    std::unique_lock<mutex> lck(mutex_);
    if (++count_ <= 0) {
        cv_.notify_one();
    }
}

} // namespace OHOS
