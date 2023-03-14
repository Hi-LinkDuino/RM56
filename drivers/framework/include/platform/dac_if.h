/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
/**
 * @addtogroup DAC
 * @{
 *
 * @brief Provides Digital to analog converter (DAC) interfaces.
 *
 * This module allows a driver to perform operations on an DAC controller for accessing devices on the DAC channel,
 * Including creating and destroying DAC controller handles and writing data
 *
 * @since 1.0
 */
/**
 * @file dac_if.h
 *
 * @brief Declares the standard DAC interface functions.
 *
 * @since 1.0
 */

#ifndef DAC_IF_H
#define DAC_IF_H

#include "platform_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct DacIoMsg {
    uint32_t number;
    uint32_t channel;
};

 /**
 * @brief Obtains the handle of an DAC controller.
 *
 * You must call this function before accessing the DAC CHANNEL.
 *
 * @param number Indicates the DAC controller ID.
 *
 * @return Returns the pointer to the {@link DevHandle} of the DAC controller if the operation is successful;
 * returns <b>NULL</b> otherwise.
 * @since 1.0
 */
DevHandle DacOpen(uint32_t number);

 /**
 * @brief Releases the handle of an DAC controller.
 *
 * If you no longer need to access the DAC controller, you should call this function to close its handle so as
 * to release unused memory resources.
 *
 * @param handle Indicates the pointer to the device handle of the DAC controller.
 *
 * @since 1.0
 */
void DacClose(DevHandle handle);

 /**
 * @brief Start the DAC device for transmission and write the target value in the specified DAC channel.
 *
 * @param handle Indicates the pointer to the device handle of the DAC controller obtained via {@link DacOpen}.
 * @param channel represents the channel through which the DAC transmits messages.
 * @param val represents the set digital target value.
 *
 * @return Returns 0 if the operation is successful; Returns a negative value otherwise..
 *
 * @since 1.0
 */
int32_t DacWrite(DevHandle handle, uint32_t channel, uint32_t val);

/**
 * @brief Enumerates DAC I/O commands.
 *
 * @since 1.0
 */
enum DacIoCmd {
    DAC_IO_READ = 0,
    DAC_IO_OPEN,
    DAC_IO_CLOSE,
    DAC_IO_WRITE,
};
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* DAC_IF_H */
