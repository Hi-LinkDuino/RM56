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

#ifndef HDI_ALLOCATOR_SERVICE_IMPL_V1_0_H
#define HDI_ALLOCATOR_SERVICE_IMPL_V1_0_H

#include <iremote_stub.h>
#include <message_option.h>
#include <message_parcel.h>

#include "allocator_service_stub.h"
#include "display_gralloc.h"
#include "idisplay_allocator.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class AllocatorService : public AllocatorServiceStub {
public:
    AllocatorService();
    virtual ~AllocatorService() override;
    int32_t AllocMem(const AllocInfo &info, BufferHandle *&handle) override;

private:
    GrallocFuncs *grallocFuncs_;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_ALLOCATOR_SERVICE_IMPL_V1_0_H