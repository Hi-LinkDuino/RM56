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

#include "clean_cache_callback_proxy.h"

#include "ipc_types.h"
#include "parcel.h"

#include "app_log_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
CleanCacheCallbackProxy::CleanCacheCallbackProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<ICleanCacheCallback>(object)
{
    APP_LOGI("create clean cache callback proxy instance");
}

CleanCacheCallbackProxy::~CleanCacheCallbackProxy()
{
    APP_LOGI("destroy clean cache callback proxy instance");
}

void CleanCacheCallbackProxy::OnCleanCacheFinished(bool succeeded)
{
    APP_LOGI("clean cache result %{public}d", succeeded);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(CleanCacheCallbackProxy::GetDescriptor())) {
        APP_LOGE("fail to OnCleanCacheFinished due to write MessageParcel fail");
        return;
    }

    if (!data.WriteBool(succeeded)) {
        APP_LOGE("fail to call OnCleanCacheFinished​, for write parcel code failed");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (!remote) {
        APP_LOGE("fail to call OnCleanCacheFinished​, for Remote() is nullptr");
        return;
    }

    int32_t ret = remote->SendRequest(
        static_cast<int32_t>(ICleanCacheCallback::Message::ON_CLEAN_CACHE_CALLBACK), data, reply, option);
    if (ret != NO_ERROR) {
        APP_LOGW("fail to call OnCleanCacheFinished​, for transact is failed, error code is: %{public}d", ret);
    }
}
}  // namespace AppExecFwk
}  // namespace OHOS
