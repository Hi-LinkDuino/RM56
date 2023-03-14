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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PROMISE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PROMISE_H

#include <atomic>
#include <future>

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
class BundlePromise {
public:
    BundlePromise() = default;
    ~BundlePromise() = default;
    /**
     * @brief Notify all tasks has executed finished.
     * @return
     */
    void NotifyAllTasksExecuteFinished()
    {
        std::lock_guard<std::mutex> lock(notifyTaskMutex_);
        if (hasNotified_) {
            APP_LOGE("promise has executed and abort when NotifyAllTasksExecuteFinished.");
            return;
        }

        APP_LOGD("Notify all tasks has executed finished.");
        hasNotified_ = true;
        promise_.set_value();
    }
    /**
     * @brief Wait for all tasks execute.
     * @return
     */
    void WaitForAllTasksExecute()
    {
        std::lock_guard<std::mutex> lock(waitTaskMutex_);
        if (hasWaited_) {
            APP_LOGE("promise has executed and abort when WaitForAllTasksExecute.");
            return;
        }

        APP_LOGD("Wait for all tasks execute.");
        hasWaited_ = true;
        future_.get();
    }
private:
    std::atomic_bool hasNotified_ = false;
    std::atomic_bool hasWaited_ = false;
    std::promise<void> promise_;
    std::future<void> future_ = promise_.get_future();
    std::mutex waitTaskMutex_;
    std::mutex notifyTaskMutex_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_PROMISE_H
