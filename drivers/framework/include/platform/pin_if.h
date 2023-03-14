/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @file pin_if.h
 *
 * @brief Declares the standard pin interface functions.
 *
 * @since 1.0
 */

#ifndef PIN_IF_H
#define PIN_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates pin pull types.
 *
 * @since 1.0
 */
enum PinPullType {
    PIN_PULL_NONE = 0,      /**< SET PIN SUSPEND. */
    PIN_PULL_UP =  1,       /**< SET PIN RESISTANCE UP. */
    PIN_PULL_DOWN = 2,      /**< SET PIN RESISTANCE DOWN. */
};
/**
 * @brief Obtains the handle of a pin.
 *
 * You must call this function before setting a pin properties.
 *
 * @param pinName Indicates the pin which you want to setting properties.
 *
 * @return Returns the pointer to the {@link DevHandle} of the pin controller which
 * to get a pin if the operation is successful;
 * returns <b>NULL</b> otherwise.
 * @since 1.0
 */
DevHandle PinGet(const char *pinName);

/**
 * @brief Releases the handle of a pin.
 *
 * If you no longer need to access the pin, you should call this function to close its handle so as
 * to release unused memory resources.
 *
 * @param handle Indicates the pointer to the device handle of the pin.
 *
 * @since 1.0
 */
void PinPut(DevHandle handle);

/**
 * @brief Set the pin pullType configuration.
 *
 * You can call this function when you need to set the pin pull configuration.
 * @param handle Indicates the pointer to the device handle of the pin.
 * @param pullType Indicates the type of pin pull.
 *
 * @return Returns <b>0</b> if set the pin Pull configuration operation is successfully;
 * Returns a negative value otherwise.
 * @since 1.0
 */
int32_t PinSetPull(DevHandle handle, enum PinPullType pullType);

/**
 * @brief Get the pin pullType configuration.
 *
 * You can call this function when you need to get the pin pull configuration.
 *
 * @param handle Indicates the pointer to the device handle of the pin.
 * @param pullType Indicates the pointer of the Pin Pull Type.
 * @return Returns <b>0</b> if get the pin Pull configuration operation is successfully;
 * Returns a negative value otherwise.
 * @since 1.0
 */
int32_t PinGetPull(DevHandle handle, enum PinPullType *pullType);

/**
 * @brief Set the pin strength configuration.
 *
 * You can call this function when you need to set the pin strength configuration.
 * @param handle Indicates the pointer to the device handle of the pin.
 * @param strength Indicates the value of pin strength.
 *
 * @return Returns <b>0</b> if set the pin strength configuration operation is successfully;
 * Returns a negative value otherwise.
 * @since 1.0
 */
int32_t PinSetStrength(DevHandle handle, uint32_t strength);

/**
 * @brief Get the pin strength configuration.
 *
 * You can call this function when you need to get the pin strength configuration.
 *
 * @param handle Indicates the pointer to the device handle of the pin.
 * @param strength Indicates the pointer of the Pin strength value.
 * @return Returns <b>0</b> if get the pin strength configuration operation is successfully;
 * Returns a negative value otherwise.
 * @since 1.0
 */
int32_t PinGetStrength(DevHandle handle, uint32_t *strength);

/**
 * @brief Set the pin function configuration.
 *
 * You can call this function when you need to set the pin function configuration.
 *
 * @param handle Indicates the pointer to the device handle of the pin description.
 * @param funcName Indicates the pointer of the pin function.
 * @return Returns <b>0</b> if set the pin function configuration operation is successfully;
 * returns a negative value otherwise.
 * @since 1.0
 */
int32_t PinSetFunc(DevHandle handle, const char *funcName);

/**
 * @brief Get the pin function configuration.
 *
 * You can call this function when you need to get the pin function configuration.
 *
 * @param handle Indicates the pointer to the device handle of the pin description.
 * @param funcName Indicates the double pointer of the pin function.
 * @return Returns <b>0</b> if get the pin function configuration operation is successfully;
 * returns a negative value otherwise.
 * @since 1.0
 */
int32_t PinGetFunc(DevHandle handle, const char **funcName);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* PIN_IF_H */
