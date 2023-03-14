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

#include <hdf_service_status.h>
#include "hdf_base.h"
#include "hdf_log.h"
#include "iservmgr_hdi.h"
#define HDF_LOG_TAG servstat_listener
namespace OHOS {
namespace HDI {
namespace ServiceManager {
namespace V1_0 {
int ServStatListenerStub::OnRemoteRequest(uint32_t code,
    OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    switch (code) {
        case SERVIE_STATUS_LISTENER_NOTIFY:
            return ServStatListenerStubOnReceive(data, reply, option);
        default:
            break;
    }

    return HDF_ERR_NOT_SUPPORT;
}

int32_t ServStatListenerStub::ServStatListenerStubOnReceive(MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    ServiceStatus status;
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        HDF_LOGI("failed to check interface token");
        return HDF_FAILURE;
    }

    const char* name = data.ReadCString();
    status.serviceName = (name == nullptr) ? "" : name;
    if (status.serviceName.empty()) {
        HDF_LOGI("failed to read serviceName in ServiceStatus");
        return HDF_FAILURE;
    }

    if (!data.ReadUint16(status.deviceClass) ||
        !data.ReadUint16(status.status)) {
        HDF_LOGI("failed to read deviceClass or status in ServiceStatus");
        return HDF_FAILURE;
    }

    const char* info = data.ReadCString();
    status.info = (info == nullptr) ? "" : info;

    OnReceive(status);
    return HDF_SUCCESS;
}
} // namespace V1_0
} // namespace ServiceManager
} // namespace HDI
} // namespace OHOS