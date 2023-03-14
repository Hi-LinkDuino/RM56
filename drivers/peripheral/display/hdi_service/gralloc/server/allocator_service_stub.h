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

#ifndef HDI_ALLOCATOR_SERVICE_STUB_V1_0_H
#define HDI_ALLOCATOR_SERVICE_STUB_V1_0_H

#include "display_gralloc.h"
#include "idisplay_allocator.h"

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
const int32_t CMD_ALLOCATOR_ALLOCMEM = 0;

class AllocatorServiceStub : public IRemoteStub<IDisplayAllocator> {
public:
    AllocatorServiceStub() = default;
    virtual ~AllocatorServiceStub() = default;
    int32_t OnRemoteRequest(uint32_t cmdId, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    int32_t AllocaltorStubAllocMem(MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

void *AllocatorServiceInstance(void);

void AllocatorServiceRelease(void *servObj);

int32_t AllocatorServiceOnRemoteRequest(void *service, int cmdId, struct HdfSBuf &data, struct HdfSBuf &reply);

#endif // HDI_ALLOCATOR_SERVICE_STUB_V1_0_H
