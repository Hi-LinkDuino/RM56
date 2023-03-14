/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
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

#ifndef PRODUCT_ACELITE_CONFIG_H
#define PRODUCT_ACELITE_CONFIG_H

/**
 * the flag indicating watch product
 */
#define OHOS_ACELITE_PRODUCT_WATCH 1

/**
 * enable dm
 */
#define ENABLE_MODULE_DM_LITE 1

/**
 * enable Canvas component Feature API on ipcamera real devices
 */
#define FEATURE_COMPONENT_CANVAS 1

/**
 * enable QRcode component
 */
#define FEATURE_COMPONENT_QRCODE 1

/**
 * support terminate() API from JS
 */
#define FEATURE_TERMINATE_ABILITY 1

/**
 * support screenOnVisible() API from JS
 */
#define FEATURE_SCREEN_ON_VISIBLE 1

#define FEATURE_FEATURE_ABILITY_MODULE 0

/**
 * enable timer JS API
 */
#define FEATURE_TIMER_MODULE 1

#define FEATURE_LOCALIZATION_MODULE 0

#define FEATURE_ACELITE_HI_LOG_PRINTF 1

#define FEATURE_ACELITE_MC_LOG_PRINTF 0

#define FEATURE_ACELITE_MC_EVENT_ERROR_CODE_PRINT 0

#define FEATURE_USER_MC_LOG_PRINTF 0

/**
 * support config for product JS modules
 */
#define FEATURE_PRODUCT_MODULE 1

/**
 * support config for private JS modules
 */
#define FEATURE_PRIVATE_MODULE 1

/**
 * support using memory pool from product side
 */
#define FEATURE_ACELITE_PRODUCT_MEMORY_POOL 1

#define FEATURE_ACELITE_DFX_MODULE 0

#define FEATURE_ACELITE_LITE_DFX_MODULE 0

/**
 * support mc lazy loading
 */
#define FEATURE_LAZY_LOADING_MODULE 1

/**
 * support intl module
 */
#define FEATURE_INTL_MODULE 1

/**
 * supoort number format module
 */
#define FEATURE_NUMBER_FORMAT 1

/**
 * date time module
 */
#define FEATURE_DATE_FORMAT 0

#define FEATURE_API_VERSION 0

#define JS_ENGINE_STATIC_MULTI_CONTEXTS_ENABLED 0

#define FEATURE_MODULE_STORAGE 1

#define FEATURE_MODULE_DEVICE 1

#define FEATURE_MODULE_AUDIO 1
/**
 * support js profiler
 */
#define FEATURE_ACELITE_MC_JS_PROFILER 1

#define FEATURE_ROTATION_API 0

#endif // PRODUCT_ACELITE_CONFIG_H
