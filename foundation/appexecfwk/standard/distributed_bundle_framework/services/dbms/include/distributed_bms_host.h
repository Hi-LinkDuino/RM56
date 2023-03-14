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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_BMS_HOST_H
#define FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_BMS_HOST_H

#include "distributed_bms_interface.h"
#include "iremote_stub.h"
#include "parcel_macro.h"

namespace OHOS {
namespace AppExecFwk {
class DistributedBmsHost : public IRemoteStub<IDistributedBms> {
public:
    DistributedBmsHost();
    virtual ~DistributedBmsHost() override;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    int HandleGetRemoteAbilityInfo(Parcel &data, Parcel &reply);
    int HandleGetRemoteAbilityInfos(Parcel &data, Parcel &reply);
    int HandleGetAbilityInfo(Parcel &data, Parcel &reply);
    int HandleGetAbilityInfos(Parcel &data, Parcel &reply);
    bool VerifyCallingPermission(const std::string &permissionName);
    template <typename T>
    bool GetParcelableInfos(Parcel &data, std::vector<T> &parcelableInfos);
    template<typename T>
    bool WriteParcelableVector(std::vector<T> &parcelableVector, Parcel &data);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_DBMS_INCLUDE_DISTRIBUTED_BMS_HOST_H