/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "audio_accessory_base.h"
#include "audio_driver_log.h"
#include "i2c_if.h"
#include "osal_time.h"

#define HDF_LOG_TAG HDF_AUDIO_KADM

#define COMM_SHIFT_8BIT      (8)
#define COMM_MASK_FF         (0xFF)
#define COMM_WAIT_TIMES      (10) // ms

#define I2C_REG_LEN         (1)
#define I2C_REG_MSGLEN      (3)
#define I2C_MSG_NUM         (2)
#define I2C_MSG_BUF_SIZE    (2)

uint16_t g_i2cDevAddr, g_i2cBusNumber;
struct AudioRegCfgGroupNode **g_audioRegCfgGroupNode = NULL;
struct AudioKcontrol *g_audioControls = NULL;

static const char *g_audioAccessoryControlsList[AUDIO_CTRL_LIST_MAX] = {
    "Main Playback Volume", "Main Capture Volume",
    "Playback Mute", "Capture Mute", "Mic Left Gain",
    "Mic Right Gain", "External Codec Enable",
    "Internally Codec Enable", "Render Channel Mode", "Captrue Channel Mode"
};
/*
 * release I2C object public function
 */
static void AccessoryI2cRelease(struct I2cMsg *msgs, int16_t msgSize, DevHandle i2cHandle)
{
    if (msgs != NULL) {
        if (msgSize == 0 && msgs->buf != NULL) {
            OsalMemFree(msgs->buf);
            msgs->buf = NULL;
        } else if (msgSize == 1 && msgs[0].buf != NULL) {
            OsalMemFree(msgs[0].buf);
            msgs[0].buf = NULL;
        } else if (msgSize >= I2C_MSG_NUM) {
            if (msgs[0].buf != NULL) {
                msgs[0].buf = NULL;
            }
            if (msgs[1].buf != NULL) {
                OsalMemFree(msgs[1].buf);
                msgs[1].buf = NULL;
            }
        }
        AUDIO_DRIVER_LOG_DEBUG("OsalMemFree msgBuf success.\n");
    }
    // close i2c device
    if (i2cHandle != NULL) {
        I2cClose(i2cHandle);
        i2cHandle = NULL;
        AUDIO_DRIVER_LOG_DEBUG("I2cClose success.\n");
    }
}

static int32_t AccessoryI2cMsgFill(const struct AudioAddrConfig *regAttr, uint16_t rwFlag,
    uint8_t *regs, struct I2cMsg *msgs)
{
    uint8_t *msgBuf = NULL;
    if (rwFlag != 0 && rwFlag != I2C_FLAG_READ) {
        AUDIO_DRIVER_LOG_ERR("invalid rwFlag value: %d.", rwFlag);
        return HDF_ERR_INVALID_PARAM;
    }
    regs[0] = regAttr->addr;
    msgs[0].addr = g_i2cDevAddr;
    msgs[0].flags = 0;
    msgs[0].len = I2C_REG_MSGLEN;
    AUDIO_DRIVER_LOG_DEBUG("msgs[0].addr=0x%02x, regs[0]=0x%02x.", msgs[0].addr, regs[0]);
    if (rwFlag == 0) { // write
        // S 11011A2A1 0 A ADDR A MS1 A LS1 A <....> P
        msgBuf = OsalMemCalloc(I2C_REG_MSGLEN);
        if (msgBuf == NULL) {
            AUDIO_DRIVER_LOG_ERR("[write]: malloc buf failed!");
            return HDF_ERR_MALLOC_FAIL;
        }
        msgBuf[0] = regs[0];
        msgBuf[1] = (uint8_t)(regAttr->value >> COMM_SHIFT_8BIT); // High 8 bit
        msgBuf[I2C_MSG_BUF_SIZE] = (uint8_t)(regAttr->value & COMM_MASK_FF);    // Low 8 bit
        msgs[0].buf = msgBuf;
        AUDIO_DRIVER_LOG_DEBUG("msgBuf[1]=0x%02x.", msgBuf[1]);
        AUDIO_DRIVER_LOG_DEBUG("msgBuf[2]=0x%02x.", msgBuf[I2C_MSG_BUF_SIZE]);
    } else {
        // S 11011A2A1 0 A ADDR A Sr 11011A2A1 1 A MS1 A LS1 A <....> NA P
        msgBuf = OsalMemCalloc(I2C_MSG_NUM);
        if (msgBuf == NULL) {
            AUDIO_DRIVER_LOG_ERR("[read]: malloc buf failed!");
            return HDF_ERR_MALLOC_FAIL;
        }
        msgs[0].len = 1;
        msgs[0].buf = regs;
        msgs[1].addr = g_i2cDevAddr;
        msgs[1].flags = I2C_FLAG_READ;
        msgs[1].len = I2C_MSG_NUM;
        msgs[1].buf = msgBuf;
    }
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AccessoryI2cReadWrite(struct AudioAddrConfig *regAttr, uint16_t rwFlag)
{
    int32_t ret;
    DevHandle i2cHandle;
    int16_t transferMsgCount = 1;
    uint8_t regs[I2C_REG_LEN];
    struct I2cMsg msgs[I2C_MSG_NUM];
    (void)memset_s(msgs, sizeof(struct I2cMsg) * I2C_MSG_NUM, 0, sizeof(struct I2cMsg) * I2C_MSG_NUM);

    AUDIO_DRIVER_LOG_DEBUG("entry.\n");
    if (regAttr == NULL || rwFlag > 1) {
        AUDIO_DRIVER_LOG_ERR("invalid parameter.");
        return HDF_ERR_INVALID_PARAM;
    }
    i2cHandle = I2cOpen(g_i2cBusNumber);
    if (i2cHandle == NULL) {
        AUDIO_DRIVER_LOG_ERR("open i2cBus:%u failed! i2cHandle is NULL", g_i2cBusNumber);
        return HDF_FAILURE;
    }
    if (rwFlag == I2C_FLAG_READ) {
        transferMsgCount = I2C_MSG_NUM;
    }
    ret = AccessoryI2cMsgFill(regAttr, rwFlag, regs, msgs);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AccessoryI2cMsgFill failed!");
        I2cClose(i2cHandle);
        return HDF_FAILURE;
    }
    ret = I2cTransfer(i2cHandle, msgs, transferMsgCount);
    if (ret != transferMsgCount) {
        AUDIO_DRIVER_LOG_ERR("I2cTransfer err:%d", ret);
        AccessoryI2cRelease(msgs, transferMsgCount, i2cHandle);
        return HDF_FAILURE;
    }
    if (rwFlag == I2C_FLAG_READ) {
        regAttr->value = (msgs[1].buf[0] << COMM_SHIFT_8BIT) | msgs[1].buf[1]; // result value 16 bit
        AUDIO_DRIVER_LOG_DEBUG("[read]: regAttr->regValue=0x%04x.\n", regAttr->value);
    }
    AccessoryI2cRelease(msgs, transferMsgCount, i2cHandle);
    return HDF_SUCCESS;
}

