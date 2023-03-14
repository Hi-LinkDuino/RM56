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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MONITOR_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MONITOR_H

#include "bundle_data_mgr.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"
#include "distributed_data_storage.h"

namespace OHOS {
namespace AppExecFwk {
class DistributedMonitor : public EventFwk::CommonEventSubscriber {
public:
    DistributedMonitor(const std::shared_ptr<BundleDataMgr> &bundleDataMgr,
                const EventFwk::CommonEventSubscribeInfo& sp):CommonEventSubscriber(sp)
    {
        bundleDataMgr_ = bundleDataMgr;
    }
    ~DistributedMonitor()
    {
        if (!bundleDataMgr_) {
            bundleDataMgr_.reset();
        }
    }
    void OnReceiveEvent(const EventFwk::CommonEventData &eventData)
    {
        APP_LOGI("OnReceiveEvent");
        auto want = eventData.GetWant();
        std::string action = want.GetAction();
        if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
            int32_t userId = eventData.GetCode();
            APP_LOGI("OnReceiveEvent switched userId:%{public}d", userId);
            std::vector<BundleInfo> bundleInfos;
            bool ret = bundleDataMgr_->GetBundleInfos(BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfos, userId);
            if (ret) {
                DistributedDataStorage::GetInstance()->UpdateDistributedData(bundleInfos);
            } else {
                APP_LOGW("OnReceiveEvent GetBundleInfos failed");
            }
        } else {
            APP_LOGI("OnReceiveEvent action = %{public}s not support", action.c_str());
        }
    }
private:
    std::shared_ptr<BundleDataMgr> bundleDataMgr_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_DISTRIBUTED_MONITOR_H