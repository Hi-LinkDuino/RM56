/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dm_device_info_manager.h"

#include "dm_constants.h"
#include "dm_log.h"

namespace OHOS {
namespace DistributedHardware {
DmDeviceInfoManager::DmDeviceInfoManager(std::shared_ptr<SoftbusConnector> &softbusConnectorPtr)
    : softbusConnector_(softbusConnectorPtr)
{
    LOGI("DmDeviceInfoManager constructor");
    decisionSoName_ = "libdevicemanagerext_decision.z.so";
}

int32_t DmDeviceInfoManager::GetTrustedDeviceList(const std::string &pkgName, const std::string &extra,
                                                  std::vector<DmDeviceInfo> &deviceList)
{
    int32_t ret = softbusConnector_->GetTrustedDeviceList(deviceList);
    if (ret != DM_OK) {
        LOGE("GetTrustedDeviceList failed");
        return ret;
    }

    if (!extra.empty() && !deviceList.empty()) {
        DmAdapterManager &adapterMgrPtr = DmAdapterManager::GetInstance();
        std::shared_ptr<IDecisionAdapter> decisionAdapter = adapterMgrPtr.GetDecisionAdapter(decisionSoName_);
        if (decisionAdapter != nullptr) {
            decisionAdapter->FilterDeviceList(deviceList, extra);
        } else {
            LOGE("GetTrustedDeviceList decisionAdapter is nullptr");
        }
    }

    return DM_OK;
}

int32_t DmDeviceInfoManager::GetLocalDeviceInfo(DmDeviceInfo &info)
{
    int32_t ret = softbusConnector_->GetLocalDeviceInfo(info);
    if (ret != DM_OK) {
        LOGE("GetLocalDeviceInfo failed");
        return ret;
    }
    return DM_OK;
}
} // namespace DistributedHardware
} // namespace OHOS
