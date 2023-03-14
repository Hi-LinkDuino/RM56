/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_touch.h"
#include "gpio_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_mem.h"
#include "osal_io.h"
#include "event_hub.h"
#include "input_i2c_ops.h"

#define I2C_TYPE 0
#define SPI_TYPE 1
#define MAX_TOUCH_DEVICE 5
#define REGISTER_BYTE_SIZE 4
#define TOUCH_CHIP_NAME_LEN 10
#if defined(CONFIG_ARCH_ROCKCHIP)
#define GTP_REG_CMD   0x8040
#define GTP_REG_CMD_1 0x8041
#endif

static TouchDriver *g_touchDriverList[MAX_TOUCH_DEVICE];
static void InputFrameReport(TouchDriver *driver);

static int32_t SetGpioDirAndLevel(int gpio, int dir, int level)
{
    int32_t ret;
    if ((dir == GPIO_DIR_IN) || (dir == GPIO_DIR_OUT)) {
        ret = GpioSetDir(gpio, dir);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: set gpio%d to %d dir failed, ret %d", __func__, gpio, dir, ret);
            return HDF_FAILURE;
        }
    }

    if ((level == GPIO_VAL_LOW) || (level == GPIO_VAL_HIGH)) {
        ret = GpioWrite(gpio, level);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: pull gpio%d to %d level failed, ret %d", __func__, gpio, level, ret);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

static int32_t SetVccPower(uint32_t status)
{
    (void)status;
    return HDF_SUCCESS;
}

static int32_t SetVciPower(uint32_t status)
{
    (void)status;
    return HDF_SUCCESS;
}

static int32_t SetGpio(uint16_t gpio, uint32_t dir, uint32_t status)
{
    int32_t ret = SetGpioDirAndLevel(gpio, dir, status);
    CHECK_RETURN_VALUE(ret);
    return HDF_SUCCESS;
}

static int32_t HandleResetEvent(ChipDevice *chipDev, uint32_t *timing, uint32_t length)
{
    int32_t ret = 0;
    uint16_t gpio;

    if (length <= PWR_DELAY_INDEX) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_FAILURE;
    }
    uint32_t type = timing[PWR_TYPE_INDEX];
    uint32_t status = timing[PWR_STATUS_INDEX];
    uint32_t dir = timing[PWR_DIR_INDEX];
    uint32_t delay = timing[PWR_DELAY_INDEX];
    HDF_LOGD("%s: type = %u, status = %u, dir = %u, delay = %u", __func__, type, status, dir, delay);

    switch (type) {
        case TYPE_VCC:
        case TYPE_VCI:
            break;
        case TYPE_INT:
            gpio = chipDev->boardCfg->pins.intGpio;
            ret = SetGpio(gpio, dir, status);
            break;
        case TYPE_RESET:
            gpio = chipDev->boardCfg->pins.rstGpio;
            ret = SetGpio(gpio, dir, status);
            break;
        default:
            HDF_LOGE("%s: unknown power type", __func__);
            break;
    }
    if (delay > 0) {
        OsalMSleep(delay);
    }
    return ret;
}

static int32_t HandlePowerEvent(ChipDevice *chipDev, uint32_t *timing, uint32_t length)
{
    int32_t ret = 0;
    uint16_t gpio;

    if (length <= PWR_DELAY_INDEX) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_FAILURE;
    }
    uint32_t type = timing[PWR_TYPE_INDEX];
    uint32_t status = timing[PWR_STATUS_INDEX];
    uint32_t dir = timing[PWR_DIR_INDEX];
    uint32_t delay = timing[PWR_DELAY_INDEX];
    HDF_LOGD("%s: type = %u, status = %u, dir = %u, delay = %u", __func__, type, status, dir, delay);

    switch (type) {
        case TYPE_VCC:
            ret = SetVccPower(status);
            break;
        case TYPE_VCI:
            ret = SetVciPower(status);
            break;
        case TYPE_RESET:
            gpio = chipDev->boardCfg->pins.rstGpio;
            ret = SetGpio(gpio, dir, status);
            break;
        case TYPE_INT:
            gpio = chipDev->boardCfg->pins.intGpio;
            ret = SetGpio(gpio, dir, status);
            break;
        default:
            HDF_LOGE("%s: unknown power type", __func__);
            break;
    }
    if (delay > 0) {
        OsalMSleep(delay);
    }
    return ret;
}

