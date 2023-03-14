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

#include "vsync_thread.h"

#include <QMutex>

#include "ace_log.h"
#include "common/task_manager.h"
#include "q_auto_lock_guard.h"
#include "vsync_dispatch_manager.h"

namespace OHOS {
namespace ACELite {
static VsyncThread *g_currentVsyncThread = nullptr;

static void RenderTickProcess()
{
    TaskManager::GetInstance()->TaskHandler();
    if (g_currentVsyncThread == nullptr) {
        return;
    }
    g_currentVsyncThread->RenderEndNotifer();
}

static void RenderEnd()
{
    if (g_currentVsyncThread == nullptr) {
        return;
    }
    g_currentVsyncThread->RenderEndNotifer();
}

void VsyncThread::RenderEndNotifer()
{
    QAutoLockGuard lockGuard(renderLock);
    renderTickCount_--;
}

bool VsyncThread::IsRendring()
{
    QAutoLockGuard lockGuard(renderLock);
    return (renderTickCount_ != 0);
}

void VsyncThread::SimuOneTickBreak() const
{
    const uint32_t sleepTime = 16;
    QThread::msleep(sleepTime);
}

TEDispatchingResult VsyncThread::SimuOneTickEvent()
{
    TEDispatchingResult result = VsyncDispatchManager::GetInstance().DispatchVsyncEvent();
    if (result != TEDispatchingResult::ACCEPTED) {
        missingCount_++;
        if (missingCount_ > MAX_MISSING_COUNT) {
            HILOG_ERROR(HILOG_MODULE_ACE, "30 TE ticks dispatching failed");
            missingCount_ = 0;
        }
    }
    return result;
}

bool VsyncThread::CheckRenderingState()
{
    if (IsRendring()) {
        stuckCount_++;
        if (stuckCount_ >= MAX_STUCK_COUNT) {
            RenderEndNotifer();
            HILOG_ERROR(HILOG_MODULE_ACE, "====================================================");
            HILOG_ERROR(HILOG_MODULE_ACE, "==    TE Event Dispatch Stuck, recover forcely    ==");
            HILOG_ERROR(HILOG_MODULE_ACE, "====================================================");
            stuckCount_ = 0;
        }
        return true;
    }
    return false;
}

void VsyncThread::run()
{
    g_currentVsyncThread = this;
    const TEHandlingHooks hooks = {RenderTickProcess, RenderEnd};
    ProductAdapter::RegTEHandlers(hooks);
    taskQuitFlag = false;
    // loop
    while (!taskQuitFlag) {
        // sleep 16ms
        SimuOneTickBreak();

        // if is rendering, skip one tick
        if (CheckRenderingState()) {
            continue;
        }

        // reset stuck count
        stuckCount_ = 0;

        // try to dispatch one tick event
        if (SimuOneTickEvent() == TEDispatchingResult::ACCEPTED) {
            // dispatch successfully, someone has received one tick event, increase the count
            QAutoLockGuard lockGuard(renderLock);
            renderTickCount_++;
        }
    }
}

void VsyncThread::Quit()
{
    taskQuitFlag = true;
}
} // namespace ACELite
} // namespace OHOS
