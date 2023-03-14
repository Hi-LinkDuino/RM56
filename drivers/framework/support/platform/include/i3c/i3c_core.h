/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef I3C_CORE_H
#define I3C_CORE_H

#include "i3c_if.h"
#include "hdf_base.h"
#include "hdf_dlist.h"
#include "osal_spinlock.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define I3C_CNTLR_MAX   20
#define I3C_ADDR_MAX             127
#define I3C_IBI_MAX              10
#define ADDRS_STATUS_BITS        2
#define BITS_PER_UINT16          16
#define ADDRS_PER_UINT16         8

#define ADDR_STATUS_BIT0_MASK    0x1
#define ADDR_STATUS_BIT1_MASK    0x2
#define ADDR_STATUS_MASK         0x3


#define I3C_RESERVED_ADDR_7H00    0x00
#define I3C_RESERVED_ADDR_7H01    0x01
#define I3C_RESERVED_ADDR_7H02    0x02
#define I3C_RESERVED_ADDR_7H3E    0x3e
#define I3C_RESERVED_ADDR_7H5E    0x5e
#define I3C_RESERVED_ADDR_7H6E    0x6e
#define I3C_RESERVED_ADDR_7H76    0x76
#define I3C_RESERVED_ADDR_7H78    0x78
#define I3C_RESERVED_ADDR_7H79    0x79
#define I3C_RESERVED_ADDR_7H7A    0x7a
#define I3C_RESERVED_ADDR_7H7B    0x7b
#define I3C_RESERVED_ADDR_7H7C    0x7c
#define I3C_RESERVED_ADDR_7H7D    0x7d
#define I3C_RESERVED_ADDR_7H7E    0x7e
#define I3C_RESERVED_ADDR_7H7F    0x7f

#define I3C_HOT_JOIN_ADDR   I3C_RESERVED_ADDR_7H02
#define I3C_BROADCAST_ADDR  I3C_RESERVED_ADDR_7H7E

struct I3cCntlr;
struct I3cMethod;
struct I3cLockMethod;
struct I3cDevice;
struct I3cVendor;

enum I3cDeviceType {
    /** I2C device */
    I3C_CNTLR_I2C_DEVICE = 0,
    /** legacy I2C device */
    I3C_CNTLR_I2C_LEGACY_DEVICE,
    /** I3C device */
    I3C_CNTLR_I3C_DEVICE,
};

struct I3cCntlr {
    OsalSpinlock lock;
    void *owner;
    int16_t busId;
    struct I3cConfig config;
    uint16_t addrSlot[(I3C_ADDR_MAX + 1) / ADDRS_PER_UINT16];
    struct I3cIbiInfo *ibiSlot[I3C_IBI_MAX];
    const struct I3cMethod *ops;
    const struct I3cLockMethod *lockOps;
    void *priv;
};

/* vendor information of I3C device */
struct I3cVendor {
    uint32_t vendorMipiId;
    uint32_t vendorVersionId;
    uint32_t vendorProductId;
};

/* In-bind Interrupt information */
struct I3cIbiInfo {
    I3cIbiFunc ibiFunc;
    uint32_t payload;
    uint8_t *data;
};

struct I3cDevice {
    struct DListHead list;
    struct I3cCntlr *cntlr;
    uint16_t bcr;
    uint16_t dcr;
    uint64_t pid;
    uint16_t addr;
    enum I3cDeviceType type;
    uint16_t dynaAddr;
    uint16_t supportIbi;
    struct I3cIbiInfo *ibi;
    struct I3cVendor vendor;
    void *devPriv;
};

struct I3cMethod {
    /**
     * @brief Send a CCC(Common Command Code), which is implemented by a specific vendor driver.
     *
     * @param cntlr Indicates the I3C controller device.
     * @param ccc Indicates the {@link I3cCccCmd}(Common Command Code) structure.
     *
     * @return Returns <b>0</b> on success; Returns a negative value otherwise.
     * @since 1.0
     */
    int32_t (*sendCccCmd)(struct I3cCntlr *cntlr, struct I3cCccCmd *ccc);

