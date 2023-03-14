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

#include "bundle_mgr_service.h"

#include "bundle_service_interface.h"
#include "bundlems_log.h"
#include "gt_bundle_manager_service.h"
#include "ohos_init.h"
#include "samgr_lite.h"

namespace OHOS {
const int STACK_SIZE = 0x1C00;
const int QUEUE_SIZE = 20;
extern Bmsbuff *g_bmsbuff;

BundleMgrService::BundleMgrService() : Service(), identity_()
{
    this->Service::GetName = BundleMgrService::GetServiceName;
    this->Service::Initialize = BundleMgrService::ServiceInitialize;
    this->Service::MessageHandle = BundleMgrService::ServiceMessageHandle;
    this->Service::GetTaskConfig = BundleMgrService::GetServiceTaskConfig;
}

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    CHECK_NULLPTR_RETURN(sm, "BundleManagerService", "get samgr error");
#ifdef __LITEOS_M__
    sm->RegisterService(BundleMgrService::GetInstance());
#else
    BOOL result = sm->RegisterService(BundleMgrService::GetInstance());
    PRINTI("BundleManagerService", "bms starts %{public}s", result ? "successfully" : "unsuccessfully");
#endif
}
SYSEX_SERVICE_INIT(Init);

const char *BundleMgrService::GetServiceName(Service *service)
{
    (void)service;
    return BMS_SERVICE;
}

BOOL BundleMgrService::ServiceInitialize(Service *service, Identity identity)
{
    if (service == nullptr) {
        return FALSE;
    }
    BundleMgrService *bundleManagerService = static_cast<BundleMgrService *>(service);
    bundleManagerService->identity_ = identity;
    Request request = {
        .msgId = BMS_SCAN_PACKAGE_MSG,
        .len = 0,
        .data = nullptr,
        .msgValue = 0,
    };
    (void) SAMGR_SendRequest(bundleManagerService->GetIdentity(), &request, nullptr);
    return TRUE;
}

Identity *BundleMgrService::GetIdentity()
{
    return &identity_;
}

BOOL BundleMgrService::ServiceMessageHandle(Service *service, Request *request)
{
    if (request == nullptr) {
        return FALSE;
    }
    if (request->msgId == BMS_INSTALL_MSG && g_bmsbuff != nullptr) {
        OHOS::GtManagerService::GetInstance().Install(g_bmsbuff->bundleParameter, nullptr,
            g_bmsbuff->bundleInstallerCallback);
    } else if (request->msgId == BMS_UNINSTALL_MSG && g_bmsbuff != nullptr) {
        OHOS::GtManagerService::GetInstance().Uninstall(g_bmsbuff->bundleParameter, nullptr,
            g_bmsbuff->bundleInstallerCallback);
    } else if (request->msgId == BMS_SCAN_PACKAGE_MSG) {
        OHOS::GtManagerService::GetInstance().ScanPackages();
    } else if (request->msgId == BMS_REGISTER_CALLBACK_MSG) {
        OHOS::GtManagerService::GetInstance().RegisterInstallerCallback(g_bmsbuff->bundleInstallerCallback);
    } else {
        return FALSE;
    }
    return TRUE;
}

TaskConfig BundleMgrService::GetServiceTaskConfig(Service *service)
{
#ifdef __LITEOS_M__
    TaskConfig config = {LEVEL_HIGH, PRI_ABOVE_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
#else
    TaskConfig config = {LEVEL_HIGH, PRI_BELOW_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
#endif
    return config;
}
} // namespace OHOS
