/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hi3516_aiao_impl.h"
#include <asm/io.h>
#include "hdf_base.h"
#include "audio_driver_log.h"
#include "audio_dai_base.h"
#include "osal_io.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

#define PERI_CRG103             0x1201019c // 0x12010000 + 0x019c

#define AOP_BUFF_SADDR_REG      0x2080
#define AOP_BUFF_SIZE_REG       0x2084
#define AOP_TRANS_SIZE_REG      0x2094
#define AOP_INF_ATTRI_REG       0x2000
#define AOP_CTRL_REG            0x2004
#define AIP_BUFF_SADDR_REG      0x1080
#define AIP_BUFF_SIZE_REG       0x1084
#define AIP_BUFF_RPTR_REG       0x108C
#define AIP_TRANS_SIZE_REG      0x1094
#define AIP_INF_ATTRI_REG       0x1000
#define AIP_CTRL_REG            0x1004
#define RX_INT_ENA              0x10a0
#define AIAO_INT_ENA            0x0000
#define RX_INT_CLR              0x10AC

#define GPIO_BASE1 0x2010
#define GPIO_BASE2 0x2400
#define GPIO_BASE3 0x2010

#define GPIO_BASE2_VAL 0x000000ff
#define GPIO_BASE3_VAL 0x00000000

#define IOCFG2_BASE_ADDR 0x112F0000
#define IOCFG3_BASE_ADDR 0x10FF0000
#define GPIO_BASE_ADDR 0x120D0000
#define BASE_ADDR_REMAP_SIZE 0x10000

#define I2S_IOCFG3_BASE1 0x44
#define I2S_IOCFG3_BASE1_VAL 0x0600

static void     *g_regAiaoBase = NULL;   // AIAO Reg Base Addr
static const int g_hiAiDevMaxNum = 3;
static const int g_hiAoDevMaxNum = 3;
static const int g_wChannelIdMax = 13;

static const AiaoClkInfo g_aioClkInfo = {
    .clkSelect        = 0,          /* 0 is Audio base clk; 1 is Audio spare clk. */
    .mclkDiv48k256fs  = 0x00152EF0, /* 48k * 256 */
    .mclkDiv441k256fs = 0x00137653,  /* 44.1k * 256 */
    .mclkDiv32k256fs  = 0x000E1F4B,  /* 32k * 256 */
    .mclkDiv48k320fs  = 0x001A7AAC,  /* 48k * 320 */
    .mclkDiv441k320fs = 0x00185FA0,  /* 44.1k * 320 */
    .mclkDiv32k320fs  = 0x0011A71E,  /* 32k * 320 */
};

uint32_t AiopRegCfg(const int reg, const int offset, uint32_t n)
{
    return reg + offset * n;
}

