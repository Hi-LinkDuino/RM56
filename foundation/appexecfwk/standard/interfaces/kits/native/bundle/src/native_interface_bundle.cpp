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

#include "native_interface_bundle.h"

#include <mutex>
#include <string>
#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy_native.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "securec.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
    static constexpr size_t APPID_MAX_LENGTH = 10240;
}
class BundleNative {
public:
    static sptr<IBundleMgr> GetBundleMgr();

private:
    static sptr<IBundleMgr> bundleMgr_;
    static std::mutex bundleMgrMutex_;
};
sptr<IBundleMgr> BundleNative::bundleMgr_ = nullptr;
std::mutex BundleNative::bundleMgrMutex_;

sptr<IBundleMgr> BundleNative::GetBundleMgr()
{
    if (bundleMgr_ == nullptr) {
        std::lock_guard<std::mutex> lock(bundleMgrMutex_);
        if (bundleMgr_ == nullptr) {
            auto systemAbilityManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (systemAbilityManager == nullptr) {
                APP_LOGE("GetBundleMgr GetSystemAbilityManager is null");
                return nullptr;
            }
            auto bundleMgrSa = systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
            if (bundleMgrSa == nullptr) {
                APP_LOGE("GetBundleMgr GetSystemAbility is null");
                return nullptr;
            }
            auto bundleMgr = OHOS::iface_cast<IBundleMgr>(bundleMgrSa);
            if (bundleMgr == nullptr) {
                APP_LOGE("GetBundleMgr iface_cast get null");
            }
            bundleMgr_ = bundleMgr;
        }
    }
    return bundleMgr_;
}
} // AppExecFwk
} // OHOS

char* OH_NativeBundle_GetAppIdByBundleName(const char* bundleName)
{
    if (bundleName == nullptr) {
        APP_LOGE("bundleName is nullptr");
        return nullptr;
    }
    std::string name(bundleName);
    if (name.empty()) {
        APP_LOGE("bundleName is empty");
        return nullptr;
    }
    auto iBundleMgr = OHOS::AppExecFwk::BundleNative::GetBundleMgr();
    if (!iBundleMgr) {
        APP_LOGE("can not get iBundleMgr");
        return nullptr;
    }
    std::string appId = iBundleMgr->GetAppIdByBundleName(name, OHOS::AppExecFwk::Constants::ANY_USERID);
    if (appId.empty()) {
        APP_LOGE("failed due to appId is empty");
        return nullptr;
    }
    size_t len = appId.size();
    if ((len == 0) || (len + 1) > OHOS::AppExecFwk::APPID_MAX_LENGTH) {
        APP_LOGE("failed due to the length of appId is 0 or to long");
        return nullptr;
    }
    char* result = static_cast<char*>(malloc(len + 1));
    if (result == nullptr) {
        APP_LOGE("failed due to malloc error");
        return nullptr;
    }

    if (strcpy_s(result, len + 1, appId.c_str()) != EOK) {
        APP_LOGE("failed due to strcpy_s error");
        free(result);
        result = nullptr;
        return nullptr;
    }
    return result;
}
