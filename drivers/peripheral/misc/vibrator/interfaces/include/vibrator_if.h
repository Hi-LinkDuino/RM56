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
 * @brief Provides a driver for upper-layer vibrator services.
 *
 * After obtaining a driver object or agent, a vibrator service starts or stops the vibrator
 * using the functions provided by the driver object or agent.
 *
 * @since 2.2
 */

/**
 * @file vibrator_if.h
 *
 * @brief Declares common APIs in the vibrator module. The APIs can be used to control
 * the vibrator to perform a one-shot or periodic vibration.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef VIBRATOR_IF_H
#define VIBRATOR_IF_H

#include <stdint.h>
#include "vibrator_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct VibratorInterface {
    /**
     * @brief Controls the vibrator to perform a one-shot vibration that lasts for a given duration.
     *
     * One-shot vibration is mutually exclusive with periodic vibration. Before using one-shot vibration,
     * exit periodic vibration.
     *
     * @param duration Indicates the duration that the one-shot vibration lasts, in milliseconds.
      * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*StartOnce)(uint32_t duration);
    /**
     * @brief Controls the vibrator to perform a periodic vibration with the preset effect.
     *
     * One-shot vibration is mutually exclusive with periodic vibration. Before using periodic vibration,
     * exit one-shot vibration.
     *
     * @param effectType Indicates the pointer to the preset effect type. It is recommended that the
     * maximum length be 64 bytes.
      * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*Start)(const char *effectType);
    /**
     * @brief Stops the vibration.
     *
     * Before the vibrator starts, it must stop vibrating in any mode. This function can be used during
     * and after the vibrating process.
     *
     * @param mode Indicates the vibration mode, which can be one-shot or periodic. For details,
     * see {@link VibratorMode}.
      * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*Stop)(enum VibratorMode mode);
};

/**
 * @brief Creates a <b>VibratorInterface</b> instance.
 *
 * The obtained <b>VibratorInterface</b> instance can be used to control the vibrator to vibrate as configured.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 2.2
 * @version 1.0
 */
const struct VibratorInterface *NewVibratorInterfaceInstance(void);

/**
 * @brief Releases this <b>VibratorInterface</b> instance to free up related resources.
 *
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 2.2
 * @version 1.0
 */
int32_t FreeVibratorInterfaceInstance(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* VIBRATOR_IF_H */
/** @} */
