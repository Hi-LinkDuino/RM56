/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_TOUCH_H
#define HDF_TOUCH_H

#include <securec.h>
#include "osal_time.h"
#include "hdf_input_device_manager.h"
#include "input_config.h"
#include "input_i2c_ops.h"

#define CHIP_CHECK_RETURN(ret) do { \
    if ((ret) != HDF_SUCCESS) { \
        return ret; \
    } \
} while (0)

#define MAX_FINGERS_NUM 10
#define SELF_TEST_RESULT_LEN 20

#define PWR_TYPE_INDEX      0
#define PWR_STATUS_INDEX    1
#define PWR_DIR_INDEX       2
#define PWR_DELAY_INDEX     3
#define PWR_CELL_LEN        4

typedef enum {
    TOUCH_DOWN,    // 0
    TOUCH_UP,      // 1
    TOUCH_CONTACT, // 2
} EventType;

typedef enum {
    TYPE_UNKNOWN,    // 0
    TYPE_VCC,        // 1
    TYPE_VCI,        // 2
    TYPE_RESET,      // 3
    TYPE_INT,        // 4
} TimingType;

typedef struct {
    int32_t x;    /* x coordinate */
    int32_t y;    /* y coordinate */
    int32_t pressure;
    int32_t trackId;   /* touch ID */
    int32_t status;    /* record every point's status */
    bool valid;
} FingerData;

typedef struct {
    FingerData fingers[MAX_FINGERS_NUM];
    int32_t realPointNum;
    int32_t definedEvent;    /* touch event: 0-down; 1-up; 2-contact */
    OsalTimespec time;
} FrameData;

struct TouchChipDevice;
typedef struct TouchPlatformDriver {
    struct HdfDeviceObject *hdfTouchDev;
    InputDevice *inputDev;
    struct TouchChipDevice *device;
    FrameData frameData;
    uint32_t devType;
    const char *devName;
    TouchBoardCfg *boardCfg;
    InputI2cClient i2cClient;
    struct OsalMutex mutex;
    uint32_t pwrStatus;
    uint32_t gestureMode;
    bool initedFlag;
    bool irqStopFlag;
} TouchDriver;

struct TouchChipOps;
typedef struct TouchChipDevice {
    TouchDriver *driver;
    const char *chipName;
    const char *vendorName;
    struct TouchChipOps *ops;
    TouchChipCfg *chipCfg;
    TouchBoardCfg *boardCfg;
} ChipDevice;

struct TouchChipOps {
    int32_t (*Init)(ChipDevice *device);
    int32_t (*Detect)(ChipDevice *device);
    int32_t (*Resume)(ChipDevice *device);
    int32_t (*Suspend)(ChipDevice *device);
    int32_t (*DataHandle)(ChipDevice *device);
    int32_t (*UpdateFirmware)(ChipDevice *device);
    void (*SetAbility)(ChipDevice *device);
};

typedef struct {
    uint32_t testType;
    char testResult[SELF_TEST_RESULT_LEN];
} CapacitanceTestInfo;

typedef struct {
    const char *cmdCode;
    const char *cmdValue;
} InputExtraCmd;

int32_t RegisterTouchChipDevice(ChipDevice *chipDev);

#endif
