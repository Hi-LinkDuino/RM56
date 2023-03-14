/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_INPUT_DEVICE_MANAGER_H
#define HDF_INPUT_DEVICE_MANAGER_H

#include "input-event-codes.h"
#include "osal_mutex.h"
#include "hdf_types.h"
#include "hdf_device_desc.h"

#ifdef HDF_LOG_TAG
#undef HDF_LOG_TAG
#endif
#define HDF_LOG_TAG HDF_INPUT_DRV
#define INPUT_DEV_PATH_LEN 64
#define MAX_INPUT_DEV_NUM  32
#define DEV_NAME_LEN 64
#define ONLINE    0
#define OFFLINE   1

#ifdef DIV_ROUND_UP
#undef DIV_ROUND_UP
#endif
#define DIV_ROUND_UP(nr, d) (((nr) + (d) - 1) / (d))

#define BYTE_HAS_BITS 8
#define BITS_TO_UINT64(count)    DIV_ROUND_UP(count, BYTE_HAS_BITS * sizeof(unsigned long))

#ifndef BITS_PER_LONG
#define BITS_PER_LONG 32
#endif
#define SET_BIT(nr)    (1UL << ((nr) % BITS_PER_LONG))

#define FF_MAX    0x7f
#define FF_CNT    (FF_MAX + 1)

#define CHECK_RETURN_VALUE(ret) do { \
    if ((ret) != HDF_SUCCESS) { \
        return ret; \
    } \
} while (0)

typedef struct {
    uint32_t devId;
    uint32_t devType;
} DevDesc;

typedef struct {
    uint32_t devId;
    uint32_t devType;
    uint32_t status;
} HotPlugEvent;

typedef struct {
    struct IDeviceIoService ioService;
    uint32_t (*getDeviceCount)(void);
} IInputManagerService;

typedef struct {
    unsigned long devProp[BITS_TO_UINT64(INPUT_PROP_CNT)];
    unsigned long eventType[BITS_TO_UINT64(EV_CNT)];
    unsigned long absCode[BITS_TO_UINT64(ABS_CNT)];
    unsigned long relCode[BITS_TO_UINT64(REL_CNT)];
    unsigned long keyCode[BITS_TO_UINT64(KEY_CNT)];
    unsigned long ledCode[BITS_TO_UINT64(LED_CNT)];
    unsigned long miscCode[BITS_TO_UINT64(MSC_CNT)];
    unsigned long soundCode[BITS_TO_UINT64(SND_CNT)];
    unsigned long forceCode[BITS_TO_UINT64(FF_CNT)];
    unsigned long switchCode[BITS_TO_UINT64(SW_CNT)];
    unsigned long keyType[BITS_TO_UINT64(KEY_CNT)];
    unsigned long ledType[BITS_TO_UINT64(LED_CNT)];
    unsigned long soundType[BITS_TO_UINT64(SND_CNT)];
    unsigned long switchType[BITS_TO_UINT64(SW_CNT)];
} DevAbility;

typedef struct {
    int32_t axis;
    int32_t min;
    int32_t max;
    int32_t fuzz;
    int32_t flat;
    int32_t range;
} DimensionInfo;

typedef struct {
    uint16_t busType;
    uint16_t vendor;
    uint16_t product;
    uint16_t version;
} InputDevIdentify;

typedef struct {
    char devName[DEV_NAME_LEN];
    InputDevIdentify id;
    DimensionInfo axisInfo[ABS_CNT];
} DevAttr;

typedef struct InputDeviceInfo {
    struct HdfDeviceObject *hdfDevObj;
    uint32_t devId;
    uint32_t devType;
    const char *devName;
    uint16_t pkgNum;
    uint16_t pkgCount;
    bool errFrameFlag;
    struct HdfSBuf *pkgBuf;
    struct HdfSBuf *eventBuf;
    void *pvtData;
    DevAttr attrSet;
    DevAbility abilitySet;
    struct InputDeviceInfo *next;
} InputDevice;

typedef struct {
    struct HdfDeviceObject *hdfDevObj;
    uint32_t devCount;
    struct OsalMutex mutex;
    bool initialized;
    InputDevice *inputDevList;
} InputManager;

enum InputDevType {
    INDEV_TYPE_TOUCH,               /* Touchscreen */
    INDEV_TYPE_KEY,                 /* Physical key */
    INDEV_TYPE_BUTTON,              /* Virtual button */
    INDEV_TYPE_CROWN,               /* Watch crown */
    INDEV_TYPE_HID_BEGIN_POS = 33,  /* HID type start position */
    INDEV_TYPE_ENCODER,             /* Customized type of a specific function or event */
    INDEV_TYPE_MOUSE,               /* Mouse */
    INDEV_TYPE_KEYBOARD,            /* Keyboard */
    INDEV_TYPE_ROCKER,              /* ROCKER */
    INDEV_TYPE_TRACKBALL,           /* TRACKBALL */
    INDEV_TYPE_UNKNOWN,             /* Unknown input device type */
};

enum InputIOsvcCmdId {
    GET_DEV_TYPE,
    SET_PWR_STATUS,
    GET_PWR_STATUS,
    GET_CHIP_INFO,
    GET_VENDOR_NAME,
    GET_CHIP_NAME,
    GET_DEV_ATTR,
    GET_DEV_ABILITY,
    SET_GESTURE_MODE,
    RUN_CAPAC_TEST,
    RUN_EXTRA_CMD,
};

enum TouchIoctlCmd {
    INPUT_IOCTL_GET_EVENT_DATA,
    INPUT_IOCTL_SET_POWER_STATUS,
    INPUT_IOCTL_GET_POWER_STATUS,
    INPUT_IOCTL_GET_DEVICE_TYPE,
    INPUT_IOCTL_GET_CHIP_INFO,
    INPUT_IOCTL_GET_VENDOR_NAME,
    INPUT_IOCTL_GET_CHIP_NAME,
    INPUT_IOCTL_SET_GESTURE_MODE,
    INPUT_IOCTL_RUN_CAPACITANCE_TEST,
    INPUT_IOCTL_RUN_EXTRA_CMD,
};
InputManager* GetInputManager(void);
int32_t RegisterInputDevice(InputDevice *device);
void UnregisterInputDevice(InputDevice *inputDev);

#endif