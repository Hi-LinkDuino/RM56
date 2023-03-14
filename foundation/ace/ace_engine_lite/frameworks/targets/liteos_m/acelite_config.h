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

// some features depend on the low layer configuration
#include "graphic_config.h"

#if (ENABLE_OHOS_ACELITE_PRODUCT_CONFIG == 1)
#include "product_acelite_config.h"
#endif

/**
 * analog clock component
 */
#ifdef FEATURE_COMPONENT_ANALOG_CLOCK
#error "must keep the global configuration unique"
#else
#define FEATURE_COMPONENT_ANALOG_CLOCK 1
#endif
/**
 * ================================================================================================
 *                 config for wearable
 * ================================================================================================
 */
/**
 * the flag indicating watch product
 */
#ifndef OHOS_ACELITE_PRODUCT_WATCH
#define OHOS_ACELITE_PRODUCT_WATCH 1
#endif

// delete this after jerry porting is ready
#ifndef JERRY_PORTING_DEPENDENCY
#define JERRY_PORTING_DEPENDENCY 1
#endif

/**
 * enable Canvas component Feature API on ipcamera real devices
 */
#ifndef FEATURE_COMPONENT_CANVAS
#define FEATURE_COMPONENT_CANVAS 1
#endif

/**
 * enable QRcode component
 */
#ifndef FEATURE_COMPONENT_QRCODE
#define FEATURE_COMPONENT_QRCODE 1
#endif

/**
 * support terminate() API from JS
 */
#ifndef FEATURE_TERMINATE_ABILITY
#define FEATURE_TERMINATE_ABILITY 1
#endif

/**
 * support screenOnVisible() API from JS
 */
#ifndef FEATURE_SCREEN_ON_VISIBLE
#define FEATURE_SCREEN_ON_VISIBLE 1
#endif

/**
 * enable FeatureAbility API
 */
#ifndef FEATURE_FEATURE_ABILITY_MODULE
#define FEATURE_FEATURE_ABILITY_MODULE 1
#endif

/**
 * enable timer JS API
 */
#ifndef FEATURE_TIMER_MODULE
#define FEATURE_TIMER_MODULE 1
#endif

/**
 * enable localization feature
 */
#ifndef FEATURE_LOCALIZATION_MODULE
#define FEATURE_LOCALIZATION_MODULE 1
#endif

#ifndef FEATURE_ACELITE_HI_LOG_PRINTF
#define FEATURE_ACELITE_HI_LOG_PRINTF 1
#endif

/**
 * support mc ace log module trace
 */
#ifndef FEATURE_ACELITE_MC_LOG_PRINTF
#define FEATURE_ACELITE_MC_LOG_PRINTF 1
#endif

/**
 * support mc ace event and error code print
 */
#ifndef FEATURE_ACELITE_MC_EVENT_ERROR_CODE_PRINT
#define FEATURE_ACELITE_MC_EVENT_ERROR_CODE_PRINT 1
#endif

/**
 * support mc console log
 */
#ifndef FEATURE_USER_MC_LOG_PRINTF
#define FEATURE_USER_MC_LOG_PRINTF 1
#endif

/**
 * support config for product JS modules
 */
#ifndef FEATURE_PRODUCT_MODULE
#define FEATURE_PRODUCT_MODULE 1
#endif

/**
 * support config for private JS modules
 */
#ifndef FEATURE_PRIVATE_MODULE
#define FEATURE_PRIVATE_MODULE 1
#endif

/**
 * support using memory pool from product side
 */
#ifndef FEATURE_ACELITE_PRODUCT_MEMORY_POOL
#define FEATURE_ACELITE_PRODUCT_MEMORY_POOL 1
#endif

/**
 * define the max length of user's console log
 */
#ifndef CONSOLE_LOG_LINE_MAX_LENGTH
#define CONSOLE_LOG_LINE_MAX_LENGTH (96)
#endif

#ifndef ACELITE_DEPENDENCY_IS_READY
#define ACELITE_DEPENDENCY_IS_READY 1
#endif
/**
 * support mc ace dfx module
 */
#ifndef FEATURE_ACELITE_DFX_MODULE
#define FEATURE_ACELITE_DFX_MODULE 1
#endif

#ifndef FEATURE_ACELITE_LITE_DFX_MODULE
#define FEATURE_ACELITE_LITE_DFX_MODULE 1
#endif

/**
 * support mc lazy loading
 */
#ifndef FEATURE_LAZY_LOADING_MODULE
#define FEATURE_LAZY_LOADING_MODULE 1
#endif

/**
 * support fatal error handling on device which not support process
 */
#ifndef FEATURE_FATAL_ERROR_HANDLING
#define FEATURE_FATAL_ERROR_HANDLING 1
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

/**
 * date time module
 */
#ifndef FEATURE_DATE_FORMAT
#define FEATURE_DATE_FORMAT 1
#endif

#ifndef FEATURE_API_VERSION
#define FEATURE_API_VERSION 1
#endif

#ifndef JS_ENGINE_STATIC_MULTI_CONTEXTS_ENABLED
#define JS_ENGINE_STATIC_MULTI_CONTEXTS_ENABLED 1
#endif

#ifndef FEATURE_MODULE_STORAGE
#define FEATURE_MODULE_STORAGE 0
#endif

#ifndef FEATURE_MODULE_AUDIO
#define FEATURE_MODULE_AUDIO 0
#endif

#ifndef FEATURE_MODULE_DEVICE
#define FEATURE_MODULE_DEVICE 0
#endif

#ifndef FEATURE_SUPPORT_HTTP
#define FEATURE_SUPPORT_HTTP 0
#endif

#ifndef FEATURE_MODULE_GEO
#define FEATURE_MODULE_GEO 0
#endif

#ifndef FEATURE_MODULE_SENSOR
#define FEATURE_MODULE_SENSOR 0
#endif

#ifndef FEATURE_MODULE_BRIGHTNESS
#define FEATURE_MODULE_BRIGHTNESS 0
#endif

#ifndef FEATURE_MODULE_BATTERY
#define FEATURE_MODULE_BATTERY 0
#endif

#ifndef FEATURE_MODULE_CONFIGURATION
#define FEATURE_MODULE_CONFIGURATION 0
#endif

/**
 * support js profiler
 */
#ifndef FEATURE_ACELITE_MC_JS_PROFILER
#define FEATURE_ACELITE_MC_JS_PROFILER 1
#endif

#ifndef DEFAULT_PROFILER_MSG_PATH
#define DEFAULT_PROFILER_MSG_PATH "user/log/"
#endif

#ifndef PROFILER_ENABLE_FLAG_FILE
#define PROFILER_ENABLE_FLAG_FILE "user/data/profiler_enable"
#endif

#ifndef RUNTIME_MODE_FILE_PATH
#define RUNTIME_MODE_FILE_PATH "user/data/jsparser_enable"
#endif

/**
 * Rotation API feature
 */
#if (ENABLE_ROTATE_INPUT == 1)
// low layer supports
#ifndef FEATURE_ROTATION_API
#define FEATURE_ROTATION_API 1
#endif
#endif // (ENABLE_ROTATE_INPUT == 1)

#endif // OHOS_ACELITE_CONFIG_H
