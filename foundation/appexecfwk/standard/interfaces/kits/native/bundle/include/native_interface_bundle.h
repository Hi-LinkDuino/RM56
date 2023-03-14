/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 * @addtogroup Native_Bundle
 * @{
 *
 * @brief Describes the Native Bundle.
 *
 * @since 9
 * @version 1.0
 */

/**
 * @file native_interface_bundle.h
 *
 * @brief Declares the <b>Bundle</b>-specific function, including function for obtaining application ID.
 *
 * @since 9
 * @version 1.0
 */
#ifndef FOUNDATION_APPEXECFWK_STANDARD_KITS_APPKIT_NATIVE_BUNDLE_INCLUDE_NATIVE_INTERFACE_BUNDLE_H
#define FOUNDATION_APPEXECFWK_STANDARD_KITS_APPKIT_NATIVE_BUNDLE_INCLUDE_NATIVE_INTERFACE_BUNDLE_H

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Obtains the application ID based on the given bundle name.
 *
 * @param bundleName Indicates the bundle name of the application.
 * @return Returns the application ID pointer if successfully obtained and needs to be freed; returns nullptr otherwise.
 * @since 9
 * @version 1.0
 */
char* OH_NativeBundle_GetAppIdByBundleName(const char* bundleName);

#ifdef __cplusplus
};
#endif
#endif // FOUNDATION_APPEXECFWK_STANDARD_KITS_APPKIT_NATIVE_BUNDLE_INCLUDE_NATIVE_INTERFACE_BUNDLE_H
