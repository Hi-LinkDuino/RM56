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
 * After obtaining the driver object or proxy, the light service distinguishes light devices by type
 * and call related APIs to obtain light information, turn on or off a light, or set the blinking mode.
 * @since 3.1
 */

/**
 * @file Light_if.h
 *
 * @brief Declares common APIs of the light module. These APIs can be used to obtain the light type,
 * turn on or off a light, and set the light brightness and blinking mode.
 * @since 3.1
 */

#ifndef LIGHT_IF_H
#define LIGHT_IF_H

#include <stdint.h>
#include "light_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the basic operations that can be performed on lights.
 *
 * The operations include obtaining light information, turning on or off a light,
 * and setting the light brightness or blinking mode.
 */

struct LightInterface {
    /**
     * @brief Obtains information about all the lights in the system.
     *
     * @param lightInfo Indicates the double pointer to the light information. For details, see {@link LightInfo}.
     * @param count Indicates the pointer to the number of lights.
     *
     * @return Returns <b>0</b> if the operation is successful.
     * @return Returns a negative value if the operation fails.
     *
     * @since 3.1
     */
    int32_t (*GetLightInfo)(struct LightInfo **lightInfo, uint32_t *count);

    /**
     * @brief Turns on available lights in the list based on the specified light type.
     *
     * @param type Indicates the light type. For details, see {@link LightType}.
     *
     * @param effect Indicates the pointer to the lighting effect. For details, see {@link LightEffect}.
     *
     * @return Returns <b>0</b> if the operation is successful.
     * @return Returns <b>-1</b> if the light type is not supported.
     * @return Returns <b>-2</b> if the blinking setting is not supported.
     * @return Returns <b>-3</b> if the brightness setting is not supported.
     *
     * @since 3.1
     */
    int32_t (*TurnOnLight)(uint32_t type, struct LightEffect *effect);

    /**
     * @brief Turns off available lights in the list based on the specified light type.
     *
     * @param type Indicates the light type. For details, see {@link LightType}.
     *
     * @return Returns <b>0</b> if the operation is successful.
     * @return Returns a negative value if the operation fails.
     *
     * @since 3.1
     */
    int32_t (*TurnOffLight)(uint32_t type);
};

/**
 * @brief Creates a <b>LightInterface</b> instance.
 *
 * The <b>LightInterface</b> instance created can be used to perform related light control operations.
 *
 * @return Returns <b>0</b> if the operation fails.
 * @return Returns a non-zero value if the operation is successful.
 *
 * @since 3.1
 */
const struct LightInterface *NewLightInterfaceInstance(void);

/**
 * @brief Releases the <b>LightInterface</b> instance and related resources.
 *
 * @return Returns <b>0</b> if the operation is successful.
 * @return Returns a negative value if the operation fails.
 *
 * @since 3.1
 */
int32_t FreeLightInterfaceInstance(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* LIGHT_IF_H */
/** @} */
