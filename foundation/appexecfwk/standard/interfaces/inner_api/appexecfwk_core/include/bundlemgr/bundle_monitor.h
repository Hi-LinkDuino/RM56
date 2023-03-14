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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MONITOR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MONITOR_H

#include "common_event_manager.h"
#include "common_event_support.h"
#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"
#include "bundle_status_callback_interface.h"
#include "bundle_constants.h"
#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMonitor : public EventFwk::CommonEventSubscriber, public std::enable_shared_from_this<BundleMonitor> {
public:
    using Want = OHOS::AAFwk::Want;

    explicit BundleMonitor(const EventFwk::CommonEventSubscribeInfo &subscribeInfo);
    ~BundleMonitor() = default;

    /**
     * @brief Subscribe commonEvent.
     * @param callback The callback of IBundleStatusCallback
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool Subscribe(const sptr<IBundleStatusCallback> &callback);
    /**
     * @brief Unsubscribe commonEvent.
     * @return Returns true if this function is successfully called; returns false otherwise.
     */
    bool UnSubscribe();
    /**
     * @brief CommonEvent callback.
     * @param eventData publish common event data.
     * @return
     */
    void OnReceiveEvent(const EventFwk::CommonEventData &eventData);

private:
    sptr<IBundleStatusCallback> callback_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MONITOR_H