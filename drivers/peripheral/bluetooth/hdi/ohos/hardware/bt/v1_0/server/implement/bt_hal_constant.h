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

#ifndef BT_HAL_CONSTANT_H
#define BT_HAL_CONSTANT_H

#include <string>

#include <sched.h>

namespace OHOS {
namespace HDI {
namespace BT {
namespace V1_0 {
static const std::string VENDOR_NAME = "libbt_vendor.z.so";
static const std::string VENDOR_INTERFACE_SYMBOL_NAME = "BLUETOOTH_VENDOR_LIB_INTERFACE";

static const std::string BT_DEVICE_ADDRESS_PATH = "/mnt/vendor/btmac.txt";

static const int THREAD_POLICY = SCHED_RR;
static const int THREAD_PRIORITY = 1;
}  // namespace V1_0
}  // namespace BT
}  // namespace HDI
}  // namespace OHOS

#endif /* BT_HAL_CONSTANT_H */