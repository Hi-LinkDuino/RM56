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
 * @addtogroup Light
 * @{
 *
 * @brief Provides APIs for the light service.
 *
 * The light module provides a unified interface for the light service to access the light driver.
 * After obtaining the light driver object or proxy, the service can call related APIs to obtain light information,
 * turn on or off a light, and set the light blinking mode based on the light type.
 * @since 3.1
 */

/**
 * @file light_type.h
 *
 * @brief Defines the light data structure, including the light type, lighting mode,
 * blinking mode and duration, return values, and lighting effect.
 * @since 3.1
 */

#ifndef LIGHT_TYPE_H
#define LIGHT_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates the return values of the light module.
 *
 * @since 3.1
 */
enum LightStatus {
    /** The operation is successful. */
    LIGHT_SUCCESS            = 0,
    /** The light type is not supported. */
    LIGHT_NOT_SUPPORT        = -1,
    /** Blinking setting is not supported. */
    LIGHT_NOT_FLASH          = -2,
    /** Brightness setting is not supported. */
    LIGHT_NOT_BRIGHTNESS     = -3,
};

/**
 * @brief Enumerates the light types.
 *
 * @since 3.1
 */
enum LightType {
    /** Unknown type */
    LIGHT_TYPE_NONE                = 0,
    /** Power light */
    LIGHT_TYPE_BATTERY             = 1,
    /** Notification light */
    LIGHT_TYPE_NOTIFICATIONS       = 2,
    /** Alarm light */
    LIGHT_TYPE_ATTENTION           = 3,
    /** Invalid type */
    LIGHT_TYPE_BUTT                = 4,
};

/**
 * @brief Enumerates the lighting modes.
 *
 * @since 3.1
 */
enum LightFlashMode {
    /** Steady on */
    LIGHT_FLASH_NONE     = 0,
    /** Blinking */
    LIGHT_FLASH_TIMED    = 1,
    /** Invalid mode */
    LIGHT_FLASH_BUTT     = 2,
};

/**
 * @brief Defines the blinking parameters.
 *
 * The parameters include the blinking mode and the on and off time for the light in a blinking period.
 *
 * @since 3.1
 */
struct LightFlashEffect {
    int32_t flashMode; /** Blinking mode. For details, see {@link LightFlashMode}. */
    int32_t onTime;      /** Duration (in ms) for which the light is on in a blinking period. */
    int32_t offTime;     /** Duration (in ms) for which the light is off in a blinking period. */
};

/**
 * @brief Defines the lighting effect parameters.
 *
 * The parameters include the brightness and blinking mode.
 *
 * @since 3.1
 */
struct LightEffect {
    int32_t lightBrightness;    /** Brightness value. The value 1 of the most significant bit indicates a color.
	                                Bits 16–31 for red, bits 8–15 for green, and bits 0–7 for blue. */
    struct LightFlashEffect flashEffect;    /** Blinking mode. For details, see {@link LightFlashEffect}. */
};

/**
 * @brief Defines the basic light information.
 *
 * Basic light information includes the light type and custom extended information.
 *
 * @since 3.1
 */
struct LightInfo {
    uint32_t lightType; /** Light type. For details, see {@link LightType}. */
    int32_t reserved;     /** Custom extended information. */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* LIGHT_TYPE_H */
/** @} */
