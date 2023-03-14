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
#ifndef FAULTLOGGER_SERVICE_PROXY_H
#define FAULTLOGGER_SERVICE_PROXY_H

#include "iremote_proxy.h"
#include "nocopyable.h"

#include "ifaultlogger_service.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLoggerServiceProxy : public IRemoteProxy<IFaultLoggerService> {
public:
    explicit FaultLoggerServiceProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IFaultLoggerService>(impl) {}
    ~FaultLoggerServiceProxy() = default;
    DISALLOW_COPY_AND_MOVE(FaultLoggerServiceProxy);

    virtual void AddFaultLog(const FaultLogInfoOhos& info) override;
    virtual sptr<IRemoteObject> QuerySelfFaultLog(int32_t faultType, int32_t maxNum) override;
    virtual void Destroy() override;

private:
    static inline BrokerDelegator<FaultLoggerServiceProxy> delegator_;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // FAULTLOGGER_SERVICE_PROXY_H