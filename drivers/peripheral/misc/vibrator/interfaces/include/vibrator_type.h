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
 * @addtogroup Vibrator
 * @{
 *
 * @brief Provides unified APIs for vibrator services to access the vibrator driver.
 *
 * After obtaining a driver object or agent, a vibrator service starts or stops the vibrator
 * using the functions provided by the driver object or agent.
 *
 * @version 1.0
 */

/**
 * @file vibrator_type.h
 *
 * @brief Defines the vibrator data structure, including the vibration mode and effect type.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef VIBRATOR_TYPE_H
#define VIBRATOR_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates the vibration modes of this vibrator.
 *
 * @since 2.2
 */
enum VibratorMode {
    VIBRATOR_MODE_ONCE   = 0,    /**< Indicates the one-shot vibration with the given duration. */
    VIBRATOR_MODE_PRESET = 1,    /**< Indicates the periodic vibration with the preset effect. */
    VIBRATOR_MODE_BUTT           /**< Indicates invalid the effect mode. */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* VIBRATOR_TYPE_H */
/** @} */
