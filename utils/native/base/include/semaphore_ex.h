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
#ifndef SEMAPHORE_EX_H
#define SEMAPHORE_EX_H

#include "nocopyable.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <time.h> // timespec since c11

namespace OHOS {

const int INVALID_SEMA_VALUE = -1;

class NamedSemaphore : public NoCopyable {
public:
    NamedSemaphore(size_t);
    NamedSemaphore(const std::string&, size_t);
    ~NamedSemaphore();

    bool Create();
    bool Unlink();

    bool Open();
    bool Close();

    bool Wait();
    bool TryWait();
    bool TimedWait(const struct timespec& ts);
    bool Post();

    int GetValue() const;

private:
    std::string name_;
    int maxCount_;
    void* sema_;
    bool named_;
};

class Semaphore : public NoCopyable {
public:
    Semaphore(int value = 1) : count_(value) {}
    ~Semaphore() = default;

    void Wait();
    void Post();

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

} // OHOS

#endif