// Read control reg bits value
int32_t AccessoryRegBitsRead(struct AudioMixerControl *regAttr, uint32_t *regValue)
{
    int32_t ret;
    struct AudioAddrConfig regVal;
    if (regAttr == NULL || regAttr->reg < 0 || regValue == NULL) {
        AUDIO_DRIVER_LOG_ERR("input invalid parameter.");
        return HDF_ERR_INVALID_PARAM;
    }
    regVal.addr  = regAttr->reg;
    ret = AccessoryI2cReadWrite(&regVal, I2C_FLAG_READ);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AccessoryRegBitsRead failed.");
        return HDF_FAILURE;
    }
    *regValue = regVal.value;
    regAttr->value = (regVal.value >> regAttr->shift) & regAttr->mask;
    if (regAttr->value > regAttr->max || regAttr->value < regAttr->min) {
        AUDIO_DRIVER_LOG_ERR("invalid bitsValue=0x%x", regAttr->value);
        return HDF_FAILURE;
    }
    if (regAttr->invert) {
        regAttr->value = regAttr->max - regAttr->value;
    }
    AUDIO_DRIVER_LOG_DEBUG("regAddr=0x%x, regValue=0x%x, currBitsValue=0x%x",
        regAttr->reg, regVal.value, regAttr->value);
    AUDIO_DRIVER_LOG_DEBUG("mask=0x%x, shift=%d, max=0x%x,min=0x%x, invert=%d",
        regAttr->mask, regAttr->shift, regAttr->max, regAttr->min, regAttr->invert);
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

