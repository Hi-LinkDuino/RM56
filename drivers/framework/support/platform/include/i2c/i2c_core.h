/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef I2C_CORE_H
#define I2C_CORE_H

#include "hdf_base.h"
#include "i2c_if.h"
#include "osal_mutex.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define I2C_BUS_MAX 16

struct I2cCntlr;
struct I2cMethod;
struct I2cLockMethod;

struct I2cCntlr {
    struct OsalMutex lock;
    void *owner;
    int16_t busId;
    void *priv;
    const struct I2cMethod *ops;
    const struct I2cLockMethod *lockOps;
};

struct I2cMethod {
    /**
     * @brief Execute one or more I2C messages, which is implemented by a specific vendor driver.
     *
     * @param cntlr Indicates the I2C controller device.
     * @param msgs Indicates the {@link I2cMsg} message array.
     * @param count Indicates the length of the message array.
     *
     * @return Returns the number of transferred message structures if the operation is successful;
     * returns a negative value otherwise.
     * @see I2cMsg
     * @since 1.0
     */
    int32_t (*transfer)(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count);
};

struct I2cLockMethod {
    /**
     * @brief Get exclusive access to an I2C controller.
     *
     * @param cntlr Indicates the I2C controller to access.
     *
     * @return Returns 0 on success; returns a negative value otherwise.
     * @since 1.0
     */
    int32_t (*lock)(struct I2cCntlr *cntlr);
    /**
     * @brief Release exclusive access to an I2C controller.
     *
     * @param cntlr Indicates the I2C controller to release.
     *
     * @since 1.0
     */
    void (*unlock)(struct I2cCntlr *cntlr);
};

/**
 * @brief Bind to a HdfDeviceObject, and do some necessary check
 *
 * @param cntlr Indicates the I2C controller device.
 * @param device The HdfDeviceObject of this I2cCntlr.
 *
 * @return Returns 0 on success; returns a negative value otherwise.
 * @since 1.0
 */
int32_t I2cCntlrAdd(struct I2cCntlr *cntlr);

/**
 * @brief Create a new I2cCntlr struct, and bind it to a HdfDeviceObject.
 *
 * @param cntlr Indicates the I2C controller device.
 *
 * @since 1.0
 */
void I2cCntlrRemove(struct I2cCntlr *cntlr);

/**
 * @brief Find and return an i2c controller by number, with ref count.
 *
 * @param number Indicates the I2C controller to get.
 *
 * @since 1.0
 */
struct I2cCntlr *I2cCntlrGet(int16_t number);

/**
 * @brief Find and return an i2c controller by number, with ref count.
 *
 * @param number Indicates the I2C controller to get.
 *
 * @since 1.0
 */
void I2cCntlrPut(struct I2cCntlr *cntlr);

/**
 * @brief Execute one or more I2C messages.
 *
 * @param cntlr Indicates the I2C controller device.
 * @param msgs Indicates the {@link I2cMsg} message array.
 * @param count Indicates the length of the message array.
 *
 * @return Returns the number of transferred message structures if the operation is successful;
 * returns a negative value otherwise.
 * @see I2cMsg
 * @since 1.0
 */
int32_t I2cCntlrTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* I2C_CORE_H */
