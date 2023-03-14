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

#ifndef AUTH_CENTER_H
#define AUTH_CENTER_H

#include <cstdint>

namespace OHOS {
namespace bluetooth {
class AuthCenter {
public:
    static AuthCenter &GetInstance();

    /**
     * @Description Verify where the app has the permission to use bluetooth
     *
     * @param pid the app's process id.
     * @param uid the app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyUseBluetoothPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to discover bluetooth
     *
     * @param pid the app's process id.
     * @param uid the app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyDiscoverBluetoothPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to manager bluetooth
     *
     * @param pid the app's process id.
     * @param uid the app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyManageBluetoothPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to Location
     *
     * @param pid the app's process id.
     * @param uid the app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyLocationPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to use bluetooth
     *
     * @param tokenID the app's token id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyUseBluetoothPermission(const std::uint32_t  &tokenID);

    /**
     * @Description Verify where the app has the permission to discover bluetooth
     *
     * @param tokenID the app's token id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif