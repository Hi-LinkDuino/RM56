/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_IBUS_INTF_H
#define HDF_IBUS_INTF_H

#include "hdf_wlan_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct BusDev;
typedef void IrqHandler(void *);

struct SdioConfigInfo {
    uint32_t maxBlockNum;
    uint32_t maxBlockSize;
    uint32_t maxRequestSize;
    uint32_t funcNumSize;
    uint32_t irqCap;
    void *data;
};

union BusInfo {
    struct SdioConfigInfo sdioInfo;
};

struct BusConfig {
    uint8_t busType;
    union BusInfo busInfo;
};

struct PrivateData {
    void *data;
    const char *driverName;
    int32_t (*release)(void *data);
};

struct DevOps {
    int32_t (*getBusInfo)(struct BusDev *dev, struct BusConfig *busCfg);
    void (*deInit)(struct BusDev *dev);
    int32_t (*init)(struct BusDev *dev, const struct HdfConfigWlanBus *busCfg);

    int32_t (*readData)(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf);
    int32_t (*writeData)(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf);

    int32_t (*bulkRead)(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf, uint32_t sg_len);
    int32_t (*bulkWrite)(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf, uint32_t sg_len);

    int32_t (*readFunc0)(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf);
    int32_t (*writeFunc0)(struct BusDev *dev, uint32_t addr, uint32_t cnt, uint8_t *buf);

    int32_t (*disableBus)(struct BusDev *dev);
    int32_t (*claimIrq)(struct BusDev *dev, IrqHandler *handler, void *data);
    int32_t (*releaseIrq)(struct BusDev *dev);
    int32_t (*reset)(struct BusDev *dev);

    void (*claimHost)(struct BusDev *dev);
    void (*releaseHost)(struct BusDev *dev);
};

struct BusDev {
    void *devBase;
    struct DevOps ops;
    struct PrivateData priData;
};

struct BusDev *HdfWlanCreateBusManager(const struct HdfConfigWlanBus *busConfig);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
