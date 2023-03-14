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

#include "kvstore_death_recipient_callback.h"

#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_mgr_service.h"

using namespace OHOS::DistributedKv;

namespace OHOS {
namespace AppExecFwk {
namespace {
const int32_t CHECK_TIMES = 300;
const int32_t CHECK_INTERVAL = 100000;  // 100ms
}  // namespace

KvStoreDeathRecipientCallback::KvStoreDeathRecipientCallback()
{
    APP_LOGI("create kvstore death recipient callback instance %{public}p", this);
}

KvStoreDeathRecipientCallback::~KvStoreDeathRecipientCallback()
{
    APP_LOGI("destroy kvstore death recipient callback instance %{public}p", this);
}

void KvStoreDeathRecipientCallback::OnRemoteDied()
{
    APP_LOGI("OnRemoteDied, register data change listener begin");
    auto dataMgr = DelayedSingleton<BundleMgrService>::GetInstance()->GetDataMgr();
    if (!dataMgr) {
        APP_LOGE("dataMgr is nullptr");
        return;
    }

    auto dataStorage = dataMgr->GetDataStorage();
    if (!dataStorage) {
        APP_LOGE("dataStorage is nullptr");
        return;
    }

    std::thread([dataStorage] {
        int32_t times = 0;
        while (times < CHECK_TIMES) {
            times++;
            // init kvStore.
            if (dataStorage && dataStorage->ResetKvStore()) {
                // register data change listener again.
                APP_LOGI("current times is %{public}d", times);
                break;
            }
            usleep(CHECK_INTERVAL);
        }
    }).detach();

    APP_LOGI("OnRemoteDied, register data change listener end");
}
}  // namespace AppExecFwk
}  // namespace OHOS