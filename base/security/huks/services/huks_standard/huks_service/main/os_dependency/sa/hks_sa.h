/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HKS_SA_H
#define HKS_SA_H

#include "iremote_broker.h"
#include "iremote_stub.h"
#include "nocopyable.h"
#include "system_ability.h"

namespace OHOS {
namespace Security {
namespace Hks {
enum ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};
enum ResponseCode {
    HW_NO_ERROR =  0,
    HW_SYSTEM_ERROR = -1,
    HW_PERMISSION_DENIED = -2,
};

constexpr int SA_ID_KEYSTORE_SERVICE = 3510;

class IHksService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.security.hks.service");
};

class HksService : public SystemAbility, public IRemoteStub<IHksService> {
    DECLEAR_SYSTEM_ABILITY(HksService)

public:
    DISALLOW_COPY_AND_MOVE(HksService);
    HksService(int saId, bool runOnCreate);
    virtual ~HksService();

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    static sptr<HksService> GetInstance();

protected:
    void OnStart() override;
    void OnStop() override;

private:
    HksService();
    bool Init();
    bool CanRequest() const;

    bool registerToService_;
    ServiceRunningState runningState_;
    static std::mutex instanceLock;
    static sptr<HksService> instance;
};
} // namespace Hks
} // namespace Security
} // namespace OHOS

#endif // HKS_SA_H