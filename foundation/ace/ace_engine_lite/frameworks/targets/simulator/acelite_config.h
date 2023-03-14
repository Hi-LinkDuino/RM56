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

/**
 * ================================================================================================
 *                 config for win simulator
 * ================================================================================================
 */
#define OHOS_ACELITE_SIMULATOR_WIN // NOTE: DO NOT use this macro directly

#ifndef JSFWK_TEST
#define JSFWK_TEST 1
#endif

/**
 * The macro is used to distinguish the real device and simulator.
 */
#ifndef TARGET_SIMULATOR
#define TARGET_SIMULATOR 1
#endif

/**
 * Compile all test entry for hmf
 */
#ifdef FEATURE_TEST_IMPLEMENTATION
#error "must keep the global configuration unique"
#else
#define FEATURE_TEST_IMPLEMENTATION 1
#endif

/**
 * enable FeatureAbility API
 */
#define FEATURE_FEATURE_ABILITY_MODULE 1

/**
 * support device API for JS
 */
#define FEATURE_MODULE_DEVICE 1

/**
 * support geo location API for JS
 */
#define FEATURE_MODULE_GEO 1

/**
 * support sensor API for JS
 */
#define FEATURE_MODULE_SENSOR 1

/**
 * support brightness API for JS
 */
#define FEATURE_MODULE_BRIGHTNESS 1

/**
 * support battery API for JS
 */
#define FEATURE_MODULE_BATTERY 1

/**
 * support configuration API for JS
 */
#define FEATURE_MODULE_CONFIGURATION 1

/**
 * timer module
 */
#define FEATURE_TIMER_MODULE 1

/**
 * execute timer callback directly for previewer
 */
#define SYNC_TIMER_CALLBACK

/**
 * support storage API for JS
 */
#define FEATURE_MODULE_STORAGE 1

/**
 * localization module
 */
#define FEATURE_LOCALIZATION_MODULE 1

#define FEATURE_CUSTOM_ENTRY_PAGE 1

/**
 * module require test
 */
#ifdef ENABLE_MODULE_REQUIRE_TEST
#error "must keep the global configuration unique"
#else
#define ENABLE_MODULE_REQUIRE_TEST 1
#endif

/**
 * define the max length of user's console log
 */
#ifndef CONSOLE_LOG_LINE_MAX_LENGTH
#define CONSOLE_LOG_LINE_MAX_LENGTH (96)
#endif

/**
 * enable Canvas component Feature API on simulator
 */
#ifndef FEATURE_COMPONENT_CANVAS
#define FEATURE_COMPONENT_CANVAS 1
#endif

#if (FEATURE_COMPONENT_QRCODE == 1)
#error "must keep the global configuration unique"
#else
#define FEATURE_COMPONENT_QRCODE 1
#endif

#ifdef _WIN32
// support memory analysis, only on win
#define SIMULATOR_MEMORY_ANALYSIS
#endif

#ifdef _WIN32
#define PROFILER_ENABLE_FLAG_FILE "..\\profiler_enable"
#ifdef SIMULATOR_MEMORY_ANALYSIS
#define MEM_PROC_ENABLE_FLAG_FILE "..\\memproc_enable"
#define MEM_LOG_FILE_PATH "..\\mem.txt"
#define MEM_BACK_UP_LOG_FILE_PREFIX "..\\"
#endif // SIMULATOR_MEMORY_ANALYSIS
#else
// path format is different on linux or apple with windows
#define PROFILER_ENABLE_FLAG_FILE "~/profiler_enable"
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