    /**
     * @brief Execute one or more I3C messages in I3C mode, which is implemented by a specific vendor driver.
     *
     * @param cntlr Indicates the I3C controller device.
     * @param msgs Indicates the {@link I3cMsg} message array.
     * @param count Indicates the length of the message array.
     *
     * @return Returns the number of transferred message structures on success; Returns a negative value otherwise.
     *
     * @see I3cMsg
     * @since 1.0
     */
    int32_t (*Transfer)(struct I3cCntlr *cntlr, struct I3cMsg *msgs, int16_t count);

    /**
     * @brief Execute one or more I3C messages in I2C mode, which is implemented by a specific vendor driver.
     *
     * @param cntlr Indicates the I3C controller device.
     * @param msgs Indicates the {@link I3cMsg} message array.
     * @param count Indicates the length of the message array.
     *
     * @return Returns the number of transferred message structures on success; Returns a negative value otherwise.
     *
     * @see I3cMsg
     * @since 1.0
     */
    int32_t (*i2cTransfer)(struct I3cCntlr *cntlr, struct I3cMsg *msgs, int16_t count);

    /**
     * @brief Set configurations of an I3C controller.
     *
     * @param cntlr Indicates the I3C controller to set.
     * @param config Indicates the pointer of configuration structure.
     *
     * @return Returns <b>0</b> on success; Returns a negative value otherwise.
     * @since 1.0
     */
    int32_t (*setConfig)(struct I3cCntlr *cntlr, struct I3cConfig *config);

    /**
     * @brief Get configurations of an I3C controller.
     *
     * @param cntlr Indicates the I3C controller to get configurations.
     * @param config Indicates the pointer of configuration structure.
     *
     * @return Returns <b>0</b> on success; Returns a negative value otherwise.
     * @since 1.0
     */
    int32_t (*getConfig)(struct I3cCntlr *cntlr, struct I3cConfig *config);

    /**
     * @brief Requeset an IBI(in-bind interrupt) for an I3C device which is supported.
     *
     * @param device Indicates the I3C device.
     *
     * @return Returns <b>0</b> on success; Returns a negative value otherwise.
     * @since 1.0
     */
    int32_t (*requestIbi)(struct I3cDevice *dev);

    /**
     * @brief Free an IBI(in-bind interrupt).
     *
     * @param device Indicates the I3C device.
     *
     * @return Returns <b>0</b> on success; Returns a negative value otherwise.
     * @since 1.0
     */
    void (*freeIbi)(struct I3cDevice *dev);
};

struct I3cLockMethod {
    /**
     * @brief Get exclusive access to an I3C controller.
     *
     * @param cntlr Indicates the I3C controller to access.
     *
     * @return Returns <b>0</b> on success; Returns a negative value otherwise.
     * @since 1.0
     */
    int32_t (*lock)(struct I3cCntlr *cntlr);
    /**
     * @brief Release exclusive access to an I3C controller.
     *
     * @param cntlr Indicates the I3C controller to release.
     *
     * @since 1.0
     */
    void (*unlock)(struct I3cCntlr *cntlr);
};

enum I3C_ERROR_CODE {
    I3C_ERROR_UNKNOWN = 0,
    I3C_ERROR_M0,
    I3C_ERROR_M1,
    I3C_ERROR_M2,
    I3C_ERROR_S0,
    I3C_ERROR_S1,
    I3C_ERROR_S2,
    I3C_ERROR_S3,
    I3C_ERROR_S4,
    I3C_ERROR_S5,
    I3C_ERROR_S6,
};

enum I3cAddrStatus {
    I3C_ADDR_FREE = 0,
    I3C_ADDR_RESERVED,
    I3C_ADDR_I2C_DEVICE,
    I3C_ADDR_I3C_DEVICE,
};

enum I3cDeviceSupportIbi {
    I3C_DEVICE_NOT_SUPPORT_IBI = 0,
    I3C_DEVICE_SUPPORT_IBI,
};

