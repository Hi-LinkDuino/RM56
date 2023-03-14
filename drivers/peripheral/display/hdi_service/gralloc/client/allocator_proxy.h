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

#ifndef HDI_ALLOCATOR_PROXY_V1_0_H
#define HDI_ALLOCATOR_PROXY_V1_0_H

#include "idisplay_allocator.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace HDI {
namespace Display {
namespace V1_0 {
class AllocatorProxy : public IRemoteProxy<IDisplayAllocator> {
public:
    explicit AllocatorProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IDisplayAllocator>(impl) {}
    virtual ~AllocatorProxy() = default;
    virtual int32_t AllocMem(const AllocInfo &info, BufferHandle *&handle) override;

private:
    static inline BrokerDelegator<AllocatorProxy> delegator_;
    static constexpr int CMD_REMOTE_ALLOCATOR_ALLOCMEM = 0;
    // static constexpr int CMD_REMOTE_ALLOCATOR_FREE = 1;
    // static constexpr int CMD_REMOTE_ALLOCATOR_MMAP = 2;
    // static constexpr int CMD_REMOTE_ALLOCATOR_UNMAP = 3;
    // static constexpr int CMD_REMOTE_ALLOCATOR_INVALIDDATE = 4;
};
} // namespace V1_0
} // namespace Display
} // namespace HDI
} // namespace OHOS

#endif // HDI_ALLOCATOR_PROXY_V1_0_H

