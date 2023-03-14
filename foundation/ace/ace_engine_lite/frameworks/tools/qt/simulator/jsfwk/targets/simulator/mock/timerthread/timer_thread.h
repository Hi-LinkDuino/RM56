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

#ifndef OHOS_ACELITE_MOCK_TIMER_THREAD_H
#define OHOS_ACELITE_MOCK_TIMER_THREAD_H

#include <QList>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "nativeapi_timer_task.h"

namespace OHOS {
namespace ACELite {
struct TimerInfo {
    bool isPeriodic = false;
    int32_t delay = 0;
    void *userCallback = nullptr;
    void *userData = nullptr;
    timerHandle_t *timerHandle = nullptr;
    int32_t remain = 0;
};

class TimerThread : public QThread {
public:
    TimerThread() {}
    virtual ~TimerThread() {}
    void run() override;
    void Quit();
    int StartTimerTaskInner(bool isPeriodic,
                            const unsigned int delay,
                            void *userCallback,
                            void *userContext,
                            timerHandle_t *timerHandle);
    int StopTimerTaskInner(const timerHandle_t timerHandle);

private:
    void Traverse();
    QList<const TimerInfo *> timerList_;
    QWaitCondition queueCondition_;
    QMutex mutexlock_;
    volatile bool taskQuitFlag = false;
};
} // namespace ACELite
} // namespace OHOS
#endif // OHOS_ACELITE_MOCK_TIMER_THREAD_H
