/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_CONFIG_H
#define OHOS_ACELITE_CONFIG_H

#if (ENABLE_OHOS_ACELITE_PRODUCT_CONFIG == 1)
#include "product_acelite_config.h"
#endif

/**
 * ================================================================================================
 *                 liteos_a config for hmf
 * ================================================================================================
 */
#define OHOS_ACELITE_LITEOS_A // NOTE: DO NOT use this macro directly

/**
 * Compile all test entry for hmf
 */
#ifndef FEATURE_TEST_IMPLEMENTATION
#define FEATURE_TEST_IMPLEMENTATION 1
#endif

/**
 * use hi log interface to trace out
 */
#ifndef FEATURE_ACELITE_HI_LOG_PRINTF
#define FEATURE_ACELITE_HI_LOG_PRINTF 1
#endif

/**
 * append no blank color to rootview for easy using, delete this after the mouse showing is optimized
 */
#ifndef FEATURE_ROOTVIEW_CUSTOM_BACKGROUND_COLOR
#define FEATURE_ROOTVIEW_CUSTOM_BACKGROUND_COLOR 1
#endif

/**
 * js system capabillity switch
 */
#ifndef FEATURE_ACELITE_SYSTEM_CAPABILITY
#define FEATURE_ACELITE_SYSTEM_CAPABILITY 1
#endif

/**
 * js profiler switch
 */
#ifndef FEATURE_ACELITE_JS_PROFILER
#define FEATURE_ACELITE_JS_PROFILER 1
#endif

/**
 * support audio API for JS
 */
#ifndef FEATURE_MODULE_AUDIO
#define FEATURE_MODULE_AUDIO 1
#endif

/**
 * switch for cipher module
 */
#ifndef ENABLE_MODULE_CIPHER
#define ENABLE_MODULE_CIPHER 1
#endif

/**
 * support storage and file API for JS
 */
#ifndef FEATURE_MODULE_STORAGE
#define FEATURE_MODULE_STORAGE 1
#endif

/**
 * support battery API for JS
 */
#ifndef FEATURE_MODULE_BATTERY
#define FEATURE_MODULE_BATTERY 1
#endif

/**
 * support device API for JS
 */
#ifndef FEATURE_MODULE_DEVICE
#define FEATURE_MODULE_DEVICE 1
#endif

/**
 * timer module
 */
#ifndef FEATURE_TIMER_MODULE
#define FEATURE_TIMER_MODULE 1
#endif

/**
 * date time module
 */
#ifndef FEATURE_DATE_FORMAT
#define FEATURE_DATE_FORMAT 1
#endif

#ifndef FEATURE_API_VERSION
#define FEATURE_API_VERSION 1
#endif

/**
 * support lite ace dfx module
 */
#ifndef FEATURE_ACELITE_DFX_MODULE
#define FEATURE_ACELITE_DFX_MODULE 1
#endif

#ifndef FEATURE_ACELITE_LITE_DFX_MODULE
#define FEATURE_ACELITE_LITE_DFX_MODULE 1
#endif

/**
 * enable Canvas component Feature API on ipcamera real devices
 */
#ifndef FEATURE_COMPONENT_CANVAS
#define FEATURE_COMPONENT_CANVAS 1
#endif

#ifndef FEATURE_LOCALIZATION_MODULE
#define FEATURE_LOCALIZATION_MODULE 1
#endif

#ifndef FEATURE_COMPONENT_CAMERA
#define FEATURE_COMPONENT_CAMERA 1
#endif

#ifndef FEATURE_COMPONENT_QRCODE
#define FEATURE_COMPONENT_QRCODE 1
#endif

#ifndef FEATURE_COMPONENT_VIDEO
#define FEATURE_COMPONENT_VIDEO 1
#endif

#ifndef FEATURE_MODULE_DIALOG
#define FEATURE_MODULE_DIALOG 1
#endif

/**
 * support intl module
 */
#ifndef FEATURE_INTL_MODULE
#define FEATURE_INTL_MODULE 1
#endif

/**
 * supoort number format module
 */
#ifndef FEATURE_NUMBER_FORMAT
#define FEATURE_NUMBER_FORMAT 1
#endif

#ifndef FEATURE_UPDATE_VIDEO_PROGRESS_ASYNC
#define FEATURE_UPDATE_VIDEO_PROGRESS_ASYNC 1
#endif

#ifndef DEFAULT_PROFILER_MSG_PATH
#define DEFAULT_PROFILER_MSG_PATH "/storage/user/data/"
#endif

#ifndef PROFILER_ENABLE_FLAG_FILE
#define PROFILER_ENABLE_FLAG_FILE "/storage/user/data/profiler_enable"
#endif

#ifndef RUNTIME_MODE_FILE_PATH
#define RUNTIME_MODE_FILE_PATH "/storage/user/data/js_snapshot_enable"
#endif

#ifndef FEATURE_SUPPORT_HTTP
#define FEATURE_SUPPORT_HTTP 1
#endif

#endif // OHOS_ACELITE_CONFIG_H
