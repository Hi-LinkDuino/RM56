/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef DI_SERVICE_OVER_BREDR_H
#define DI_SERVICE_OVER_BREDR_H

#include <cstdint>
#include "base_def.h"

namespace OHOS {
namespace bluetooth {
class DiServiceOverBredrInterface {
public:
    virtual int RegisterSDP(uint16_t startHandle, uint16_t endHandle) = 0;
    virtual void DeregisterSDP() = 0;

    virtual ~DiServiceOverBredrInterface() {}
};

class DiServiceOverBredr : public DiServiceOverBredrInterface {
public:
    DiServiceOverBredr() {}
    ~DiServiceOverBredr() {}

    int RegisterSDP(uint16_t startHandle, uint16_t endHandle) override;
    void DeregisterSDP() override;

    BT_DISALLOW_COPY_AND_ASSIGN(DiServiceOverBredr);
private:
    uint32_t sdpHandle_ = 0;
    int SdpAddServiceClassIdList() const;
    int SdpAddProtocol(uint16_t startHandle, uint16_t endHandle) const;
    int SdpAddBrowseGroupList() const;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif // DI_SERVICE_OVER_BREDR_H