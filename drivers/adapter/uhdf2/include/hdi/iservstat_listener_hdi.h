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
#ifndef HDI_SERVICE_STATUS_LISTENER_INF_H
#define HDI_SERVICE_STATUS_LISTENER_INF_H

#include <refbase.h>
#include <iremote_broker.h>
#include <iremote_stub.h>

namespace OHOS {
namespace HDI {
namespace ServiceManager {
namespace V1_0 {
enum ServiceStatusType {
    SERVIE_STATUS_START,
    SERVIE_STATUS_CHANGE,
    SERVIE_STATUS_STOP,
    SERVIE_STATUS_MAX,
};

struct ServiceStatus {
    std::string serviceName;
    uint16_t deviceClass;   /* ref to DeviceClass */
    uint16_t status;        /* ref to ServiceStatusType */
    std::string info;
};

class IServStatListener : public ::OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"HDI.IServiceStatusListener.V1_0");
    virtual void OnReceive(const ServiceStatus &status) = 0;
};

class ServStatListenerStub : public ::OHOS::IRemoteStub<IServStatListener> {
public:
    ServStatListenerStub() = default;
    virtual ~ServStatListenerStub() {}

    int OnRemoteRequest(uint32_t code,
        ::OHOS::MessageParcel &data, ::OHOS::MessageParcel &reply, ::OHOS::MessageOption &option) override;
private:
    int32_t ServStatListenerStubOnReceive(::OHOS::MessageParcel& data,
        ::OHOS::MessageParcel& reply, ::OHOS::MessageOption& option);
};
} // namespace V1_0
} // namespace ServiceManager
} // namespace HDI
} // namespace OHOS

#endif /* HDI_SERVICE_STATUS_LISTENER_INF_H */