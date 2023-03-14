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

/**
 * ================================================================================================
 *                 hi3516_linux config for hmf
 * ================================================================================================
 */

/**
 * Compile all test entry for hmf
 */
#ifdef FEATURE_TEST_IMPLEMENTATION
#error "must keep the global configuration unique"
#else
#define FEATURE_TEST_IMPLEMENTATION 1
#endif

/**
 * use hi log interface to trace out
 */
#ifdef FEATURE_ACELITE_HI_LOG_PRINTF
#error "must keep the global configuration unique"
#else
#define FEATURE_ACELITE_HI_LOG_PRINTF 1
#endif

/**
 * append no blank color to rootview for easy using, delete this after the mouse showing is optimized
 */
#ifdef FEATURE_ROOTVIEW_CUSTOM_BACKGROUND_COLOR
#error "must keep the global configuration unique"
#else
#define FEATURE_ROOTVIEW_CUSTOM_BACKGROUND_COLOR 1
#endif

/**
 * js system capabillity switch
 */
#ifdef FEATURE_ACELITE_SYSTEM_CAPABILITY
#error "must keep the system capability unique"
#else
#define FEATURE_ACELITE_SYSTEM_CAPABILITY 1
#endif

/**
 * js profiler switch
 */
#ifdef FEATURE_ACELITE_JS_PROFILER
#error "must keep the global configuration unique"
#else
#define FEATURE_ACELITE_JS_PROFILER 1
#endif

/**
 * support audio API for JS
 */
#define FEATURE_MODULE_AUDIO 1

/**
 * switch for cipher module
 */
#define ENABLE_MODULE_CIPHER 1

/**
 * support storage and file API for JS
 */
#define FEATURE_MODULE_STORAGE 1

/**
 * support device API for JS
 */
#define FEATURE_MODULE_DEVICE 1

/**
 * timer module
 */
#define FEATURE_TIMER_MODULE 1

/**
 * support lite ace dfx module
 */
#define FEATURE_ACELITE_DFX_MODULE 1

#define FEATURE_ACELITE_LITE_DFX_MODULE 1

#define FEATURE_API_VERSION 1

/**
 * enable Canvas component Feature API on ipcamera real devices
 */
#define FEATURE_COMPONENT_CANVAS 1

#define FEATURE_LOCALIZATION_MODULE 1

#ifdef FEATURE_COMPONENT_CAMERA
#error "must keep the global configuration unique"
#else
#define FEATURE_COMPONENT_CAMERA 1
#endif

#ifdef FEATURE_COMPONENT_VIDEO
#error "must keep the global configuration unique"
#else
#define FEATURE_COMPONENT_VIDEO 1
#endif

#define FEATURE_UPDATE_VIDEO_PROGRESS_ASYNC 1

#ifdef FEATURE_MODULE_DIALOG
#error "must keep the global configuration unique"
#else
#define FEATURE_MODULE_DIALOG 1
#endif

#define DEFAULT_PROFILER_MSG_PATH "/user/data/"

#define PROFILER_ENABLE_FLAG_FILE "/user/data/profiler_enable"

#define RUNTIME_MODE_FILE_PATH "/user/data/js_snapshot_enable"

#endif // OHOS_ACELITE_CONFIG_H

