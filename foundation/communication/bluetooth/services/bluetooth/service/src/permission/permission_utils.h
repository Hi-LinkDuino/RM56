/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef PERMISSION_UTILS_H
#define PERMISSION_UTILS_H

#include <cstdint>
#include "accesstoken_kit.h"

namespace OHOS {
namespace bluetooth {
using namespace OHOS::Security::AccessToken;

class PermissionUtils {
public:
    static int VerifyUseBluetoothPermission();
    static int VerifyDiscoverBluetoothPermission();
    static int VerifyManageBluetoothPermission();
    static int VerifyLocationPermission();

    static int VerifyUseBluetoothPermission(const std::uint32_t  &tokenID);
    static int VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif