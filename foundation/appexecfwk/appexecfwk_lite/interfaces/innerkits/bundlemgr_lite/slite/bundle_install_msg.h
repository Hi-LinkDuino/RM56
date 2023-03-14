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

#ifndef OHOS_BUNDLE_INSTALL_MSG_H
#define OHOS_BUNDLE_INSTALL_MSG_H

#include <stdint.h>

#include "bundle_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    BUNDLE_INSTALL_DOING = 0,
    BUNDLE_INSTALL_OK = 1,
    BUNDLE_INSTALL_FAIL = 2,
    BUNDLE_UNINSTALL_DOING = 3,
    BUNDLE_UNINSTALL_OK = 4,
    BUNDLE_UNINSTALL_FAIL = 5
} InstallState;

typedef struct {
    char *bundleName;
    char *label;
    char *smallIconPath;
    char *bigIconPath;
    InstallState installState;
    uint8_t installProcess;
} BundleInstallMsg;

/**
 * @brief Get the install state and install process of the bundle.
 *
 * @param bundleName Indicates the name of the bundle.
 * @param installState Obtains install state.
 * @param installProcess Obtains install process.
 * @return Returns success or not.
 *
 * @since 4.0
 * @version 4.0
 */
bool GetInstallState(const char *bundleName, InstallState *installState, uint8_t *installProcess);

/**
 * @brief Get the bundle size.
 *
 * @param bundleName Indicates the name of the bundle.
 * @return Returns bundle size or returns 0 if get bundle size failed.
 *
 */
uint32_t GetBundleSize(const char *bundleName);

/**
 * @brief Register installer callback.
 *
 * @param installerCallback Indicates the installer callback.
 * @return Returns success or failure.
 *
 */
bool RegisterInstallerCallback(InstallerCallback installerCallback);

/**
 * @brief Update bundleInfo when system language has been changed.
 *
 */
void UpdateBundleInfoList(void);

 /**
 * @brief Obtains the {@link BundleInfo} of all bundles with no replication in the system.
 *
 * @param flags Specifies whether each of the obtained {@link BundleInfo} objects can contain {@link AbilityInfo}.
 *               The value <b>1</b> indicates that it can contain {@link AbilityInfo}, and <b>0</b> indicates that
 *              it cannot.
 * @param bundleInfos Indicates the double pointer to the obtained {@link BundleInfo} objects.
 * @param len Indicates the pointer to the number of {@link BundleInfo} objects obtained.
 * @return Returns {@link ERR_OK} if this function is successfully called; returns another error code defined in
 *         {@link AppexecfwkErrors} otherwise.
 *
 */
uint8_t GetBundleInfosNoReplication(const int flags, BundleInfo **bundleInfos, int32_t *len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OHOS_BUNDLE_INSTALL_MSG_H */
/** @} */