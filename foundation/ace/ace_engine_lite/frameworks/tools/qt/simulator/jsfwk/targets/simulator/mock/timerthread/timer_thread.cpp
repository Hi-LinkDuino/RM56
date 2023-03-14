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

#include "timer_thread.h"

#include "q_auto_lock_guard.h"

using namespace OHOS::ACELite;

static TimerThread *g_timerInstance = nullptr;

int InitTimerTask()
{
    // default
    return 0;
}

int StartTimerTask(bool isPeriodic,
                   const unsigned int delay,
                   void *userCallback,
                   void *userData,
                   timerHandle_t *timerHandle)
{
    if (g_timerInstance == nullptr) {
        return -1;
    }

    return g_timerInstance->StartTimerTaskInner(isPeriodic, delay, userCallback, userData, timerHandle);
}

int StopTimerTask(const timerHandle_t timerHandle)
{
    if (g_timerInstance == nullptr) {
        return -1;
    }

    return g_timerInstance->StopTimerTaskInner(timerHandle);
}

namespace OHOS {
namespace ACELite {
const static uint32_t TIMER_STEP = 10;
typedef void (*UserCallbackFunc)(void *data);

void TimerThread::run()
{
    g_timerInstance = this;
    taskQuitFlag = false;
    while (!taskQuitFlag) {
        Traverse();
        QThread::msleep(TIMER_STEP); // calculate once every 10ms
    }
}

void TimerThread::Quit()
{
    taskQuitFlag = true;
}

void TimerThread::Traverse()
{
    QAutoLockGuard lockGuard(mutexlock_);
    QMutableListIterator<const TimerInfo *> listIterator(timerList_);
    while (listIterator.hasNext()) {
        TimerInfo *info = const_cast<TimerInfo *>(listIterator.next());
        if (info->remain > 0) {
            info->remain -= TIMER_STEP;
        } else {
            UserCallbackFunc func = (UserCallbackFunc)(info->userCallback);
            func(info->userData);
            if (info->isPeriodic) {
                info->remain = info->delay;
            } else {
                listIterator.remove();
                delete info;
                info = nullptr;
            }
        }
    }
}

int TimerThread::StartTimerTaskInner(bool isPeriodic,
                                     const unsigned int delay,
                                     void *userCallback,
                                     void *userData,
                                     timerHandle_t *timerHandle)
{
    auto info = new TimerInfo();
    if (info == nullptr) {
        return -1;
    }
    info->delay = delay;
    info->remain = delay;
    info->isPeriodic = isPeriodic;
    info->userCallback = userCallback;
    info->userData = userData;
    *timerHandle = info;
    info->timerHandle = reinterpret_cast<timerHandle_t *>info;
    QAutoLockGuard lockGuard(mutexlock_);
    timerList_.append(info);
    return 0;
}

int TimerThread::StopTimerTaskInner(const timerHandle_t timerHandle)
{
    QAutoLockGuard lockGuard(mutexlock_);
    QMutableListIterator<const TimerInfo *> listIterator(timerList_);
    while (listIterator.hasNext()) {
        const TimerInfo *info = const_cast<TimerInfo *>(listIterator.next());
        if (info == nullptr) {
            continue;
        }
        if (info->timerHandle == timerHandle) {
            listIterator.remove();
            delete info;
            info = nullptr;
        }
    }
    return 0;
}
} // namespace ACELite
} // namespace OHOS