/* Mapping physical address to  virtual address, for acodec and aiao */
int32_t AiaoHalSysInit(void)
{
    // AIAO REMAP
    if (g_regAiaoBase == NULL) {
        g_regAiaoBase = OsalIoRemap(AIAO_REG_BASE, AIAO_MAX_REG_SIZE);
        if (g_regAiaoBase == NULL) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int32_t AiaoClockReset(void)
{
    volatile const uint32_t crgVal = 0xa; // AIAO CLOCK RESET

    /* RESET AIAO */
    volatile char *vAddr = OsalIoRemap(PERI_CRG103, sizeof(uint32_t));
    if (vAddr == NULL) {
        AUDIO_DEVICE_LOG_ERR("vAddr is null \n");
        return HDF_FAILURE;
    }

    OSAL_WRITEL(crgVal, vAddr);
    OsalIoUnmap((void *)((uintptr_t)(void*)vAddr));
    return HDF_SUCCESS;
}

uint32_t AiaoHalReadReg(uint32_t offset)
{
    if (g_regAiaoBase == NULL) {
        AUDIO_DEVICE_LOG_ERR("g_regAiaoBase is null.\n");
        return 0x0;
    }

    return (*(volatile uint32_t *)((unsigned char *)g_regAiaoBase + (uint32_t)offset));
}

static int32_t AiaoGetBclkSel(const uint32_t bclkDiv, uint32_t *bclkSel)
{
    const int bclkDivReg[12][2] = {
        {1, 0x00}, {2, 0x02}, {3, 0x01},
        {4, 0x03}, {6, 0x04}, {8, 0x05},
        {12, 0x06}, {16, 0x07}, {24, 0x08},
        {32, 0x09}, {48, 0x0a}, {64, 0x0b},
    }; // AIO_MCLK_TO_BCLK
    if (bclkSel == NULL) {
        AUDIO_DEVICE_LOG_ERR("bclkSel is null.");
        return HDF_FAILURE;
    }

    for (int32_t num = 0; num < sizeof(bclkDivReg) / sizeof(bclkDivReg[0]); num++) {
        if (bclkDivReg[num][0] == bclkDiv) {
            *bclkSel = bclkDivReg[num][1];
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

static int32_t AiaoGetLrclkSel(const uint32_t lrclkDiv, uint32_t *lrclkSel)
{
    const int lrclkDivReg[6][2] = {
        {16, 0x00}, {32, 0x01}, {48, 0x02},
        {64, 0x03}, {128, 0x04}, {256, 0x05},
    }; // AIO_BCLK_TO_FSCLK
    if (lrclkSel == NULL) {
        AUDIO_DEVICE_LOG_ERR("lrclkSel is null.\n");
        return HDF_FAILURE;
    }

    for (int32_t num = 0; num < sizeof(lrclkDivReg) / sizeof(lrclkDivReg[0]); num++) {
        if (lrclkDivReg[num][0] == lrclkDiv) {
            *lrclkSel = lrclkDivReg[num][1];
            return HDF_SUCCESS;
        }
    }
    return HDF_FAILURE;
}

void AiaoHalWriteReg(uint32_t offset, uint32_t value)
{
    if (g_regAiaoBase == NULL) {
        AUDIO_DEVICE_LOG_ERR("g_aio_base is null.\n");
        return;
    }

    *(volatile  uint32_t *)((unsigned char *)(g_regAiaoBase) + (uint32_t)(offset)) = value;
}

int32_t AiaoSysPinMux(void)
{
    void *regIocfg3Base = OsalIoRemap(IOCFG3_BASE_ADDR, BASE_ADDR_REMAP_SIZE);
    if (regIocfg3Base == NULL) {
        AUDIO_DEVICE_LOG_ERR("regIocfg3Base is NULL.");
        return HDF_FAILURE;
    }

    void *regGpioBase = OsalIoRemap(GPIO_BASE_ADDR, BASE_ADDR_REMAP_SIZE);
    if (regGpioBase == NULL) {
        OsalIoUnmap(regIocfg3Base);
        AUDIO_DEVICE_LOG_ERR("regGpioBase is NULL.");
        return HDF_FAILURE;
    }

    AUDIO_DEVICE_LOG_DEBUG("I2s0PinMuxAmpUnmute AmpUnmute");
    SysWritel((uintptr_t)regIocfg3Base + I2S_IOCFG3_BASE1, I2S_IOCFG3_BASE1_VAL);
    SysWritel((uintptr_t)regGpioBase + GPIO_BASE1, GPIO_BASE3_VAL);
    SysWritel((uintptr_t)regGpioBase + GPIO_BASE2, GPIO_BASE2_VAL);
    SysWritel((uintptr_t)regGpioBase + GPIO_BASE3, GPIO_BASE3_VAL);

    OsalIoUnmap(regGpioBase);
    OsalIoUnmap(regIocfg3Base);

    return HDF_SUCCESS;
}

static int32_t AipHalSetBufferAddr(uint32_t chnId, uint64_t value)
{
    uint32_t saddr;

    if (chnId >= g_wChannelIdMax) {
        AUDIO_DEVICE_LOG_ERR("ai_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    saddr = (uint32_t)(value & 0xffffffff);
    AiaoHalWriteReg(AiopRegCfg(AIP_BUFF_SADDR_REG, OFFSET_MULTL, chnId), saddr); // buf start

    return HDF_SUCCESS;
}

int32_t AipHalSetBuffWptr(uint32_t chnId, uint32_t value)
{
    UTxBuffWptr unTmp;

    if (chnId >= g_wChannelIdMax) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }
    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AIP_BUFF_WPTR_REG, OFFSET_MULTL, chnId));
    unTmp.Bits.txBuffWptr = value;
    AiaoHalWriteReg(AiopRegCfg(AIP_BUFF_WPTR_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

int32_t AipHalSetBuffRptr(uint32_t chnId, uint32_t value)
{
    UTxBuffRptr unTmp;

    if (chnId >= g_hiAiDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("ai_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AIP_BUFF_RPTR_REG, OFFSET_MULTL, chnId));
    if (unTmp.Bits.txBuffRptr != value) {
        unTmp.Bits.txBuffRptr = value;
        AiaoHalWriteReg(AiopRegCfg(AIP_BUFF_RPTR_REG, OFFSET_MULTL, chnId), unTmp.u32);
    }

    return HDF_SUCCESS;
}

static int32_t AipHalSetBufferSize(uint32_t chnId, uint32_t value)
{
    UTxBuffSize unTmp;

    if (chnId >= g_hiAoDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AIP_BUFF_SIZE_REG, OFFSET_MULTL, chnId));
    unTmp.Bits.txBuffSize = value;
    AiaoHalWriteReg(AiopRegCfg(AIP_BUFF_SIZE_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

static int32_t AipHalSetTransSize(uint32_t chnId, uint32_t value)
{
    UTxTransSize unTmp;

    if (chnId >= g_hiAoDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("ai_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AIP_TRANS_SIZE_REG, OFFSET_MULTL, chnId));
    unTmp.Bits.txTransSize = value;
    AiaoHalWriteReg(AiopRegCfg(AIP_TRANS_SIZE_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

int32_t AipHalSetRxStart(uint32_t chnId, bool en)
{
    URxDspCtrl unTmp;

    if (chnId >= g_hiAiDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("AipHalSetRxStart chnId %d is invalid!\n", chnId);
        return -1;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AIP_CTRL_REG, OFFSET_MULTL, chnId));

    unTmp.Bits.rxDisableDone = 0;
    unTmp.Bits.rxEnable = en;

    AiaoHalWriteReg(AiopRegCfg(AIP_CTRL_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return 0;
}

static void AopSetCtrlReg(uint32_t chnId)
{
    UTxDspCtrl aopCtrlReg;
    aopCtrlReg.Bits.reserved0 = 0;
    aopCtrlReg.Bits.volume = 0x79; /* 0db */
    aopCtrlReg.Bits.txEnable = 0;
    aopCtrlReg.Bits.reserved1 = 0;
    aopCtrlReg.Bits.reserved2 = 0;

    AiaoHalWriteReg(AiopRegCfg(AOP_CTRL_REG, OFFSET_MULTL, chnId), aopCtrlReg.u32);
    AUDIO_DEVICE_LOG_DEBUG("[OHOS] AopSetCtrlReg AopSetAttrReg write = %08x\n", aopCtrlReg.u32);
}

static int32_t AopHalSetBufferAddr(uint32_t chnId, uint64_t value)
{
    uint32_t saddr;

    if (chnId >=  g_wChannelIdMax) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    saddr = (uint32_t)(value & 0xffffffff);
    AiaoHalWriteReg(AiopRegCfg(AOP_BUFF_SADDR_REG, OFFSET_MULTL, chnId), saddr); // buf start

    return HDF_SUCCESS;
}

int32_t AopHalSetBuffWptr(uint32_t chnId, uint32_t value)
{
    UTxBuffWptr unTmp;

    if (chnId >= g_wChannelIdMax) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AOP_BUFF_WPTR_REG, OFFSET_MULTL, chnId));
    unTmp.Bits.txBuffWptr = value;
    AiaoHalWriteReg(AiopRegCfg(AOP_BUFF_WPTR_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

int32_t AopHalSetBuffRptr(uint32_t chnId, uint32_t value)
{
    UTxBuffRptr unTmp;

    if (chnId >= g_wChannelIdMax) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AOP_BUFF_RPTR_REG, OFFSET_MULTL, chnId));

    unTmp.Bits.txBuffRptr = value;
    AiaoHalWriteReg(AiopRegCfg(AOP_BUFF_RPTR_REG, OFFSET_MULTL, chnId), unTmp.u32);
    return HDF_SUCCESS;
}

static int32_t AopHalSetBufferSize(uint32_t chnId, uint32_t value)
{
    UTxBuffSize unTmp;

    if (chnId >= g_hiAoDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AOP_BUFF_SIZE_REG, OFFSET_MULTL, chnId));

    unTmp.Bits.txBuffSize = value;
    AiaoHalWriteReg(AiopRegCfg(AOP_BUFF_SIZE_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

static int32_t AopHalSetTransSize(uint32_t chnId, uint32_t value)
{
    UTxTransSize unTmp;

    if (chnId >= g_hiAoDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AOP_TRANS_SIZE_REG, OFFSET_MULTL, chnId));

    unTmp.Bits.txTransSize = value;
    AiaoHalWriteReg(AiopRegCfg(AOP_TRANS_SIZE_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

int32_t AopHalSetTxStart(uint32_t chnId, bool en)
{
    UTxDspCtrl unTmp;

    if (chnId >= g_hiAoDevMaxNum) {
        AUDIO_DEVICE_LOG_ERR("ao_dev%d is invalid!\n", chnId);
        return HDF_FAILURE;
    }

    unTmp.u32 = AiaoHalReadReg(AiopRegCfg(AOP_CTRL_REG, OFFSET_MULTL, chnId));
    unTmp.Bits.txEnable = en;

    AiaoHalWriteReg(AiopRegCfg(AOP_CTRL_REG, OFFSET_MULTL, chnId), unTmp.u32);

    return HDF_SUCCESS;
}

int32_t AopHalDevEnable(uint32_t chnId)
{
    int ret = AopHalSetTxStart(chnId, HI_TRUE);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t AiaoGetBclkFsclk(const uint32_t fsBit, const uint32_t rate,
    const int32_t mclkSel, int32_t *bclkSel, int32_t *lrclkSel)
{
    int32_t ret;
    uint32_t mclkRateNum;
    const int aioFifoBitWidth256 = 256; // 256 is aiao fifo bit width
    const int aioFifoBitWidth320 = 320; // 320 is aiao fifo bit width

    if (g_aioClkInfo.mclkDiv48k256fs == mclkSel) {
        mclkRateNum = AUDIO_SAMPLE_RATE_48000 * aioFifoBitWidth256;
    } else if (g_aioClkInfo.mclkDiv32k256fs == mclkSel) {
        mclkRateNum = AUDIO_SAMPLE_RATE_32000 * aioFifoBitWidth256;
    } else if (g_aioClkInfo.mclkDiv441k256fs == mclkSel) {
        mclkRateNum = AUDIO_SAMPLE_RATE_44100 * aioFifoBitWidth256;
    } else if (g_aioClkInfo.mclkDiv48k320fs == mclkSel) {
        mclkRateNum = AUDIO_SAMPLE_RATE_48000 * aioFifoBitWidth320;
    } else if (g_aioClkInfo.mclkDiv32k320fs == mclkSel) {
        mclkRateNum = AUDIO_SAMPLE_RATE_32000 * aioFifoBitWidth320;
    } else if (g_aioClkInfo.mclkDiv441k320fs == mclkSel) {
        mclkRateNum = AUDIO_SAMPLE_RATE_44100 * aioFifoBitWidth320;
    } else {
        AUDIO_DEVICE_LOG_ERR("not support this mclk\n");
        return HDF_FAILURE;
    }

    if ((mclkRateNum % (fsBit * rate)) != 0) {
        AUDIO_DEVICE_LOG_ERR("can not get the bclkDivision ratio\n");
        return HDF_FAILURE;
    }

    ret = AiaoGetBclkSel(mclkRateNum / (fsBit * rate), bclkSel);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AiaoGetBclkSel error\n");
        return ret;
    }

    ret = AiaoGetLrclkSel(fsBit, lrclkSel);

    return ret;
}

int32_t AiaoGetMclk(uint32_t rate, uint32_t *mclkSel)
{
    if (mclkSel == NULL) {
        AUDIO_DEVICE_LOG_ERR("mclkSel is null");
        return HDF_FAILURE;
    }
    switch (rate) {
        case AUDIO_SAMPLE_RATE_8000:
        case AUDIO_SAMPLE_RATE_16000:
        case AUDIO_SAMPLE_RATE_32000:
        case AUDIO_SAMPLE_RATE_64000: {
            *mclkSel = g_aioClkInfo.mclkDiv32k256fs;
            break;
        }

        case AUDIO_SAMPLE_RATE_12000:
        case AUDIO_SAMPLE_RATE_24000:
        case AUDIO_SAMPLE_RATE_48000:
        case AUDIO_SAMPLE_RATE_96000: {
            *mclkSel = g_aioClkInfo.mclkDiv48k256fs;
            break;
        }

        case AUDIO_SAMPLE_RATE_11025:
        case AUDIO_SAMPLE_RATE_22050:
        case AUDIO_SAMPLE_RATE_44100: {
            *mclkSel = g_aioClkInfo.mclkDiv441k256fs;
            break;
        }

        default: {
            AUDIO_DEVICE_LOG_ERR("not support this sample rate\n");
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

static uint32_t AiaoGetBitCnt(uint32_t bitWidth)
{
    if (bitWidth == BIT_WIDTH8) {
        return BIT_WIDTH8; /* 8:8bit */
    } else if (bitWidth == BIT_WIDTH16) {
        return BIT_WIDTH16; /* 16:16bit */
    } else {
        return BIT_WIDTH32; /* 32:32bit */
    }
}

int32_t AiaoSetSysCtlRegValue(uint32_t mclkSel, uint32_t bitWidth, uint32_t rate, uint32_t *clkRegVal)
{
    int32_t ret;
    uint32_t fsBit;
    uint32_t bclkSel;
    uint32_t lrClkSel;

    if (clkRegVal == NULL) {
        AUDIO_DEVICE_LOG_ERR("param is nullptr.");
        return HDF_ERR_INVALID_PARAM;
    }
    fsBit = AiaoGetBitCnt(bitWidth) * 2; // 2 is bit width
    (void)memset_s(&bclkSel, sizeof(uint32_t), 0, sizeof(uint32_t));
    (void)memset_s(&lrClkSel, sizeof(uint32_t), 0, sizeof(uint32_t));
    ret = AiaoGetBclkFsclk(fsBit, rate, mclkSel, &bclkSel, &lrClkSel);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AiaoGetBclkFsclk fail");
        return HDF_FAILURE;
    }

    *clkRegVal = bclkSel | (lrClkSel << 4); /* 4:left shift 4 bit */

    return HDF_SUCCESS;
}

static void AipSetCtrlReg(uint32_t chnId)
{
    URxDspCtrl aipCtrlReg;
    aipCtrlReg.Bits.reserved0 = 0;
    aipCtrlReg.Bits.rxEnable = 0;
    aipCtrlReg.Bits.rxDisableDone = 0;
    aipCtrlReg.Bits.reserved1 = 0;
    AiaoHalWriteReg(AiopRegCfg(AIP_CTRL_REG, OFFSET_MULTL, chnId), aipCtrlReg.u32);
}

int32_t AiaoRxIntClr(uint32_t chnId)
{
    const uint32_t rxTransIntClear = 0x1;
    AiaoHalWriteReg(AiopRegCfg(RX_INT_CLR, OFFSET_MULTL, chnId), rxTransIntClear);
    return HDF_SUCCESS;
}

int32_t AiaoDeviceInit(uint32_t chnId)
{
    const uint32_t aipAttrVal = 0xe4880014;
    const uint32_t aopAttrVal = 0xe4000054;

    const uint32_t rxIntEnaVal = 0x1;
    const uint32_t rxCh0IntEna = 0x1;

    AiaoHalWriteReg(AiopRegCfg(AIP_INF_ATTRI_REG, OFFSET_MULTL, chnId), aipAttrVal);
    AiaoHalWriteReg(AiopRegCfg(AOP_INF_ATTRI_REG, OFFSET_MULTL, chnId),  aopAttrVal);
    AopSetCtrlReg(chnId);
    AipSetCtrlReg(chnId);

    AiaoHalWriteReg(AiopRegCfg(AIAO_INT_ENA, OFFSET_MULTL, chnId), rxCh0IntEna);
    AiaoHalWriteReg(AiopRegCfg(RX_INT_ENA, OFFSET_MULTL, chnId), rxIntEnaVal);
    return HDF_SUCCESS;
}

int32_t AudioAoInit(const struct PlatformData *platformData)
{
    int ret;

    if (platformData == NULL || platformData->renderBufInfo.phyAddr == 0) {
        AUDIO_DEVICE_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }

    ret = AopHalSetBufferAddr(0, platformData->renderBufInfo.phyAddr);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AopHalSetBufferAddr fail.");
        return HDF_FAILURE;
    }

    ret = AopHalSetBufferSize(0, platformData->renderBufInfo.cirBufSize);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AopHalSetBufferSize: failed.");
        return HDF_FAILURE;
    }

    ret = AopHalSetTransSize(0, platformData->renderBufInfo.trafBufSize);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AopHalSetTransSize fail");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

int32_t AudioAiInit(const struct PlatformData *platformData)
{
    int ret;
    if (platformData == NULL || platformData->captureBufInfo.phyAddr == 0) {
        AUDIO_DEVICE_LOG_ERR("input param is NULL.");
        return HDF_FAILURE;
    }
    
    ret = AipHalSetBufferAddr(0, platformData->captureBufInfo.phyAddr);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AipHalSetBufferAddr: failed.");
        return HDF_FAILURE;
    }

    ret = AipHalSetBufferSize(0, platformData->captureBufInfo.cirBufSize);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AipHalSetBufferSize: failed.");
        return HDF_FAILURE;
    }

    AUDIO_DEVICE_LOG_DEBUG("trafBufSize = 0x%x", platformData->captureBufInfo.trafBufSize);

    ret = AipHalSetTransSize(0, platformData->captureBufInfo.trafBufSize);
    if (ret != HDF_SUCCESS) {
        AUDIO_DEVICE_LOG_ERR("AipHalSetTransSize fail.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