static int32_t InputPinMuxCfg(uint32_t regAddr, int32_t regSize, uint32_t regValue)
{
#if defined(CONFIG_ARCH_SPRD) || defined(CONFIG_ARCH_ROCKCHIP) || defined(LOSCFG_PLATFORM_STM32MP157)
    return HDF_SUCCESS;
#endif
    uint8_t *base = NULL;
    if (regAddr == 0) {
        HDF_LOGE("%s: regAddr invalid", __func__);
        return HDF_FAILURE;
    }

    base = OsalIoRemap(regAddr, regSize);
    if (base == NULL) {
        HDF_LOGE("%s: ioremap failed", __func__);
        return HDF_FAILURE;
    }

    OSAL_WRITEL(regValue, base);
    OsalIoUnmap((void *)base);
    return HDF_SUCCESS;
}

#if defined(CONFIG_ARCH_ROCKCHIP)
static int32_t SetResetStatus(TouchDriver *driver)
{
    int32_t ret;
    uint8_t writeBuf[5]; // 5: buffer size

    writeBuf[0] = (GTP_REG_CMD_1 >> 8) & 0xFF; // 8:high byte 0xffmask
    writeBuf[1] = GTP_REG_CMD_1 & 0xFF;
    writeBuf[2] = 0x00; // 2:index  0x00: reg value
    writeBuf[3] = 0x56; // 3:index  0x56: reg value
    writeBuf[4] = 0xAA; // 4:index  0xAA: reg value
    ret = InputI2cWrite(&driver->i2cClient, writeBuf, 5); // 5: buffer size
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InputI2cWrite failed, ret = %d", __func__, ret);
        return ret;
    }

    writeBuf[0] = (GTP_REG_CMD >> 8) & 0xFF; // 8:high byte 0xffmask
    writeBuf[1] = GTP_REG_CMD & 0xFF;
    writeBuf[2] = 0xAA; // 2:index 0xAA: reg value

    ret = InputI2cWrite(&driver->i2cClient, writeBuf, 3); // 3: buffer size
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InputI2cWrite failed, ret = %d", __func__, ret);
        return ret;
    }

    return HDF_SUCCESS;
}
#endif

