/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "interfaces/hap_verify.h"

#include <mutex>

#include "init/device_type_manager.h"
#include "init/hap_crl_manager.h"
#include "init/trusted_root_ca.h"
#include "init/trusted_source_manager.h"
#include "init/trusted_ticket_manager.h"
#include "verify/hap_verify_v2.h"

namespace OHOS {
namespace Security {
namespace Verify {
static std::mutex g_mtx;
static bool g_isInit = false;

bool HapVerifyInit()
{
    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    TrustedSourceManager& trustedAppSourceManager = TrustedSourceManager::GetInstance();
    HapCrlManager& hapCrlManager = HapCrlManager::GetInstance();
    DeviceTypeManager& deviceTypeManager = DeviceTypeManager::GetInstance();
    TrustedTicketManager& trustedTicketSourceManager = TrustedTicketManager::GetInstance();
    g_mtx.lock();
    g_isInit = rootCertsObj.Init() && trustedAppSourceManager.Init();
    if (!g_isInit) {
        rootCertsObj.Recovery();
        trustedAppSourceManager.Recovery();
    }
    trustedTicketSourceManager.Init();
    hapCrlManager.Init();
    deviceTypeManager.GetDeviceTypeInfo();
    g_mtx.unlock();
    return g_isInit;
}

bool EnableDebugMode()
{
    TrustedRootCa& rootCertsObj = TrustedRootCa::GetInstance();
    TrustedSourceManager& trustedAppSourceManager = TrustedSourceManager::GetInstance();
    g_mtx.lock();
    bool ret = rootCertsObj.EnableDebug() && trustedAppSourceManager.EnableDebug();
    if (!ret) {
        rootCertsObj.DisableDebug();
        trustedAppSourceManager.DisableDebug();
    }
    g_mtx.unlock();
    return ret;
}

int HapVerify(const std::string& filePath, HapVerifyResult& hapVerifyResult)
{
    if (!g_isInit && !HapVerifyInit()) {
        return VERIFY_SOURCE_INIT_FAIL;
    }
    HapVerifyV2 hapVerifyV2;
    return hapVerifyV2.Verify(filePath, hapVerifyResult);
}
} // namespace Verify
} // namespace Security
} // namespace OHOS
