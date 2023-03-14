/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */


#ifndef REGULATOR_IF_H
#define REGULATOR_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* regulator status on or off */
enum RegulatorStatus {
    REGULATOR_STATUS_ON = 1,
    REGULATOR_STATUS_OFF,
};

/* regulator mode, set voltage or current */
enum RegulatorChangeMode {
    REGULATOR_CHANGE_VOLTAGE = 1,
    REGULATOR_CHANGE_CURRENT,
};
/**
 * @brief Gets a regulator.
 *
 * This function must be called to get its device handle before operating the regulator.
 *
 * @param name Indicates regulator name.
 *
 * @return If the operation is successful, a pointer to the regulator device handle is returned.
 *
 * @since 1.0
 */
DevHandle RegulatorOpen(const char *name);
/**
 * @brief Releases a regulator.
 *
 * If you no longer need the regulator, call this function to  release it
 *
 * @param handle Represents a pointer to the regulator device handle.
 *
 * @since 1.0
 */
void RegulatorClose(DevHandle handle);
/**
 * @brief Enables a regulator.
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @return <b>0</b> If the regulator enables successfully; Otherwise, a negative value is returned.
 *
 * @attention That if the regulator has been enabled before calling this function, calling this function will succeed.
 *
 * @since 1.0
 */
int32_t RegulatorEnable(DevHandle handle);
/**
 * @brief Disable a regulator.
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @return <b>0</b> If the regulator disable successfully; Otherwise, a negative value is returned.
 *
 * @attention If the regulator status alwayson is true or there is regulator child  not disable, disabling  fail
 *
 * @since 1.0
 */
int32_t RegulatorDisable(DevHandle handle);
/**
 * @brief Force disable a regulator.
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @return <b>0</b> If the regulator disable successfully; Otherwise, a negative value is returned.
 *
 * @attention No matter whether the status of the regulator is alwayson or the status of the child is enable,
 *            the regulator is disabled.
 *
 * @since 1.0
 */
int32_t RegulatorForceDisable(DevHandle handle);
/**
 * @brief Set the output voltage range of a regulator.
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @param minUv  Represents minimum voltage
 * @param minUv  Represents maximum voltage
 * @return <b>0</b> If the regulator setVoltage successfully; Otherwise, a negative value is returned.
 *
 * @attention If the set voltage is not within the contrants, the setting fails.
 *
 * @since 1.0
 */
int32_t RegulatorSetVoltage(DevHandle handle, uint32_t minUv, uint32_t maxUv);
/**
 * @brief Get a regulator voltage.
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @param voltage Voltage obtained.
 * @return <b>0</b> If the regulator get voltage successfully; Otherwise, a negative value is returned.
 *
 * @since 1.0
 */
int32_t RegulatorGetVoltage(DevHandle handle, uint32_t *voltage);
/**
 * @brief Set regulator current range
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @param minUa Minimum  current
 * @param maxUa Maximum  current
 * @return <b>0</b> If the regulator set current range successfully; Otherwise, a negative value is returned.
 *
 * @attention If the setting range exceeds the limit, the setting fails
 *
 * @since 1.0
 */
int32_t RegulatorSetCurrent(DevHandle handle, uint32_t minUa, uint32_t maxUa);
/**
 * @brief Get a regulator current
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @param voltage Current obtained
 * @return <b>0</b> If the regulator getCurrent successfully; Otherwise, a negative value is returned.
 *
 * @since 1.0
 */
int32_t RegulatorGetCurrent(DevHandle handle, uint32_t *regCurrent);
/**
 * @brief Get a regulator status
 *
 * @param handle  Represents a pointer to the regulator handle, which is obtained through {@ link RegulatorOpen}.
 * @param status Status obtained, enable or disable
 * @return <b>0</b> If the regulator get status successfully; Otherwise, a negative value is returned.
 *
 * @since 1.0
 */
int32_t RegulatorGetStatus(DevHandle handle, uint32_t *status);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* REGULATOR_IF_H */
/** @} */
