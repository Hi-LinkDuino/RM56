/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef INPUT_GONFIG_H
#define INPUT_GONFIG_H

#include "hdf_types.h"
#include "device_resource_if.h"

#define REG_CONFIG_LEN 2

typedef struct {
    uint8_t devType;
    const char *devName;
    uint32_t resolutionX;
    uint32_t resolutionY;
} BoardAttrCfg;

typedef struct {
    uint8_t busNum;
    uint16_t clkGpio;
    uint16_t dataGpio;
    uint32_t i2cClkReg[REG_CONFIG_LEN];
    uint32_t i2cDataReg[REG_CONFIG_LEN];
} BoardI2cCfg;

typedef struct {
    uint8_t busNum;
    uint16_t clkGpio;
    uint16_t csGpio;
    uint16_t misoGpio;
    uint16_t mosiGpio;
    uint32_t spiClkReg[REG_CONFIG_LEN];
    uint32_t spiCsReg[REG_CONFIG_LEN];
    uint32_t spiMisoReg[REG_CONFIG_LEN];
    uint32_t spiMosiReg[REG_CONFIG_LEN];
} BoardSpiCfg;

typedef struct {
    uint8_t busType;
    union {
        BoardI2cCfg i2c;
        BoardSpiCfg spi;
    };
} BoardBusCfg;

typedef struct {
    uint16_t rstGpio;
    uint16_t intGpio;
    uint32_t rstPinReg[REG_CONFIG_LEN];
    uint32_t intPinReg[REG_CONFIG_LEN];
} BoardPinCfg;

typedef struct {
    uint16_t pwrType;
    uint16_t pwrNum;
    uint32_t pwrValue;
} PwrDesc;

typedef struct {
    PwrDesc vcc;
    PwrDesc vci;
} BoardPwrCfg;

typedef struct {
    uint8_t capacitanceTest;
    uint8_t gestureMode;
    uint8_t gloverMOde;
    uint8_t coverMode;
    uint8_t chargeMode;
    uint8_t knuckleMode;
} BoardFeatureCfg;

typedef struct {
    const struct DeviceResourceNode *boardNode;
    BoardAttrCfg attr;
    BoardBusCfg bus;
    BoardPinCfg pins;
    BoardPwrCfg power;
    BoardFeatureCfg feature;
} TouchBoardCfg;

typedef struct {
    uint32_t *buf;
    uint32_t count;
} SeqArray;

typedef struct {
    SeqArray pwrOn;
    SeqArray pwrOff;
    SeqArray resume;
    SeqArray suspend;
} ChipPwrSeq;

typedef struct {
    uint16_t irqFlag;
    uint8_t wordMode;    // 0, 1 , 2, 3 polar & phase
    uint8_t commMode;    // 2:DMA 1:polling  0:interrupt
    uint32_t maxSpeed;
    uint32_t csSetupTime;
    uint32_t csHoldTime;
    uint32_t csRecoveryTime;
} ChipSpiCfg;

typedef struct {
    uint16_t irqFlag;
    uint32_t commAddr;    // i2c device addr
    uint32_t maxSpeed;
} ChipI2cCfg;

typedef struct {
    uint8_t busType;
    union {
        ChipI2cCfg chipI2c;
        ChipSpiCfg chipSpi;
    };
} ChipBusCfg;

typedef struct {
    const struct DeviceResourceNode *chipNode;
    const char *chipName;
    const char *vendorName;
    const char *chipInfo;
    uint16_t chipVersion;
    ChipBusCfg bus;
    ChipPwrSeq pwrSeq;
} TouchChipCfg;

typedef struct {
    struct HdfDeviceObject *hdfKeyDev;
    const char *keyName;
    uint8_t devType;
    uint16_t gpioNum;
    uint16_t irqFlag;
    uint32_t debounceTime;
} KeyChipCfg;

typedef struct {
    struct HdfDeviceObject *hdfEncoderDev;
    uint8_t devType;
    uint16_t gpioClk;
    uint16_t gpioData;
    uint16_t gpioSW;
} EncoderCfg;

int32_t ParseTouchBoardConfig(const struct DeviceResourceNode *node, TouchBoardCfg *config);
int32_t ParseTouchChipConfig(const struct DeviceResourceNode *node, TouchChipCfg *config);
int32_t ParseKeyConfig(const struct DeviceResourceNode *node, KeyChipCfg *config);
int32_t ParseEncoderConfig(const struct DeviceResourceNode *node, EncoderCfg *config);

#endif