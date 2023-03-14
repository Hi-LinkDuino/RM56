/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
 * @addtogroup BundleManager
 * @{
 *
 * @brief Provides structures and functions for managing application bundles and obtaining application information.
 *
 * You can use functions provided by this module to install, update, or uninstall an application, obtain
 * {@link AbilityInfo} and {@link BundleInfo} about an application, and obtain the bundle name of an application
 * based on the application's user ID (UID).
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file installParam.h
 *
 * @brief Declares the parameters that users can specify when they install, update, or uninstall an application.
 *
 * The parameters involve the application installation or update path and whether to retain particular data during
 * application uninstallation.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_INSTALL_PARAM_H
#define OHOS_INSTALL_PARAM_H

#include "stdint.h"

#include <stdbool.h>

enum InstallLocation {
/**
 * The installation or update path is <b>storage/app</b> when {@link installLocation} is set to <b>1</b>.
 */
    INSTALL_LOCATION_INTERNAL_ONLY = 1,
/**
 * The installation or update path is <b>sdcard/app</b> when {@link installLocation} is set to <b>2</b>.
 */
    INSTALL_LOCATION_PREFER_EXTERNAL = 2,
};

/**
* @brief Defines parameters used for application installation, update, or uninstallation.
*/
typedef struct {
    /** Installation or update path of the application */
    int32_t installLocation;

    /** Whether to retain particular data during application uninstallation */
    bool keepData;
} InstallParam;

#endif // OHOS_INSTALL_PARAM_H
/** @} */