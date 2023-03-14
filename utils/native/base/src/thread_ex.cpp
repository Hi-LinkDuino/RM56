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

#include "thread_ex.h"
#include <sys/resource.h>
#include <sys/prctl.h>
#include <iostream>
#include "utils_log.h"

namespace OHOS {
using ThreadFunc = int (*)(void*);
using PThreadRoutine = void* (*) (void*);

struct ThreadParam {
    ThreadFunc startRoutine;
    void* args;
    int priority;
    std::string name;

    // prctl only support set the name of the calling process.
    static int proxy(const ThreadParam* t)
    {
        if (t == nullptr) {
            UTILS_LOGE("invalid param.");
            return -1;
        }
        ThreadFunc f = t->startRoutine;
        void* userData = t->args;
        int prio = t->priority;
        std::string threadName = t->name;

        delete t;
        t = nullptr;

        // set thread priority
        (void)setpriority(PRIO_PROCESS, 0, prio);

        // set thread name
        if (!threadName.empty()) {
            prctl(PR_SET_NAME, threadName.substr(0, MAX_THREAD_NAME_LEN).c_str(), 0, 0, 0);
        }

        return f(userData);
    }
};

bool CreatePThread(ThreadParam& para, size_t stackSize, pthread_t *threadId)

{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // create thread as "detached", so it cleans up after itself.
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    auto t = new ThreadParam; // t would be delete in ThreadParam::proxy
    t->startRoutine = para.startRoutine;
    t->args = para.args;
    t->priority = para.priority;
    t->name = para.name;

    para.startRoutine = (ThreadFunc)&ThreadParam::proxy;
    para.args = t;

    if (stackSize) {
        pthread_attr_setstacksize(&attr, stackSize);
    }

    errno = 0;
    pthread_t thread;
    int result = pthread_create(&thread, &attr, (PThreadRoutine)para.startRoutine, para.args);
    pthread_attr_destroy(&attr);

    if (result != 0) {
        return false;
    }

    if (threadId != NULL) {
        *threadId = thread;
    }

    return true;
}

Thread::Thread()
    : thread_(INVALID_PTHREAD_T), status_(ThreadStatus::OK), exitPending_(false), running_(false)
{
}

Thread::~Thread()
{
}

ThreadStatus Thread::Start(const std::string& name, int32_t priority, size_t stack)
{
    std::lock_guard<std::mutex> lk(lock_);
    if (running_) {
        // already started
        return ThreadStatus::INVALID_OPERATION;
    }

    status_ = ThreadStatus::OK;
    exitPending_ = false;
    thread_ = INVALID_PTHREAD_T;
    running_ = true;

    ThreadParam para;
    para.startRoutine = ThreadStart;
    para.args = this;
    para.name = name;
    para.priority = priority;

    bool res = CreatePThread(para, stack, &thread_);
    if (!res) {
        status_ = ThreadStatus::UNKNOWN_ERROR;   // something happened!
        running_ = false;
        thread_ = INVALID_PTHREAD_T;
        return ThreadStatus::UNKNOWN_ERROR;
    }

    return ThreadStatus::OK;
}

ThreadStatus Thread::Join()
{
    // If the two thread IDs are equal, pthread_equal() returns a non-zero value; otherwise, it returns 0.
    if (pthread_equal(thread_, pthread_self()) != 0) {
        return ThreadStatus::WOULD_BLOCK;
    }

    std::unique_lock<std::mutex> lk(lock_);
    while (running_) {
        cvThreadExited_.wait(lk);
    }

    return status_;
}

ThreadStatus Thread::NotifyExitSync()
{
    // If the two thread IDs are equal, pthread_equal() returns a non-zero value; otherwise, it returns 0.
    if (pthread_equal(thread_, pthread_self()) != 0) {
        // don't call NotifyExitSync() from this !;
        return ThreadStatus::WOULD_BLOCK;
    }

    std::unique_lock<std::mutex> lk(lock_);
    exitPending_ = true;

    while (running_ == true) {
        cvThreadExited_.wait(lk);
    }

    exitPending_ = false;

    return status_;
}

void Thread::NotifyExitAsync()
{
    std::lock_guard<std::mutex> lk(lock_);
    exitPending_ = true;
}

bool Thread::ReadyToWork()
{
    return true;
}

bool Thread::IsExitPending() const
{
    std::lock_guard<std::mutex> lk(lock_);
    return exitPending_;
}

bool Thread::IsRunning() const
{
    std::lock_guard<std::mutex> lk(lock_);
    return running_;
}

int Thread::ThreadStart(void* args)
{
    Thread* const self = static_cast<Thread*>(args);
    bool first = true;

    do {
        bool result = false;
        if (first) {
            first = false;
            if (self->ReadyToWork() && !self->IsExitPending()) {
                result = self->Run();
            }
        } else {
            result = self->Run();
        }

        {
            std::unique_lock<std::mutex> lk(self->lock_);
            if ((!result) || self->exitPending_) {
                self->exitPending_ = true;
                self->running_ = false;
                self->thread_ = INVALID_PTHREAD_T;
                self->cvThreadExited_.notify_all();
                break;
            }
        }
    } while (true);

    return 0;
}

} // namespace OHOS
