/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "bundlems_slite_client.h"

#include "adapter.h"
#include "appexecfwk_errors.h"
#include "bundle_mgr_service.h"
#include "bundlems_log.h"
#include "cmsis_os2.h"
#include "ohos_mem_pool.h"
#include "samgr_lite.h"
#include "unistd.h"
#include "utils.h"
#include "want_utils.h"

namespace OHOS {
const unsigned int ERROR_SLEEP_TIMES = 300;
const unsigned int RETRY_TIMES = 10;
Bmsbuff *g_bmsbuff = nullptr;

bool BundleMsClient::Initialize() const
{
    if (bmsProxy_ != nullptr) {
        return true;
    }
    int retry = RETRY_TIMES;
    while (retry--) {
        IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(BMS_SERVICE, BMS_SLITE_FEATURE);
        if (iUnknown == nullptr) {
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }

        (void)iUnknown->QueryInterface(iUnknown, DEFAULT_VERSION, (void **)&bmsProxy_);
        if (bmsProxy_ == nullptr) {
            osDelay(ERROR_SLEEP_TIMES); // sleep 300ms
            continue;
        }

        return true;
    }

    return false;
}

bool BundleMsClient::Install(const char *hapPath, const InstallParam *installParam,
    InstallerCallback installerCallback) const
{
    if (g_bmsbuff == nullptr) {
        g_bmsbuff = reinterpret_cast<Bmsbuff *>(OhosMalloc(MEM_TYPE_APPFMK_LSRAM, sizeof(Bmsbuff)));
    }
    BundleMgrService *service = BundleMgrService::GetInstance();
    if (service == nullptr) {
        return false;
    }
    if (hapPath == nullptr) {
        return false;
    }

    if (memset_s(g_bmsbuff->bundleParameter, MAX_PATH_LEN, 0, MAX_PATH_LEN) != 0) {
        return false;
    }
    int len = strlen(hapPath);
    if (len >= MAX_PATH_LEN) {
        return false;
    }
    if (memcpy_s(g_bmsbuff->bundleParameter, MAX_PATH_LEN, hapPath, len + 1) != 0) {
        return false;
    }
    g_bmsbuff->bundleInstallerCallback = installerCallback;
    Request request = {
        .msgId = BMS_INSTALL_MSG,
        .len = 0,
        .data = nullptr,
        .msgValue = 0,
    };

    int32_t ret = SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
    return ret == ERR_OK;
}

bool BundleMsClient::Uninstall (const char *bundleName, const InstallParam *installParam,
    InstallerCallback installerCallback) const
{
    BundleMgrService *service = BundleMgrService::GetInstance();
    if (service == nullptr) {
        return false;
    }
    if (bundleName == nullptr) {
        return false;
    }
    if (g_bmsbuff == nullptr) {
        g_bmsbuff = reinterpret_cast<Bmsbuff *>(OhosMalloc(MEM_TYPE_APPFMK_LSRAM, sizeof(Bmsbuff)));
    }
    if (memset_s(g_bmsbuff->bundleParameter, MAX_PATH_LEN, 0, MAX_PATH_LEN) != 0) {
        return false;
    }
    int len = strlen(bundleName);
    if (len >= MAX_PATH_LEN) {
        return false;
    }
    if (memcpy_s(g_bmsbuff->bundleParameter, MAX_PATH_LEN, bundleName, len + 1) != 0) {
        return false;
    }
    g_bmsbuff->bundleInstallerCallback = installerCallback;
    Request request = {
        .msgId = BMS_UNINSTALL_MSG,
        .len = 0,
        .data = nullptr,
        .msgValue = 0,
    };
    int32_t ret = SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
    return ret == ERR_OK;
}

bool BundleMsClient::RegisterInstallerCallback (InstallerCallback installerCallback) const
{
    BundleMgrService *service = BundleMgrService::GetInstance();
    if (service == nullptr) {
        return false;
    }
    if (installerCallback == nullptr) {
        return false;
    }
    if (g_bmsbuff == nullptr) {
        g_bmsbuff = reinterpret_cast<Bmsbuff *>(OhosMalloc(MEM_TYPE_APPFMK_LSRAM, sizeof(Bmsbuff)));
    }
    g_bmsbuff->bundleInstallerCallback = installerCallback;
    Request request = {
        .msgId = BMS_REGISTER_CALLBACK_MSG,
        .len = 0,
        .data = nullptr,
        .msgValue = 0,
    };
    int32_t ret = SAMGR_SendRequest(service->GetIdentity(), &request, nullptr);
    return ret == ERR_OK;
}

uint8_t BundleMsClient::QueryAbilityInfo (const Want *want, AbilityInfo *abilityInfo) const
{
    if (!Initialize()) {
        return -1;
    }
    return bmsProxy_->QueryAbilityInfo(want, abilityInfo);
}

uint8_t BundleMsClient::GetBundleInfo (const char *bundleName, int32_t flags, BundleInfo *bundleInfo) const
{
    if (!Initialize()) {
        return -1;
    }
    return bmsProxy_->GetBundleInfo(bundleName, flags, bundleInfo);
}

uint8_t BundleMsClient::GetBundleInfos (int32_t flags, BundleInfo **bundleInfos, int32_t *len) const
{
    if (!Initialize()) {
        return -1;
    }
    return bmsProxy_->GetBundleInfos(flags, bundleInfos, len);
}

bool BundleMsClient::GetInstallState (const char *bundleName, InstallState *installState, uint8_t *installProcess) const
{
    if (!Initialize()) {
        return false;
    }
    return bmsProxy_->GetInstallState(bundleName, installState, installProcess);
}

uint32_t BundleMsClient::GetBundleSize (const char *bundleName) const
{
    if (!Initialize()) {
        return -1;
    }
    return bmsProxy_->GetBundleSize(bundleName);
}

void BundleMsClient::UpdateBundleInfoList () const
{
    if (!Initialize()) {
        return;
    }
    bmsProxy_->UpdateBundleInfoList();
}

uint8_t BundleMsClient::GetBundleInfosNoReplication (const int flags, BundleInfo **bundleInfos, int32_t *len) const
{
    if (!Initialize()) {
        return -1;
    }
    return bmsProxy_->GetBundleInfosNoReplication(flags, bundleInfos, len);
}
} //  namespace OHOS
