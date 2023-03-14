/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "bundle_monitor.h"

namespace OHOS {
namespace AppExecFwk {
BundleMonitor::BundleMonitor(const EventFwk::CommonEventSubscribeInfo &subscribeInfo)
    : EventFwk::CommonEventSubscriber(subscribeInfo)
{
    APP_LOGI("BundleMonitor constructor with subscribeInfo");
}

bool BundleMonitor::Subscribe(const sptr<IBundleStatusCallback> &callback)
{
    APP_LOGI("Subscribe called");
    callback_ = callback;

    if (EventFwk::CommonEventManager::SubscribeCommonEvent(shared_from_this()) != true) {
        APP_LOGE("SubscribeCommonEvent occur exception.");
        return false;
    }
    return true;
}

bool BundleMonitor::UnSubscribe()
{
    APP_LOGI("unsubscribe called");
    if (EventFwk::CommonEventManager::UnSubscribeCommonEvent(shared_from_this()) != true) {
        APP_LOGE("UnsubscribeCommonEvent occur exception.");
        return false;
    }
    callback_ = nullptr;
    return true;
}

void BundleMonitor::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    APP_LOGI("OnReceiveEvent common event onReceiveEvent called");
    Want want = eventData.GetWant();
    std::string action = want.GetAction();
    std::string bundleName = want.GetElement().GetBundleName();
    int userId = want.GetIntParam(Constants::USER_ID, Constants::INVALID_USERID);
    APP_LOGI("OnReceiveEvent action = %{public}s, bundle = %{public}s, userId = %{public}d",
        action.c_str(), bundleName.c_str(), userId);
    if ((action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) && (callback_ != nullptr)) {
        callback_->OnBundleAdded(bundleName, userId);
    } else if ((action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) && (callback_ != nullptr)) {
        callback_->OnBundleUpdated(bundleName, userId);
    } else if ((action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) && (callback_ != nullptr)) {
        callback_->OnBundleRemoved(bundleName, userId);
    } else {
        APP_LOGI("OnReceiveEvent action = %{public}s not support", action.c_str());
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS