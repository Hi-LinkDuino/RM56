/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HI3516_AIAO_IMPL_H
#define HI3516_AIAO_IMPL_H

#include <linux/types.h>
#include "audio_platform_base.h"

/* AIO  base address */
#define AIAO_REG_BASE            0x113b0000
#define AIAO_MAX_REG_SIZE        0x10000

#define AOP_BUFF_WPTR_REG        0x2088
#define AOP_BUFF_RPTR_REG        0x208C
#define AIP_BUFF_WPTR_REG        0x1088
#define OFFSET_MULTL  8

typedef enum {
    HI_FALSE = 0,
    HI_TRUE  = 1,
} HiBool;

typedef enum {
    AUDIO_BIT_WIDTH_8   = 0,   /* 8bit width */
    AUDIO_BIT_WIDTH_16  = 1,   /* 16bit width */
    AUDIO_BIT_WIDTH_24  = 2,   /* 24bit width */
    AUDIO_BIT_WIDTH_BUTT,
} AudioBitWidth;

typedef struct {
    bool clkSelect;
    /* 256fs */
    unsigned int mclkDiv48k256fs;
    unsigned int mclkDiv441k256fs;
    unsigned int mclkDiv32k256fs;
    /* 320fs */
    unsigned int mclkDiv48k320fs;
    unsigned int mclkDiv441k320fs;
    unsigned int mclkDiv32k320fs;
} AiaoClkInfo;

// define the union u_rx_dsp_ctrl
typedef union {
    struct {
        unsigned int reserved0  : 28;      // [27..0]
        unsigned int rxEnable   : 1;       // [28]
        unsigned int rxDisableDone : 1;    // [29]
        unsigned int reserved1  : 2;       // [31..30]
    } Bits;
    unsigned int u32;
} URxDspCtrl;

typedef union {
    struct {
        unsigned int reserved0  : 8;       // [7..0]
        unsigned int volume     : 7;       // [14..8]
        unsigned int reserved1  : 13;      // [27..15]
        unsigned int txEnable   : 1;       // [28]
        unsigned int reserved2  : 3;       // [31..29]
    } Bits;
    unsigned int u32;
} UTxDspCtrl;

// define the union u_tx_buff_rptr
typedef union {
    struct {
        unsigned int txBuffRptr : 24;  // [23..0]
        unsigned int reserved0 : 8;     // [31..24]
    } Bits;
    unsigned int u32;
} UTxBuffRptr;

typedef union {
    struct {
        unsigned int txBuffWptr : 24;  // [23..0]
        unsigned int reserved0 : 8;     // [31..24]
    } Bits;
    unsigned int u32;
} UTxBuffWptr;

typedef union {
    struct {
        unsigned int txBuffSize : 24;  // [23..0]
        unsigned int reserved0 : 8;     // [31..24]
    } Bits;
    unsigned int u32;
} UTxBuffSize;

typedef union {
    struct {
        unsigned int txTransSize : 24;  // [23..0]
        unsigned int reserved0 : 8;      // [31..24]
    } Bits;
    unsigned int u32;
} UTxTransSize;

uint32_t AiopRegCfg(const int reg, const int offset, uint32_t n);
int32_t AiaoHalSysInit(void);
int32_t AiaoClockReset(void);
uint32_t AiaoHalReadReg(uint32_t offset);
int32_t AopHalSetBuffRptr(uint32_t chnId, uint32_t value);
int32_t AopHalSetBuffWptr(uint32_t chnId, uint32_t value);
int32_t AipHalSetBuffWptr(uint32_t chnId, uint32_t value);
int32_t AipHalSetBuffRptr(uint32_t chnId, uint32_t value);
int32_t AopHalSetTxStart(uint32_t chnId, bool en);
int32_t AipHalSetRxStart(uint32_t chnId, bool en);
int32_t AopHalDevEnable(uint32_t chnId);
int32_t AiaoDeviceInit(uint32_t chnId);
int32_t AudioAoInit(const struct PlatformData *platformData);
int32_t AudioAiInit(const struct PlatformData *platformData);
int32_t AiaoSysPinMux(void);
int32_t AiaoRxIntClr(uint32_t chnId);
int32_t AiaoGetMclk(uint32_t rate, uint32_t *mclkSel);
int32_t AiaoSetSysCtlRegValue(uint32_t mclkSel, uint32_t bitWidth,
    uint32_t rate, uint32_t *clkRegVal);
#endif // __HI3516_ACODEC_H__
