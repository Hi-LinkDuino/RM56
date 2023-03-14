/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ETH_CHIP_DRIVER_H
#define ETH_CHIP_DRIVER_H

#include "eth_device.h"
#include "net_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_CHIPDRIVER_COUNT 16

struct EthMacOps {
    void (*MacInit)(void);
    int32_t (*PortReset)(struct EthDevice *ethDevice);
    int32_t (*PortInit)(struct EthDevice *ethDevice);
};

struct HdfEthMacChipDriver {
    struct EthMacOps *ethMacOps;  /**< Ethernet Mac some basic methods */
};

struct HdfEthNetDeviceData {
    struct HdfEthMacChipDriver *macChipDriver;   /**< Mac ChipDriver */
};

struct HdfEthNetDeviceData *GetEthNetDeviceData(const struct NetDevice *netDev);

struct HdfEthChipDriverFactory {
    const char *driverName;
    int32_t (*InitEthDriver)(struct EthDevice *ethDevice);
    int32_t (*DeinitEthDriver)(struct EthDevice *ethDevice);
    struct HdfEthMacChipDriver *(*BuildMacDriver)(void);
    void (*ReleaseMacDriver)(struct HdfEthMacChipDriver *chipDriver);
    void (*GetMacAddr)(unsigned char *addr, int len);
};

struct HdfEthChipDriverManager {
    struct HdfEthChipDriverFactory **chipFactoryInsts;
    int32_t (*RegChipDriver)(struct HdfEthChipDriverFactory *factoryInst);
    struct HdfEthChipDriverFactory *(*GetEthChipDriverByName)(const char *name);
};

struct HdfEthChipDriverManager *HdfEthGetChipDriverMgr(void);

#ifdef __cplusplus
}
#endif

#endif /* ETH_CHIP_DRIVER_H */
