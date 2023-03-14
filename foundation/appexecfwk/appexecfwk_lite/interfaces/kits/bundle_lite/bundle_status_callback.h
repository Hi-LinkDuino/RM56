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
 * @file bundle_status_callback.h
 *
 * @brief Declares the callback invoked upon state changes of an application and the structure defining the callback
 *        information.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef OHOS_BUNDLE_STATUS_CALLBACK_H
#define OHOS_BUNDLE_STATUS_CALLBACK_H

/**
 * @brief Called when the installation, update, or uninstallation state of an application changes.
 *
 * This callback is used to listen for state change results of applications in the system.
 * @param installType Indicates the installation, update, or uninstallation state. The value <b>0</b> indicates that
 *                    the application is being installed or updated, and <b>1</b> indicates that the application is
 *                    being uninstalled.
 * @param resultCode Indicates the status code returned for the application installation, update, or uninstallation
 *                   result. For details, see {@link AppexecfwkErrors}.
 * @param resultMessage Indicates the pointer to the result message returned with the status code.
 * @param bundleName Indicates the pointer to the name of the bundle whose state has changed.
 * @param data Indicates the pointer to the data that will be used by the callback upon a state change. The data is
 *             passed through {@link BundleStatusCallback} during callback registration.
 *
 * @since 1.0
 * @version 1.0
 */
typedef void (*BundleStateCallback)(const uint8_t installType, const uint8_t resultCode, const void *resultMessage,
    const char *bundleName, void *data);

/**
 * @brief Defines the application state callback information.
 */
typedef struct {
    /**
     * Pointer to the bundle name of the application that the callback listens to. If this parameter is set to
     * <b>nullptr</b>, the callback listens to all applications in the system.
     */
    char *bundleName;

    /**
     * Pointer to the registered data that will be used by {@link BundleStateCallback}.
     */
    void *data;

    /**
     * Callback to be invoked upon application state changes.
     */
    BundleStateCallback callBack;
} BundleStatusCallback;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif // __cplusplus
/**
 * @brief Clears and releases the memory occupied by the {@link BundleStatusCallback} structure.
 *
 *
 * @param bundleStatusCallback Indicates the pointer to the {@link BundleStatusCallback} structure to clear.
 *
 * @since 1.0
 * @version 1.0
 */
void ClearBundleStatusCallback(BundleStatusCallback *bundleStatusCallback);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif // __cplusplus
#endif // OHOS_BUNDLE_STATUS_CALLBACK_H
/** @} */