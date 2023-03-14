/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef ETH_DEVICE_H
#define ETH_DEVICE_H

#include "net_device.h"
#include "hdf_device_desc.h"
#include <lwip/netif.h>

#define MAX_NAME_LENGTH 16
#define ETH_DEVICE_MAX 6

#define DELAY_TIME_LONG     3000
#define DELAY_TIME_MEDIUM   50
#define DELAY_TIME_SHORT    5
#define SLEEP_TIME_SHORT    20
#define SLEEP_TIME_MEDIUM   30
#define SLEEP_TIME_COMMON   60
#define SLEEP_TIME_LONG     250

#define MAC_ADDR_OFFSET_L8  8
#define MAC_ADDR_OFFSET_L16 16
#define MAC_ADDR_OFFSET_L24 24

struct EthDevice {
    struct NetDevice *netdev;
    struct ConfigEthDevList *config;
    const char *name;
    void *priv;
};

struct HdfConfigEthMac {
    uint32_t regBase;
    uint32_t irqVector;
    uint8_t mdioFrqDiv;
    uint8_t txBusy;
    uint32_t iobase;
    uint32_t regOffSize;
};

struct HdfConfigEthPhy {
    uint8_t phyMode;
};

struct ConfigEthDevList {
    uint8_t deviceInstId;
    uint8_t isSetDefault;
    const char *driverName;
    uint8_t hwXmitq;
    uint8_t qSize;
    uint8_t port;
    struct HdfConfigEthMac ethMac;
    struct HdfConfigEthPhy ethPhy;
};

struct EthConfig {
    struct ConfigEthDevList deviceInst[ETH_DEVICE_MAX];
    uint16_t deviceListSize;
};

struct EthDevice *CreateEthDevice(const struct ConfigEthDevList *configEthDevList);
int32_t ReleaseEthDevice(struct EthDevice *ethDevice);
int32_t GetEthIfName(const struct ConfigEthDevList *configEthDevList, char *ifName, uint32_t ifNameSize);
struct EthConfig *GetEthConfig(const struct DeviceResourceNode *node);

#endif /* ETH_DEVICE_H */