/**
 * @brief Set the config of a I3C controller.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param config Indicates the pointer of the configuration structure.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cCntlrSetConfig(struct I3cCntlr *cntlr, struct I3cConfig *config);

/**
 * @brief Get the config of a I3C controller.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param config Indicates the pointer of the configuration structure.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cCntlrGetConfig(struct I3cCntlr *cntlr, struct I3cConfig *config);

/**
 * @brief Requeset an IBI(in-bind interrupt) for an I3C device which is supported.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param addr Indicates the address of device to requeset IBI(In-Band Interrupt).
 * @param func Indicates the call back function of the IBI.
 * @param payload Indicates the length of payload data, in bytes.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cCntlrRequestIbi(struct I3cCntlr *cntlr, uint16_t addr, I3cIbiFunc func, uint32_t payload);

/**
 * @brief Free the IBI(in-bind interrupt) which is Requeseted by {@link I3cCntlrRequestIbi}.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param addr Indicates the address of device to requeset IBI(In-Band Interrupt).
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cCntlrFreeIbi(struct I3cCntlr *cntlr, uint16_t addr);

/**
 * @brief Send a CCC (Common Command Code) to an I3C device which is supported.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param ccc The pointer of CCC (Common Command Code) structure to send.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cCntlrSendCccCmd(struct I3cCntlr *cntlr, struct I3cCccCmd *ccc);

/**
 * @brief Execute one or more I3C messages in I2C mode.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param msgs Indicates the {@link I3cMsg} message array.
 * @param count Indicates the length of the message array.
 *
 * @return Returns the number of transferred message structures on success; Returns a negative value otherwise.
 *
 * @see I3cMsg
 * @since 1.0
 */
int32_t I3cCntlrI2cTransfer(struct I3cCntlr *cntlr, struct I3cMsg *msgs, int16_t count);

/**
 * @brief Execute one or more I3C messages in I3C mode.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param msgs Indicates the {@link I3cMsg} message array.
 * @param count Indicates the length of the message array.
 *
 * @return Returns the number of transferred message structures on success; Returns a negative value otherwise.
 *
 * @see I3cMsg
 * @since 1.0
 */
int32_t I3cCntlrTransfer(struct I3cCntlr *cntlr, struct I3cMsg *msgs, int16_t count);

/**
 * @brief Add an I3C device or an I2C device to device list.
 *
 * @param device Indicates the I3C device or I2C device.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cDeviceAdd(struct I3cDevice *device);

/**
 * @brief Remove an I3C device or an I2C device from device list.
 *
 * @param device Indicates the I3C device or I2C device.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
void I3cDeviceRemove(struct I3cDevice *device);

/**
 * @brief Find and return an I3C controller by number, with ref count.
 *
 * @param number Indicates the I3C controller to get.
 *
 * @return Returns an I3C controller object on success; Returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
struct I3cCntlr *I3cCntlrGet(int16_t number);

/**
 * @brief Release an I3C controller obtained by {@link I3cCntlrGet}.
 *
 * @param number Indicates the I3C controller to get.
 *
 * @since 1.0
 */
void I3cCntlrPut(struct I3cCntlr *cntlr);

/**
 * @brief Add an I3C controller to controller list.
 *
 * @param cntlr Indicates the I3C controller device.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
int32_t I3cCntlrAdd(struct I3cCntlr *cntlr);

/**
 * @brief Remove an I3C controller from controller list.
 *
 * @param cntlr Indicates the I3C controller device.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
void I3cCntlrRemove(struct I3cCntlr *cntlr);

/**
 * @brief Get an I3C device by addr.
 *
 * @param cntlr Indicates the I3C controller device.
 * @param addr Indicates the address of the device which you would like to get.
 *
 * @return Returns <b>0</b> on success; Returns a negative value otherwise.
 * @since 1.0
 */
struct I3cDevice *GetDeviceByAddr(struct I3cCntlr *cntlr, uint16_t addr);

/**
 * @brief IBI(In-bind Interrupt) callback function.
 *
 * @param device Indicates the device that generated the IBI.
 *
 * @return Returns an I3C device object on success; Returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
int32_t I3cCntlrIbiCallback(struct I3cDevice *device);

enum I3cIoCmd {
    I3C_IO_I2C_TRANSFER = 0,
    I3C_IO_PRIV_TRANSFER,
    I3C_IO_OPEN,
    I3C_IO_CLOSE,
    I3C_IO_GET_CONFIG,
    I3C_IO_SET_CONFIG,
    I3C_IO_REQUEST_IBI,
    I3C_IO_FREE_IBI,
};

int I3cCheckReservedAddr(uint16_t addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* I3C_CORE_H */
