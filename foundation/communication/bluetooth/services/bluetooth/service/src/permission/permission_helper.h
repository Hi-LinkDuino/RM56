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

#ifndef PERMISSION_HELPER_H
#define PERMISSION_HELPER_H
#include <string>

namespace OHOS {
namespace bluetooth {
class PermissionHelper {
public:
    /**
     * @Description Verify where the app has the permission.
     *
     * @param permissionName Permission name.
     * @param pid The app's process id.
     * @param uid The app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyPermission(const std::string &permissionName, const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission.
     *
     * @param permissionName Permission name.
     * @param callerToken The app's token id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyPermission(const std::string &permissionName, const std::uint32_t &callerToken);

    /**
     * @Description Verify where the app has the permission to use bluetooth
     *
     * @param pid The app's process id.
     * @param uid The app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyUseBluetoothPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to discover bluetooth
     *
     * @param pid The app's process id.
     * @param uid The app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyDiscoverBluetoothPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to manager bluetooth
     *
     * @param pid The app's process id.
     * @param uid The app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyManageBluetoothPermission(const int &pid, const int &uid);

    /**
     * @Description Verify where the app has the permission to location
     *
     * @param pid The app's process id.
     * @param uid The app id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyLocationPermission(const int &pid, const int &uid);

        /**
     * @Description Verify where the app has the permission to use bluetooth
     *
     * @param tokenID The app's token id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyUseBluetoothPermission(const std::uint32_t  &tokenID);

    /**
     * @Description Verify where the app has the permission to discover bluetooth
     *
     * @param tokenID The app's token id.
     * @return int PERMISSION_DENIED or PERMISSION_GRANTED
     */
    static int VerifyDiscoverBluetoothPermission(const std::uint32_t  &tokenID);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif