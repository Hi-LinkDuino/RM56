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

#include <iservice_registry.h>
#include <iremote_stub.h>
#include <hdf_base.h>
#include <hdf_log.h>
#include "iservmgr_hdi.h"

namespace OHOS {
namespace HDI {
namespace ServiceManager {
namespace V1_0 {
constexpr int DEVICE_SERVICE_MANAGER_SA_ID = 5100;
constexpr int DEVSVC_MANAGER_GET_SERVICE = 3;
constexpr int DEVSVC_MANAGER_REGISER_SVCLISTENER = 4;
constexpr int DEVSVC_MANAGER_UNREGISER_SVCLISTENER = 5;

class ServiceManagerProxy : public IRemoteProxy<IServiceManager> {
public:
    explicit ServiceManagerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IServiceManager>(impl) {}
    ~ServiceManagerProxy() {}

    sptr<IRemoteObject> GetService(const char* serviceName) override;
    int32_t RegisterServiceStatusListener(::OHOS::sptr<IServStatListener> listener,
        uint16_t deviceClass) override;
    int32_t UnregisterServiceStatusListener(::OHOS::sptr<IServStatListener> listener) override;
private:
    static inline BrokerDelegator<ServiceManagerProxy> delegator_;
};

sptr<IServiceManager> IServiceManager::Get()
{
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        HDF_LOGE("failed to get sa manager");
        return nullptr;
    }
    sptr<IRemoteObject> remote = saManager->GetSystemAbility(DEVICE_SERVICE_MANAGER_SA_ID);
    if (remote != nullptr) {
        return iface_cast<IServiceManager>(remote);
    }

    HDF_LOGE("failed to get sa hdf service manager");
    return nullptr;
}

int32_t ServiceManagerProxy::RegisterServiceStatusListener(::OHOS::sptr<IServStatListener> listener,
    uint16_t deviceClass)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(ServiceManagerProxy::GetDescriptor()) ||
        !data.WriteUint16(deviceClass) ||
        !data.WriteRemoteObject(listener->AsObject())) {
        return HDF_FAILURE;
    }

    int status = Remote()->SendRequest(DEVSVC_MANAGER_REGISER_SVCLISTENER, data, reply, option);
    if (status) {
        HDF_LOGE("failed to register servstat listener, %{public}d", status);
    }
    return status;
}

int32_t ServiceManagerProxy::UnregisterServiceStatusListener(::OHOS::sptr<IServStatListener> listener)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(ServiceManagerProxy::GetDescriptor()) ||
        !data.WriteRemoteObject(listener->AsObject())) {
        return HDF_FAILURE;
    }

    int status = Remote()->SendRequest(DEVSVC_MANAGER_UNREGISER_SVCLISTENER, data, reply, option);
    if (status) {
        HDF_LOGE("failed to unregister servstat listener, %{public}d", status);
    }
    return status;
}

sptr<IRemoteObject> ServiceManagerProxy::GetService(const char* serviceName)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(ServiceManagerProxy::GetDescriptor()) || !data.WriteCString(serviceName)) {
        return nullptr;
    }

    MessageOption option;
    int status = Remote()->SendRequest(DEVSVC_MANAGER_GET_SERVICE, data, reply, option);
    if (status) {
        HDF_LOGE("get hdi service %{public}s failed, %{public}d", serviceName, status);
        return nullptr;
    }
    HDF_LOGD("get hdi service %{public}s success ", serviceName);
    return reply.ReadRemoteObject();
}
} // namespace V1_0
} // namespace ServiceManager
} // namespace HDI
} // namespace OHOS