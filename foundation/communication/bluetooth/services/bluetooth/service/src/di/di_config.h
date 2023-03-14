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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Defines device info config functions.
 *
 * @since 6
 */

/**
 * @file device_info_config.h
 *
 * @brief Device info config.
 *
 * @since 6
 */

#ifndef DEVICE_INFO_CONFIG_H
#define DEVICE_INFO_CONFIG_H

#include "adapter_device_info.h"
#include "base_def.h"

namespace OHOS {
namespace bluetooth {
/**
 * @brief Represents device info config.
 *
 * @since 6
 */
class DIConfig {
public:
    /**
     * @brief Get device info config singleton instance pointer.
     *
     * @return Returns the singleton instance pointer.
     * @since 6
     */
    static DIConfig &GetInstance();

    /**
     * @brief Load config info.
     *
     * @return Returns <b>true</b> if the operation is successful;
     *         returns <b>false</b> if the operation fails.
     * @since 6
     */
    bool LoadConfigFile() const;

    /**
     * @brief Get specification id.
     *
     * @return Returns specification id.
     * @since 6
     */
    int GetSpecificaitonId() const;

    /**
     * @brief Get vendor id.
     *
     * @return Returns vendor id.
     * @since 6
     */
    int GetVendorId() const;

    /**
     * @brief Get product id.
     *
     * @return Returns product id.
     * @since 6
     */
    int GetProductId() const;

    /**
     * @brief Get version.
     *
     * @return Returns version.
     * @since 6
     */
    int GetVersion() const;

    /**
     * @brief Get primary record.
     *
     * @return Returns primary record.
     * @since 6
     */
    bool GetPrimaryRecord() const;

    /**
     * @brief Get vendor id source.
     *
     * @return Returns vendor id source.
     * @since 6
     */
    int GetVendorIdSource() const;

private:
    /**
     * @brief A constructor used to create a <b>DIConfig</b> instance.
     *
     * @since 6
     */
    DIConfig();

    /**
     * @brief A destructor used to delete the <b>DIConfig</b> instance.
     *
     * @since 6
     */
    ~DIConfig();

    IAdapterDeviceInfo *config_ {nullptr};

    BT_DISALLOW_COPY_AND_ASSIGN(DIConfig);
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // DEVICE_INFO_CONFIG_H