// Update contrl reg bits value
int32_t AccessoryRegBitsUpdate(struct AudioMixerControl regAttr)
{
    int32_t ret;
    struct AudioAddrConfig regVal;
    uint32_t newValue, newMask, value;
    if (regAttr.invert) {
        regAttr.value = regAttr.max - regAttr.value;
    }
    newValue = regAttr.value << regAttr.shift;
    newMask = regAttr.mask << regAttr.shift;
    ret = AccessoryRegBitsRead(&regAttr, &value);
    if (ret != HDF_SUCCESS) {
        ADM_LOG_ERR("AccessoryRegBitsRead faileded, ret=%d.", ret);
        return HDF_FAILURE;
    }
    regVal.value = (value & ~newMask) | (newValue & newMask);
    regVal.addr  = regAttr.reg;
    ret = AccessoryI2cReadWrite(&regVal, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AccessoryI2cReadWrite faileded.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("regAddr=0x%x, regValue=0x%x, oldValue=0x%x, newValue=0x%x,",
        regAttr.reg, regVal.value, regAttr.value, newValue);
    AUDIO_DRIVER_LOG_DEBUG(" mask=0x%x, shift=%d, max=0x%x, min=0x%x, invert=%d",
        newMask, regAttr.shift, regAttr.max, regAttr.min, regAttr.invert);
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

// update external codec I2S frequency
int32_t AccessoryDaiParamsUpdate(struct DaiParamsVal daiParamsVal)
{
    int32_t ret;
    const int itemNum = 3; // current only 3 items(frequency, format, channel)
    struct AudioMixerControl *regAttr = NULL;
    ret = (g_audioRegCfgGroupNode == NULL || g_audioRegCfgGroupNode[AUDIO_DAI_PATAM_GROUP] == NULL
           || g_audioRegCfgGroupNode[AUDIO_DAI_PATAM_GROUP]->regCfgItem == NULL
           || g_audioRegCfgGroupNode[AUDIO_DAI_PATAM_GROUP]->itemNum < itemNum);
    if (ret) {
        AUDIO_DRIVER_LOG_ERR("g_audioRegCfgGroupNode[AUDIO_DAI_PATAM_GROUP] is NULL.");
        return HDF_FAILURE;
    }
    regAttr = g_audioRegCfgGroupNode[AUDIO_DAI_PATAM_GROUP]->regCfgItem;
    regAttr[0].value = daiParamsVal.frequencyVal;
    ret = AccessoryRegBitsUpdate(regAttr[0]);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set freq failed.");
        return HDF_FAILURE;
    }
    regAttr[1].value = daiParamsVal.formatVal;
    ret = AccessoryRegBitsUpdate(regAttr[1]);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set format failed.");
        return HDF_FAILURE;
    }
    regAttr[itemNum - 1].value = daiParamsVal.channelVal;
    ret = AccessoryRegBitsUpdate(regAttr[itemNum - 1]);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set channel failed.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceFrequencyParse(uint32_t rate, uint16_t *freq)
{
    if (freq == NULL) {
        AUDIO_DRIVER_LOG_ERR("input param is NULL");
        return HDF_FAILURE;
    }
    switch (rate) {
        case I2S_SAMPLE_FREQUENCY_8000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_8000;
            break;
        case I2S_SAMPLE_FREQUENCY_11025:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_11025;
            break;
        case I2S_SAMPLE_FREQUENCY_12000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_12000;
            break;
        case I2S_SAMPLE_FREQUENCY_16000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_16000;
            break;
        case I2S_SAMPLE_FREQUENCY_22050:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_22050;
            break;
        case I2S_SAMPLE_FREQUENCY_24000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_24000;
            break;
        case I2S_SAMPLE_FREQUENCY_32000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_32000;
            break;
        case I2S_SAMPLE_FREQUENCY_44100:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_44100;
            break;
        case I2S_SAMPLE_FREQUENCY_48000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_48000;
            break;
        case I2S_SAMPLE_FREQUENCY_64000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_64000;
            break;
        case I2S_SAMPLE_FREQUENCY_88200:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_88200;
            break;
        case I2S_SAMPLE_FREQUENCY_96000:
            *freq = I2S_SAMPLE_FREQUENCY_REG_VAL_96000;
            break;
        default:
            AUDIO_DRIVER_LOG_ERR("rate: %u is not support.", rate);
            return HDF_ERR_NOT_SUPPORT;
    }
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceCfgGet(struct AccessoryData *accessoryData,
    struct AccessoryTransferData *accessoryTransferData)
{
    int32_t ret;
    int32_t index;
    uint32_t audioCfgCtrlCount;
    struct AudioControlConfig *ctlcfgItem;
    ret = (accessoryData == NULL || accessoryData->regConfig == NULL || accessoryTransferData == NULL);
    if (ret) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }
    g_i2cDevAddr = accessoryTransferData->i2cDevAddr;
    g_i2cBusNumber = accessoryTransferData->i2cBusNumber;
    g_audioRegCfgGroupNode = accessoryData->regConfig->audioRegParams;
    ret = (g_audioRegCfgGroupNode[AUDIO_CTRL_CFG_GROUP] == NULL ||
           g_audioRegCfgGroupNode[AUDIO_CTRL_CFG_GROUP]->ctrlCfgItem == NULL ||
           g_audioRegCfgGroupNode[AUDIO_CTRL_PATAM_GROUP] == NULL ||
           g_audioRegCfgGroupNode[AUDIO_CTRL_PATAM_GROUP]->regCfgItem == NULL);
    if (ret) {
        AUDIO_DRIVER_LOG_ERR("parsing params is NULL.");
        return HDF_FAILURE;
    }
    ctlcfgItem = g_audioRegCfgGroupNode[AUDIO_CTRL_CFG_GROUP]->ctrlCfgItem;
    audioCfgCtrlCount = g_audioRegCfgGroupNode[AUDIO_CTRL_CFG_GROUP]->itemNum;
    g_audioControls = (struct AudioKcontrol *)OsalMemCalloc(audioCfgCtrlCount * sizeof(struct AudioKcontrol));
    accessoryTransferData->accessoryRegCfgGroupNode = g_audioRegCfgGroupNode;
    accessoryTransferData->accessoryCfgCtrlCount = audioCfgCtrlCount;
    accessoryTransferData->accessoryControls = g_audioControls;
    for (index = 0; index < audioCfgCtrlCount; index++) {
        g_audioControls[index].iface = ctlcfgItem[index].iface;
        g_audioControls[index].name  = g_audioAccessoryControlsList[ctlcfgItem[index].arrayIndex];
        g_audioControls[index].Info  = AudioInfoCtrlOps;
        g_audioControls[index].privateValue =
            (unsigned long)(uintptr_t)(void*)(&g_audioRegCfgGroupNode[AUDIO_CTRL_PATAM_GROUP]->regCfgItem[index]);
        g_audioControls[index].Get = AudioAccessoryGetCtrlOps;
        g_audioControls[index].Set = AudioAccessorySetCtrlOps;
    }
    return HDF_SUCCESS;
}

/*
 * init control reg to default value
 */
int32_t AccessoryDeviceCtrlRegInit(void)
{
    int32_t ret, i;
    struct AudioAddrConfig *initCfg;
    // Set codec control register(00h-14h) default value
    ret = (g_audioRegCfgGroupNode == NULL || g_audioRegCfgGroupNode[AUDIO_INIT_GROUP] == NULL
           || g_audioRegCfgGroupNode[AUDIO_INIT_GROUP]->addrCfgItem == NULL);
    if (ret) {
        AUDIO_DRIVER_LOG_ERR("g_audioRegCfgGroupNode[AUDIO_INIT_GROUP] is NULL.");
        return HDF_FAILURE;
    }
    initCfg = g_audioRegCfgGroupNode[AUDIO_INIT_GROUP]->addrCfgItem;
    for (i = 0; i < g_audioRegCfgGroupNode[AUDIO_INIT_GROUP]->itemNum; i++) {
        AUDIO_DRIVER_LOG_DEBUG("i=%d, Addr = [0x%2x]", i, initCfg[i].addr);
        ret = AccessoryI2cReadWrite(&initCfg[i], 0);
        if (ret != HDF_SUCCESS) {
            AUDIO_DRIVER_LOG_ERR("AccessoryI2cReadWrite(write) err, regAttr.regAddr: 0x%x.\n",
                                 initCfg[i].addr);
            return HDF_FAILURE;
        }
        OsalMSleep(COMM_WAIT_TIMES);
    }
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceRegRead(const struct AccessoryDevice *codec, uint32_t reg, uint32_t *val)
{
    int32_t ret;
    struct AudioAddrConfig regAttr;
    if (val == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }
    (void)codec;
    regAttr.addr = (uint8_t)reg;
    regAttr.value = 0;
    ret = AccessoryI2cReadWrite(&regAttr, I2C_FLAG_READ);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("failed.");
        return HDF_FAILURE;
    }
    *val = regAttr.value;
    AUDIO_DRIVER_LOG_DEBUG("success");
    return HDF_SUCCESS;
}

int32_t AccessoryDeviceRegWrite(const struct AccessoryDevice *codec, uint32_t reg, uint32_t value)
{
    int32_t ret;
    struct AudioAddrConfig regAttr;
    (void)codec;
    regAttr.addr = (uint8_t)reg;
    regAttr.value = (uint16_t)value;
    ret = AccessoryI2cReadWrite(&regAttr, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("failed.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("success");
    return HDF_SUCCESS;
}

int32_t AccessoryGetConfigInfo(const struct HdfDeviceObject *device, struct AccessoryData *accessoryData)
{
    if (device == NULL || accessoryData == NULL) {
        AUDIO_DRIVER_LOG_ERR("param is null!");
        return HDF_FAILURE;
    }

    if (accessoryData->regConfig != NULL) {
        ADM_LOG_ERR("g_codecData regConfig  fail!");
        return HDF_FAILURE;
    }

    accessoryData->regConfig = (struct AudioRegCfgData *)OsalMemCalloc(sizeof(*(accessoryData->regConfig)));
    if (accessoryData->regConfig == NULL) {
        ADM_LOG_ERR("malloc AudioRegCfgData fail!");
        return HDF_FAILURE;
    }

    if (CodecGetRegConfig(device, accessoryData->regConfig) != HDF_SUCCESS) {
        ADM_LOG_ERR("CodecGetRegConfig fail!");
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
