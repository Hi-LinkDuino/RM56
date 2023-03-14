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

#ifndef HDF_REMOTE_ADPATER_H
#define HDF_REMOTE_ADPATER_H

#include <ipc_object_stub.h>
#include <parcel.h>
#include <string>

#include "hdf_remote_adapter_if.h"
#include "hdf_remote_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

class HdfRemoteServiceStub : public OHOS::IPCObjectStub {
public:
    explicit HdfRemoteServiceStub(struct HdfRemoteService *service);
    int OnRemoteRequest(uint32_t code,
        OHOS::MessageParcel &data, OHOS::MessageParcel &reply, OHOS::MessageOption &option) override;
    ~HdfRemoteServiceStub();
private:
    struct HdfRemoteService *service_;
};

class HdfDeathNotifier : public OHOS::IRemoteObject::DeathRecipient {
public:
    HdfDeathNotifier(struct HdfRemoteService *service, struct HdfDeathRecipient *recipient);
    virtual ~HdfDeathNotifier();
    void OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &object)  override;

private:
    struct HdfDeathRecipient *recipient_;
    struct HdfRemoteService *service_;
};

struct HdfRemoteServiceHolder {
    HdfRemoteServiceHolder();
    ~HdfRemoteServiceHolder() = default;

    bool SetInterfaceDescriptor(const char *desc);
    struct HdfRemoteService service_;
    OHOS::sptr<OHOS::IRemoteObject> remote_;
    OHOS::sptr<OHOS::IRemoteObject::DeathRecipient> deathRecipient_;
};

struct HdfRemoteService *HdfRemoteAdapterBind(OHOS::sptr<OHOS::IRemoteObject> binder);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* HDF_REMOTE_ADPATER_H */
