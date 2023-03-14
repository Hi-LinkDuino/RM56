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
#ifndef FAULTLOGGER_SERVICE_STUB_H
#define FAULTLOGGER_SERVICE_STUB_H

#include "iremote_stub.h"
#include "nocopyable.h"

#include "ifaultlogger_service.h"

namespace OHOS {
namespace HiviewDFX {
class FaultLoggerServiceStub : public IRemoteStub<IFaultLoggerService> {
public:
    FaultLoggerServiceStub() = default;
    virtual ~FaultLoggerServiceStub() = default;
    DISALLOW_COPY_AND_MOVE(FaultLoggerServiceStub);

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // FAULTLOGGER_SERVICE_STUB_H