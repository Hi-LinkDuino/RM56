/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines profile service manager, including common functions.
 *
 * @since 6
 */

/**
 * @file interface_profile_manager.h
 *
 * @brief profile manager interface.
 *
 * @since 6
 */

#ifndef INTERFACE_PROFILE_MANAGER_H
#define INTERFACE_PROFILE_MANAGER_H

#include "interface_profile.h"
#include <vector>

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents interface profile manager.
 *
 * @since 6
 */
class BLUETOOTH_API IProfileManager {
public:
    /**
     * @brief A destructor used to delete the <b>IProfileManager</b> instance.
     *
     * @since 6
     */
    virtual ~IProfileManager() = default;

    /**
     * @brief Get profile manager singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     * @since 6
     */
    static IProfileManager *GetInstance();

    /**
     * @brief Get profile service pointer.
     *
     * @param name Profile service name.
     * @return Returns the profile service pointer.
     * @since 6
     */
    virtual IProfile *GetProfileService(const std::string &name) const = 0;

    /**
     * @brief Get profile service ID list.
     *
     * @return Returns vector of enabled profile services ID.
     * @since 6
     */
    virtual std::vector<uint32_t> GetProfileServicesList() const = 0;

    /**
     * @brief Get profile service connect state.
     *
     * @param profileID Profile service ID.
     * @return Returns connect state for designated profile service.
     * @since 6
     */
    virtual BTConnectState GetProfileServiceConnectState(const uint32_t profileID) const = 0;

    /**
     * @brief Get local device supported uuids.
     *
     * @param[out] Vector which use to return support uuids.
     * @since 6
     */
    virtual void GetProfileServicesSupportedUuids(std::vector<std::string> &uuids) const = 0;
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // INTERFACE_PROFILE_MANAGER_H