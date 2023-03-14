/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_HID_ADAPTER_H
#define HDF_HID_ADAPTER_H

#include "hdf_input_device_manager.h"

#ifdef DIV_ROUND_UP
#undef DIV_ROUND_UP
#endif
#define DIV_ROUND_UP(nr, d) (((nr) + (d) - 1) / (d))

#ifdef BYTE_HAS_BITS
#undef BYTE_HAS_BITS
#endif
#define BYTE_HAS_BITS 8

#ifdef BITS_TO_LONG
#undef BITS_TO_LONG
#endif
#define BITS_TO_LONG(count) DIV_ROUND_UP(count, BYTE_HAS_BITS * sizeof(unsigned long))

#define INPUT_PROP_MAX    0x1f
#define HDF_INPUT_PROP_CNT    (INPUT_PROP_MAX + 1)
#define EV_MAX            0x1f
#define HDF_EV_CNT            (EV_MAX + 1)
#define ABS_MAX           0x3f
#define HDF_ABS_CNT           (ABS_MAX + 1)
#define REL_MAX           0x0f
#define HDF_REL_CNT           (REL_MAX + 1)
#define KEY_MAX           0x2ff
#define HDF_KEY_CNT           (KEY_MAX + 1)
#define SND_MAX           0x07
#define HDF_SND_CNT           (SND_MAX + 1)
#define LED_MAX           0x0f
#define HDF_LED_CNT           (LED_MAX + 1)
#define MSC_MAX           0x07
#define HDF_MSC_CNT           (MSC_MAX + 1)
#define SW_MAX            0x0f
#define HDF_SW_CNT            (SW_MAX + 1)
#define FF_MAX            0x7f
#define HDF_FF_CNT            (FF_MAX + 1)

typedef struct {
    int32_t axis;
    int32_t min;
    int32_t max;
    int32_t fuzz;
    int32_t flat;
    int32_t range;
} AbsAttr;

typedef struct HidInformation {
    uint32_t devType;
    const char *devName;

    unsigned long devProp[BITS_TO_LONG(HDF_INPUT_PROP_CNT)];
    unsigned long eventType[BITS_TO_LONG(HDF_EV_CNT)];
    unsigned long absCode[BITS_TO_LONG(HDF_ABS_CNT)];
    unsigned long relCode[BITS_TO_LONG(HDF_REL_CNT)];
    unsigned long keyCode[BITS_TO_LONG(HDF_KEY_CNT)];
    unsigned long ledCode[BITS_TO_LONG(HDF_LED_CNT)];
    unsigned long miscCode[BITS_TO_LONG(HDF_MSC_CNT)];
    unsigned long soundCode[BITS_TO_LONG(HDF_SND_CNT)];
    unsigned long forceCode[BITS_TO_LONG(HDF_FF_CNT)];
    unsigned long switchCode[BITS_TO_LONG(HDF_SW_CNT)];
    AbsAttr axisInfo[HDF_ABS_CNT];

    uint16_t bustype;
    uint16_t vendor;
    uint16_t product;
    uint16_t version;
} HidInfo;

enum HidType {
    HID_TYPE_TOUCH,               /* Touchscreen */
    HID_TYPE_KEY,                 /* Physical key */
    HID_TYPE_BUTTON,              /* Virtual button */
    HID_TYPE_CROWN,               /* Watch crown */
    HID_TYPE_BEGIN_POS = 33,    /* HID type start position */
    HID_TYPE_ENCODER,             /* Encoder */
    HID_TYPE_MOUSE,             /* Mouse */
    HID_TYPE_KEYBOARD,          /* Keyboard */
    HID_TYPE_ROCKER,              /* ROCKER */
    HID_TYPE_TRACKBALL,           /* TRACKBALL */
    HID_TYPE_UNKNOWN,           /* Unknown input device type */
};

void SendInfoToHdf(HidInfo *info);
void* HidRegisterHdfInputDev(HidInfo *info);
void HidUnregisterHdfInputDev(const void *inputDev);
void HidReportEvent(const void *inputDev, uint32_t type, uint32_t code, int32_t value);

#endif
