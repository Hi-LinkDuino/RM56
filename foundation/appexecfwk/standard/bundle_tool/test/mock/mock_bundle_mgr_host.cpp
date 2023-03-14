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

#include "mock_bundle_mgr_host.h"

using namespace OHOS::AAFwk;
namespace OHOS {
namespace AppExecFwk {
bool MockBundleMgrHost::DumpInfos(
    const DumpFlag flag, const std::string &bundleName, int32_t userId, std::string &result)
{
    APP_LOGD("enter");
    APP_LOGD("flag: %{public}d", flag);
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    if (bundleName.size() > 0) {
        result = bundleName + "\n";
    } else {
        result = "OK";
    }
    return true;
}

bool MockBundleMgrHost::CleanBundleCacheFiles(const std::string &bundleName,
    const sptr<ICleanCacheCallback> &cleanCacheCallback, int32_t userId)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    cleanCacheCallback->OnCleanCacheFinished(0);
    return true;
}

bool MockBundleMgrHost::CleanBundleDataFiles(const std::string &bundleName, const int userId)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    return true;
}

bool MockBundleMgrHost::SetApplicationEnabled(const std::string &bundleName, bool isEnable, int32_t userId)
{
    APP_LOGD("enter");
    APP_LOGD("bundleName: %{public}s", bundleName.c_str());
    APP_LOGD("isEnable: %{public}d", isEnable);
    return true;
}

bool MockBundleMgrHost::SetAbilityEnabled(const AbilityInfo &abilityInfo, bool isEnable, int32_t userId)
{
    APP_LOGD("enter");
    APP_LOGD("abilityName: %{public}s", abilityInfo.name.c_str());
    APP_LOGD("isEnable: %{public}d", isEnable);
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS