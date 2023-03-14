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

#ifndef HDI_DEVICE_INTERFACE_H
#define HDI_DEVICE_INTERFACE_H
#include <unordered_map>
#include <vector>
#include <memory>
#include "hdi_display.h"

namespace OHOS {
namespace HDI {
namespace DISPLAY {
class HdiDeviceInterface {
public:
    static std::vector<std::shared_ptr<HdiDeviceInterface>> DiscoveryDevice();
    virtual std::unordered_map<uint32_t, std::shared_ptr<HdiDisplay>> DiscoveryDisplay() = 0;
    virtual int32_t Init() = 0;
    virtual void DeInit() = 0;
    virtual ~HdiDeviceInterface() {}
};
} // namespace OHOS
} // namespace HDI
} // namespace DISPLAY

#endif // HDI_DEVICE_INTERFACE_H