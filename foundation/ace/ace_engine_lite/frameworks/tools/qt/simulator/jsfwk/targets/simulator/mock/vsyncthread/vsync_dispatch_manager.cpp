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
#include "vsync_dispatch_manager.h"

#include "js_async_work.h"
#include "q_auto_lock_guard.h"

namespace OHOS {
namespace ACELite {
VsyncDispatchManager &VsyncDispatchManager::GetInstance()
{
    static VsyncDispatchManager vsyncDispatchManager;
    return vsyncDispatchManager;
}

void VsyncDispatchManager::RegisterVsyncReceiver(JSThread *app)
{
    QAutoLockGuard lockGuard(listLock_);
    appList_.append(app);
}

void VsyncDispatchManager::UnregisterVsyncReceiver(JSThread *app)
{
    QAutoLockGuard lockGuard(listLock_);
    appList_.removeOne(app);
}

TEDispatchingResult VsyncDispatchManager::DispatchVsyncEvent()
{
    TEDispatchingResult result = TEDispatchingResult::REFUSED;
    QAutoLockGuard lockGuard(listLock_);
    if (!appList_.isEmpty()) {
        for (int i = 0; i < appList_.size(); i++) {
            JSThread *app = appList_.at(i);
            if (app != nullptr) {
                result = app->TryToDispatchTEEvent();
                if (result == TEDispatchingResult::ACCEPTED) {
                    break;
                }
            }
        }
    }
    return result;
}
} // namespace ACELite
} // namespace OHOS
