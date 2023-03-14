/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
#ifndef HDF_WLAN_CONFIG_H
#define HDF_WLAN_CONFIG_H

#include "device_resource_if.h"
#define WLAN_DEVICE_MAX 3
#define WLAN_MAX_CHIP_NUM 3
#define BUS_FUNC_MAX 1
#define CHIP_SDIO_DEVICE_ID_COUNT 1

struct HdfConfigWlanStation {
    const char *name;
    uint8_t mode;
};

struct HdfConfigWlanHostAp {
    const char *name;
    uint8_t mode;
    uint8_t vapResNum;
    uint8_t userResNum;
};

struct HdfConfigWlanP2P {
    const char *name;
    uint8_t mode;
};

struct HdfConfigWlanMac80211 {
    uint8_t mode;
};

struct HdfConfigWlanPhy {
    uint8_t mode;
};

struct HdfConfigWlanModuleConfig {
    uint32_t featureMap;
    const char *msgName;
    struct HdfConfigWlanStation station;
    struct HdfConfigWlanHostAp hostAp;
    struct HdfConfigWlanP2P p2p;
    struct HdfConfigWlanMac80211 mac80211;
    struct HdfConfigWlanPhy Phy;
};

/* ----------------------------------------------*
 * deviceList architecture                           *
 * ---------------------------------------------- */
struct HdfConfigWlanPower {
    uint8_t powerSeqDelay;
    uint8_t powerType;
    uint8_t gpioId;
    uint8_t activeLevel;
};


struct HdfConfWlanPowers {
    uint8_t powerCount;
    struct HdfConfigWlanPower power0;
    struct HdfConfigWlanPower power1;
};

struct HdfConfWlanRest {
    uint8_t resetType;
    uint8_t gpioId;
    uint8_t activeLevel;
    uint8_t resetHoldTime;
};

struct HdfConfigWlanBus {
    uint8_t busType;
    uint8_t busIdx;
    uint8_t funcNum[BUS_FUNC_MAX];
    uint32_t funcNumSize;
    uint16_t timeout;
    uint16_t blockSize;
};

struct HdfConfigWlanDevInst {
    uint8_t deviceInstId;
    struct HdfConfWlanPowers powers;
    struct HdfConfWlanRest reset;
    struct HdfConfigWlanBus bus;
    uint8_t bootUpTimeOut;
};

struct HdfConfigWlanDeviceList {
    struct HdfConfigWlanDevInst deviceInst[WLAN_DEVICE_MAX];
    uint16_t deviceListSize;
};
/* ----------------------------------------------*
 * chip config                             *
 * ---------------------------------------------- */
struct HdfConfWlanSdioArgs {
    uint16_t vendorId;
    uint16_t deviceId[1];
};
struct HdfConfigWlanChipInst {
    const char *driverName;
    struct HdfConfWlanSdioArgs chipSdio;
};
struct HdfConfigWlanChipList {
    struct HdfConfigWlanChipInst chipInst[WLAN_MAX_CHIP_NUM];
    uint16_t chipInstSize;
};

struct HdfConfigWlanConfig {
    const char* hostChipName;
    struct HdfConfigWlanModuleConfig moduleConfig;
    struct HdfConfigWlanDeviceList deviceList;
    struct HdfConfigWlanChipList chipList;
};
struct HdfConfigWlanRoot {
    struct HdfConfigWlanConfig wlanConfig;
};

struct HdfConfigWlanRoot *HdfWlanGetModuleConfigRoot(void);
int32_t HdfParseWlanConfig(const struct DeviceResourceNode *node);

#endif