static int32_t SetPowerOnTiming(ChipDevice *chipDev, bool enableRst)
{
    int32_t i;
    int32_t ret;
    uint32_t rstPinAddr;
    uint32_t rstPinValue;
    uint32_t intPinAddr;
    uint32_t intPinValue;
    SeqArray pwrOnTiming = {0};
    TouchDriver *driver = chipDev->driver;

    rstPinAddr = driver->boardCfg->pins.rstPinReg[0];
    rstPinValue = driver->boardCfg->pins.rstPinReg[1];
    intPinAddr = driver->boardCfg->pins.intPinReg[0];
    intPinValue = driver->boardCfg->pins.intPinReg[1];

    if (InputPinMuxCfg(rstPinAddr, REGISTER_BYTE_SIZE, rstPinValue) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (InputPinMuxCfg(intPinAddr, REGISTER_BYTE_SIZE, intPinValue) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: rstPinAddr = 0x%x, rstPinValue = 0x%x, intPinAddr = 0x%x, intPinValue = 0x%x",
        __func__, rstPinAddr, rstPinValue, intPinAddr, intPinValue);

    ret = memcpy_s(&pwrOnTiming, sizeof(SeqArray), &chipDev->chipCfg->pwrSeq.pwrOn, sizeof(SeqArray));
    if (ret != EOK) {
        HDF_LOGE("%s: memcpy_s failed", __func__);
        return HDF_FAILURE;
    }

    if ((pwrOnTiming.buf == NULL) || (pwrOnTiming.count == 0)) {
        HDF_LOGE("%s: pwrOnTiming config is invalid", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < pwrOnTiming.count / PWR_CELL_LEN; i++) {
        if (enableRst) {
            ret = HandleResetEvent(chipDev, pwrOnTiming.buf, PWR_CELL_LEN);
        } else {
            ret = HandlePowerEvent(chipDev, pwrOnTiming.buf, PWR_CELL_LEN);
        }
        CHECK_RETURN_VALUE(ret);
        pwrOnTiming.buf = pwrOnTiming.buf + PWR_CELL_LEN;
    }
#if defined(CONFIG_ARCH_ROCKCHIP)
    ret = SetResetStatus(driver);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SetResetStatus failed", __func__);
    }
#endif
    return HDF_SUCCESS;
}

static void EventHandle(TouchDriver *driver, ChipDevice *chipDev)
{
    int32_t ret;
    if ((chipDev->ops == NULL) || (chipDev->ops->DataHandle == NULL)) {
        return;
    }

    ret = chipDev->ops->DataHandle(chipDev);
    if (ret == HDF_SUCCESS) {
        InputFrameReport(driver);
    }
}

static int32_t IrqHandle(uint16_t intGpioNum, void *data)
{
    TouchDriver *driver = (TouchDriver *)data;
    int ret = GpioDisableIrq(intGpioNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: disable irq failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }

    EventHandle(driver, driver->device);

    ret = GpioEnableIrq(intGpioNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void InputFrameReport(TouchDriver *driver)
{
    InputDevice *dev = driver->inputDev;
    FrameData *frame = &driver->frameData;
    int32_t i;

    OsalMutexLock(&driver->mutex);
    for (i = 0; i < MAX_FINGERS_NUM; i++) {
        if (frame->fingers[i].valid) {
            input_report_abs(dev, ABS_MT_POSITION_X, frame->fingers[i].x);
            input_report_abs(dev, ABS_MT_POSITION_Y, frame->fingers[i].y);
            input_report_abs(dev, ABS_MT_TRACKING_ID, frame->fingers[i].trackId);
            input_mt_sync(dev);
        }
    }

    if ((frame->definedEvent == TOUCH_DOWN) || (frame->definedEvent == TOUCH_CONTACT)) {
        input_report_key(dev, BTN_TOUCH, 1); // BTN_TOUCH DOWN
    } else {
        input_report_key(dev, BTN_TOUCH, 0); // BTN_TOUCH UP
    }
    OsalMutexUnlock(&driver->mutex);
    input_sync(dev);
}

static int32_t SetupChipIrq(ChipDevice *chipDev)
{
    int32_t ret;
    uint16_t intGpioNum = chipDev->boardCfg->pins.intGpio;
    uint16_t irqFlag = chipDev->chipCfg->bus.chipI2c.irqFlag;

    if (chipDev->driver == NULL) {
        HDF_LOGE("%s: invalid param", __func__);
        return HDF_FAILURE;
    }
    irqFlag |= GPIO_IRQ_USING_THREAD;
    HDF_LOGD("%s: gpioNum = %u, irqFlag = %u", __func__, intGpioNum, irqFlag);
    ret = GpioSetIrq(intGpioNum, irqFlag, IrqHandle, chipDev->driver);
    if (ret != 0) {
        HDF_LOGE("%s: register irq failed, ret %d", __func__, ret);
        return ret;
    }

    ret = GpioEnableIrq(intGpioNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void ChipReset(ChipDevice *chipDev)
{
    (void)SetPowerOnTiming(chipDev, true);
}

static int32_t ChipDriverInit(ChipDevice *chipDev)
{
    int32_t count = 20;  // 20: reset time
    int32_t ret;
#if defined(CONFIG_ARCH_ROCKCHIP)
    ret = SetGpio(chipDev->boardCfg->pins.rstGpio, 1, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set rstGpio to output failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    ret = SetGpio(chipDev->boardCfg->pins.intGpio, 1, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set intGpio to output failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    OsalMSleep(100); // 100: delay time
    ret = SetGpio(chipDev->boardCfg->pins.rstGpio, 1, 1);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: set intGpio to output failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    OsalMSleep(20); // 20: delay time
#endif
    ret = SetPowerOnTiming(chipDev, false);
    CHECK_RETURN_VALUE(ret);

    if ((chipDev->ops == NULL) || (chipDev->ops->Detect == NULL)) {
        return HDF_FAILURE;
    }

    while (count --) {
        ret = chipDev->ops->Detect(chipDev);
        if (ret == HDF_SUCCESS) {
            break;
        }
        HDF_LOGI("%s: reset chip %d time", __func__, count);
        ChipReset(chipDev);
        OsalMSleep(100); // 100 : wait 100msthen try one time
    }
    CHECK_RETURN_VALUE(ret);
    HDF_LOGI("%s: chipDetect succ, ret = %d ", __func__, ret);

#if defined(CONFIG_ARCH_SPRD) || defined(CONFIG_ARCH_ROCKCHIP) || defined(LOSCFG_PLATFORM_STM32MP157)
    HDF_LOGI("%s: do not update firmware", __func__);
#else
    ret = chipDev->ops->UpdateFirmware(chipDev);
    CHECK_RETURN_VALUE(ret);
    HDF_LOGI("%s: update firmware success", __func__);
#endif

    ret = SetupChipIrq(chipDev);
    CHECK_RETURN_VALUE(ret);
    return HDF_SUCCESS;
}

static InputDevice *InputDeviceInstance(ChipDevice *chipDev)
{
    InputDevice *inputDev = (InputDevice *)OsalMemAlloc(sizeof(InputDevice));
    if (inputDev == NULL) {
        HDF_LOGE("%s: instance input device failed", __func__);
        return NULL;
    }
    (void)memset_s(inputDev, sizeof(InputDevice), 0, sizeof(InputDevice));

    inputDev->hdfDevObj = chipDev->driver->hdfTouchDev;
    inputDev->pvtData = (void *)chipDev;
    inputDev->devType = chipDev->driver->boardCfg->attr.devType;
    inputDev->devName = chipDev->driver->devName;

    return inputDev;
}

static int32_t ChipMatchCheck(const ChipDevice *chipDev, const TouchDriver *driver)
{
    const struct DeviceResourceNode *boardNode = driver->boardCfg->boardNode;
    const struct DeviceResourceNode *chipNode = chipDev->chipCfg->chipNode;

    if ((boardNode == NULL) || (boardNode->parent == NULL)) {
        HDF_LOGE("%s: board node or upper node is null", __func__);
        return HDF_FAILURE;
    }

    if ((chipNode == NULL) || (chipNode->parent == NULL) || (chipNode->parent->parent == NULL)) {
        HDF_LOGE("%s: chip node or upper node is null ", __func__);
        return HDF_FAILURE;
    }

    if (boardNode->parent == chipNode->parent->parent) {
        HDF_LOGI("%s: boardNode's father name = %s, chipNode's grandpa name = %s", __func__,
            boardNode->parent->name, chipNode->parent->parent->name);
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

static int32_t DeviceBindDriver(ChipDevice *chipDev)
{
    TouchDriver *driver = NULL;
    int32_t ret = HDF_FAILURE;
    int32_t i;
    for (i = 0; i < MAX_TOUCH_DEVICE; i++) {
        if ((g_touchDriverList[i] != NULL) && g_touchDriverList[i]->initedFlag) {
            ret = ChipMatchCheck(chipDev, g_touchDriverList[i]);
            if (ret == HDF_SUCCESS) {
                driver = g_touchDriverList[i];
                break;
            }
        }
    }

    if ((ret == HDF_FAILURE) || (driver == NULL)) {
        return HDF_FAILURE;
    }

    driver->i2cClient.i2cCfg.addr = chipDev->chipCfg->bus.chipI2c.commAddr;
    driver->device = chipDev;
    chipDev->driver = driver;
    chipDev->boardCfg = driver->boardCfg;
    return HDF_SUCCESS;
}

int32_t RegisterTouchChipDevice(ChipDevice *chipDev)
{
    int32_t ret;
    InputDevice *inputDev = NULL;
    if ((chipDev == NULL) || (chipDev->chipCfg == NULL)) {
        return HDF_ERR_INVALID_PARAM;
    }

    ret = DeviceBindDriver(chipDev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: chip device match driver failed", __func__);
        return HDF_FAILURE;
    }

    ret = ChipDriverInit(chipDev);
    if (ret != HDF_SUCCESS) {
        goto EXIT;
    }

    inputDev = InputDeviceInstance(chipDev);
    if (inputDev == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = RegisterInputDevice(inputDev);
    if (ret != HDF_SUCCESS) {
        goto EXIT1;
    }
    chipDev->driver->inputDev = inputDev;
    chipDev->ops->SetAbility(chipDev);
    return HDF_SUCCESS;

EXIT1:
    OsalMemFree(inputDev);
EXIT:
    chipDev->driver->device = NULL;
    return HDF_FAILURE;
}

static int32_t TouchGetDevType(TouchDriver *driver, struct HdfSBuf *reply)
{
    uint32_t devType = driver->devType;
    HDF_LOGI("%s: enter, devType is %u", __func__, devType);
    bool ret = HdfSbufWriteUint32(reply, devType);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t TouchSetPowerStatus(TouchDriver *driver, struct HdfSBuf *data)
{
    uint32_t pwrStatus = 0;
    bool ret = HdfSbufReadUint32(data, &pwrStatus);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    driver->pwrStatus = pwrStatus;
    HDF_LOGI("%s: set power status is %u", __func__, pwrStatus);
    return HDF_SUCCESS;
}

static int32_t TouchGetPowerStatus(TouchDriver *driver, struct HdfSBuf *reply)
{
    uint32_t pwrStatus = driver->pwrStatus;
    bool ret = HdfSbufWriteUint32(reply, pwrStatus);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: get power status is %u", __func__, pwrStatus);
    return HDF_SUCCESS;
}

static int32_t TouchGetDeviceStrInfo(TouchDriver *driver, int32_t cmd, struct HdfSBuf *reply)
{
    const char *info = NULL;
    if ((driver->device == NULL) || (driver->device->chipCfg == NULL)) {
        HDF_LOGE("%s: parameter invalid", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmd) {
        case GET_CHIP_NAME:
            info = driver->device->chipName;
            break;
        case GET_VENDOR_NAME:
            info = driver->device->vendorName;
            break;
        case GET_CHIP_INFO:
            info = driver->device->chipCfg->chipInfo;
            break;
        default:
            info = NULL;
            break;
    }

    bool ret = HdfSbufWriteString(reply, info);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: cmd is %d, the info is %s", __func__, cmd, info);
    return HDF_SUCCESS;
}

static int32_t TouchGetDeviceAttr(TouchDriver *driver, struct HdfSBuf *reply)
{
    char *tempStr = "main_touch";
    int32_t ret;
    if (driver->inputDev == NULL) {
        return HDF_FAILURE;
    }

    HDF_LOGE("%s: enter", __func__);
    ret = strncpy_s(driver->inputDev->attrSet.devName, DEV_NAME_LEN, tempStr, strlen(tempStr));
    if (ret != 0) {
        HDF_LOGE("%s: strncpy dev attr failed", __func__);
        return HDF_FAILURE;
    }

    if (!HdfSbufWriteBuffer(reply, &driver->inputDev->attrSet, sizeof(DevAttr))) {
        HDF_LOGE("%s: sbuf write dev attr failed", __func__);
    }

    HDF_LOGI("%s: get dev attr succ", __func__);
    return HDF_SUCCESS;
}

static int32_t TouchGetDeviceAbility(TouchDriver *driver, struct HdfSBuf *reply)
{
    if (driver->inputDev == NULL) {
        return HDF_FAILURE;
    }
    HDF_LOGE("%s: enter", __func__);

    if (!HdfSbufWriteBuffer(reply, &driver->inputDev->abilitySet, sizeof(DevAbility))) {
        HDF_LOGE("%s: sbuf write dev ability failed", __func__);
    }

    HDF_LOGI("%s: get dev ability succ", __func__);
    return HDF_SUCCESS;
}

static int32_t TouchSetGestureMode(TouchDriver *driver, struct HdfSBuf *data)
{
    uint32_t gestureMode = 0;
    bool ret = HdfSbufReadUint32(data, &gestureMode);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }
    driver->gestureMode = gestureMode;
    HDF_LOGD("%s: set gesture mode is %u", __func__, gestureMode);
    return HDF_SUCCESS;
}

static int32_t TouchSelfCapacitance(TouchDriver *driver, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    CapacitanceTestInfo capacTest = {0};
    bool ret = HdfSbufReadUint32(data, &capacTest.testType);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteUint32 failed", __func__);
        return HDF_FAILURE;
    }

    if (strncpy_s(capacTest.testResult, SELF_TEST_RESULT_LEN, "SUCCESS", sizeof("SUCCESS")) != EOK) {
        HDF_LOGE("%s: strncpy_s fail", __func__);
        return HDF_FAILURE;
    }

    ret = HdfSbufWriteString(reply, (const char *)capacTest.testResult);
    if (!ret) {
        HDF_LOGE("%s: HdfSbufWriteString failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: capac test type is %u, test result is %s", __func__, capacTest.testType, capacTest.testResult);
    return HDF_SUCCESS;
}

static int32_t TouchRunExtraCmd(TouchDriver *driver, struct HdfSBuf *data)
{
    InputExtraCmd cmd = {0};
    cmd.cmdCode = HdfSbufReadString(data);
    if (cmd.cmdCode == NULL) {
        HDF_LOGE("%s: HdfSbufReadString failed", __func__);
        return HDF_FAILURE;
    }

    cmd.cmdValue = HdfSbufReadString(data);
    if (cmd.cmdValue == NULL) {
        HDF_LOGE("%s: HdfSbufReadString failed", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: extra cmd code is %s, cmd value is %s", __func__, cmd.cmdCode, cmd.cmdValue);
    return HDF_SUCCESS;
}

static int32_t HdfTouchDispatch(struct HdfDeviceIoClient *client, int32_t cmd,
    struct HdfSBuf *data, struct HdfSBuf *reply)
{
    int32_t ret;
    TouchDriver *touchDriver = NULL;
    if ((client == NULL) || (client->device == NULL) || (data == NULL) || (reply == NULL)) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

    touchDriver = (TouchDriver *)client->device->priv;
    if (touchDriver == NULL) {
        HDF_LOGE("%s: touchDriver is null", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: cmd = %d", __func__, cmd);
    switch (cmd) {
        case GET_DEV_TYPE:
            ret = TouchGetDevType(touchDriver, reply);
            break;
        case SET_PWR_STATUS:
            ret = TouchSetPowerStatus(touchDriver, data);
            break;
        case GET_PWR_STATUS:
            ret = TouchGetPowerStatus(touchDriver, reply);
            break;
        case GET_CHIP_NAME:
        case GET_VENDOR_NAME:
        case GET_CHIP_INFO:
            ret = TouchGetDeviceStrInfo(touchDriver, cmd, reply);
            break;
        case GET_DEV_ATTR:
            ret = TouchGetDeviceAttr(touchDriver, reply);
            break;
        case GET_DEV_ABILITY:
            ret = TouchGetDeviceAbility(touchDriver, reply);
            break;
        case SET_GESTURE_MODE:
            ret = TouchSetGestureMode(touchDriver, data);
            break;
        case RUN_CAPAC_TEST:
            ret = TouchSelfCapacitance(touchDriver, data, reply);
            break;
        case RUN_EXTRA_CMD:
            ret = TouchRunExtraCmd(touchDriver, data);
            break;
        default:
            ret = HDF_SUCCESS;
            HDF_LOGE("%s: cmd unknown, cmd = 0x%x", __func__, cmd);
            break;
    }
    return ret;
}

static int32_t TouchSetupBus(TouchDriver *driver, TouchBoardCfg *config)
{
    uint8_t busType = config->bus.busType;
    uint8_t busNum = config->bus.i2c.busNum;
    if (busType == I2C_TYPE) {
        uint32_t i2cClkAddr = config->bus.i2c.i2cClkReg[0];
        uint32_t i2cClkValue = config->bus.i2c.i2cClkReg[1];
        uint32_t i2cDataAddr = config->bus.i2c.i2cDataReg[0];
        uint32_t i2cDataValue = config->bus.i2c.i2cDataReg[1];

        if (InputPinMuxCfg(i2cClkAddr, REGISTER_BYTE_SIZE, i2cClkValue) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        if (InputPinMuxCfg(i2cDataAddr, REGISTER_BYTE_SIZE, i2cDataValue) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }

        /* get i2c handle */
        driver->i2cClient.i2cHandle = I2cOpen(busNum);
        if (driver->i2cClient.i2cHandle == NULL) {
            HDF_LOGE("%s: open i2c%u failed", __func__, busNum);
            return HDF_FAILURE;
        }
        return HDF_SUCCESS;
    }

    if (busType == SPI_TYPE) {
        HDF_LOGI("%s: setup spi bus succ", __func__);
        return HDF_SUCCESS;
    }

    return HDF_FAILURE;
}

static int32_t TouchInitData(TouchDriver *driver, TouchBoardCfg *config)
{
    driver->devType = config->attr.devType;
    driver->devName = config->attr.devName;
    driver->i2cClient.i2cCfg.busNum = config->bus.i2c.busNum;
    driver->irqStopFlag = false;

    return HDF_SUCCESS;
}

static int32_t TouchDriverInit(TouchDriver *driver, TouchBoardCfg *config)
{
    int32_t ret = TouchInitData(driver, config);
    CHECK_RETURN_VALUE(ret);

    ret = TouchSetupBus(driver, config);
    CHECK_RETURN_VALUE(ret);

    ret = OsalMutexInit(&driver->mutex);
    CHECK_RETURN_VALUE(ret);

    driver->initedFlag = true;
    return HDF_SUCCESS;
}

static TouchBoardCfg *BoardConfigInstance(struct HdfDeviceObject *device)
{
    TouchBoardCfg *boardCfg = (TouchBoardCfg *)OsalMemAlloc(sizeof(TouchBoardCfg));
    if (boardCfg == NULL) {
        HDF_LOGE("%s: instance board config failed", __func__);
        return NULL;
    }
    (void)memset_s(boardCfg, sizeof(TouchBoardCfg), 0, sizeof(TouchBoardCfg));

    if (ParseTouchBoardConfig(device->property, boardCfg) != HDF_SUCCESS) {
        HDF_LOGE("%s: parse board config failed", __func__);
        OsalMemFree(boardCfg);
        boardCfg = NULL;
    }
    return boardCfg;
}

static TouchDriver *TouchDriverInstance(void)
{
    TouchDriver *touchDrv = (TouchDriver *)OsalMemAlloc(sizeof(TouchDriver));
    if (touchDrv == NULL) {
        HDF_LOGE("%s: instance touch driver failed", __func__);
        return NULL;
    }
    (void)memset_s(touchDrv, sizeof(TouchDriver), 0, sizeof(TouchDriver));
    return touchDrv;
}

static void AddTouchDriver(TouchDriver *driver)
{
    int32_t i;
    for (i = 0; i < MAX_TOUCH_DEVICE; i++) {
        if (g_touchDriverList[i] == NULL) {
            g_touchDriverList[i] = driver;
            return;
        }
    }
}

static int32_t HdfTouchDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    static struct IDeviceIoService touchService = {
        .Dispatch = HdfTouchDispatch,
    };

    device->service = &touchService;
    return HDF_SUCCESS;
}

static int32_t HdfTouchDriverProbe(struct HdfDeviceObject *device)
{
    int32_t ret;
    TouchBoardCfg *boardCfg = NULL;
    TouchDriver *touchDriver = NULL;

    HDF_LOGI("%s: enter", __func__);
    if (device == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    boardCfg = BoardConfigInstance(device);
    if (boardCfg == NULL) {
        return HDF_ERR_MALLOC_FAIL;
    }
    touchDriver = TouchDriverInstance();
    if (touchDriver == NULL) {
        goto EXIT;
    }

    ret = TouchDriverInit(touchDriver, boardCfg);
    if (ret == HDF_SUCCESS) {
        touchDriver->hdfTouchDev = device;
        touchDriver->boardCfg = boardCfg;
        AddTouchDriver(touchDriver);
        device->priv = (void *)touchDriver;
        HDF_LOGI("%s: %s exit succ", __func__, boardCfg->attr.devName);
        return HDF_SUCCESS;
    }

EXIT:
    OsalMemFree(boardCfg);
    if (touchDriver != NULL) {
        touchDriver->boardCfg = NULL;
        OsalMemFree(touchDriver);
    }
    return HDF_FAILURE;
}

static void HdfTouchDriverRelease(struct HdfDeviceObject *device)
{
    TouchDriver *driver = NULL;
    InputDevice *inputDev = NULL;
    int32_t i;

    if (device == NULL || device->priv == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return;
    }
    driver = device->priv;

    for (i = 0; i < MAX_TOUCH_DEVICE; i++) {
        if (g_touchDriverList[i] == driver) {
            inputDev = driver->inputDev;
            break;
        }
    }

    if (inputDev != NULL) {
        UnregisterInputDevice(inputDev);
        driver->inputDev = NULL;
    }

    OsalMutexDestroy(&driver->mutex);
    OsalMemFree(driver);
    if (i < MAX_TOUCH_DEVICE) {
        g_touchDriverList[i] = NULL;
    }
}

struct HdfDriverEntry g_hdfTouchEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_TOUCH",
    .Bind = HdfTouchDriverBind,
    .Init = HdfTouchDriverProbe,
    .Release = HdfTouchDriverRelease,
};

HDF_INIT(g_hdfTouchEntry);
