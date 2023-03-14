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

#ifndef USB_PARAM_H
#define USB_PARAM_H

#include <string>
#include <vector>

namespace OHOS {
namespace USB {
struct UsbDev {
    uint8_t busNum;
    uint8_t devAddr;
};
struct UsbPipe {
    uint8_t interfaceId;
    uint8_t endpointId;
};
struct UsbCtrlTransfer {
    int32_t requestType;
    int32_t requestCmd;
    int32_t value;
    int32_t index;
    int32_t timeout;
};
} // namespace USB
} // namespace OHOS
#endif // USB_PARAM_H
