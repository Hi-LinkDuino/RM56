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

/**
 * @addtogroup BundleManager
 *
 * @brief Provides structures and functions for managing application bundles and obtaining application information.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file bundle_manager.h
 *
 * @brief Declares functions used for managing application bundles and obtaining bundle information.
 *
 * You can use functions provided in this file to install, update, or uninstall an application, obtain
 * {@link AbilityInfo} and {@link BundleInfo} about an application, obtain the bundle name of an application based
 * on the application's user ID (UID), and obtain {@link BundleInfo} objects of all applications or keep-alive
 * applications in the system.
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef OHOS_BUNDLEMANAGER_INTERFACE_H
#define OHOS_BUNDLEMANAGER_INTERFACE_H

#include "bundle_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Obtains the {@link BundleInfo} of an application based on the specified bundle name.
 *
 * @attention Before querying a {@link BundleInfo} object, you should first call <b>memset</b> on the constructed
 *            {@link BundleInfo} object so that each field in it can be properly initialized for carrying the
 *            obtained information.
 * @param bundleName Indicates the pointer to the name of the application bundle to query.
 * @param flags Specifies whether the obtained {@link BundleInfo} object can contain {@link AbilityInfo}. The value
 *              <b>1</b> indicates that it can contain {@link AbilityInfo}, and <b>0</b> indicates that it cannot.
 * @param bundleInfo Indicates the pointer to the obtained {@link BundleInfo} object.
 *
 * @return Returns {@link ERR_OK} if this function is successfully called; returns another error code defined in
 *         {@link AppexecfwkErrors} otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
uint8_t GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OHOS_BUNDLEMANAGER_INTERFACE_H */
/** @} */
