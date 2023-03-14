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

#ifndef OHOS_BUNDLE_INFO_H
#define OHOS_BUNDLE_INFO_H

#include <cstdint>

/**
 * @brief The mock defines the bundle information, please refer to the bundle_info.h file locating in
 *        appexecfwk_appexecfwk_lite repo.
 */
typedef struct {
    /**
     * Whether the application is a system application. System applications cannot be uninstalled. The value
     * <b>true</b> indicates a system application, and <b>false</b> indicates a non-system application.
     */
    bool isSystemApp;

    /** Minimum API version required */
    int32_t compatibleApi;

    /** Target API version */
    int32_t targetApi;

    /** Version code of the application, which is the internal version number */
    int32_t versionCode;

    /** Pointer to the version name visible to users */
    char *versionName;

    /** Pointer to the bundle name of the application */
    char *bundleName;

    /** Pointer to the application name visible to users */
    char *label;
} BundleInfo;

#endif // OHOS_BUNDLE_INFO_H
/** @} */
