/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef EVENT_HUB_H
#define EVENT_HUB_H

#include "input-event-codes.h"
#include "hdf_input_device_manager.h"
#include "osal_time.h"

#define input_report_abs    ReportAbs
#define input_report_key    ReportKey
#define input_report_rel    ReportRel
#define input_sync          ReportSync
#define input_mt_sync       ReportMtSync

typedef struct {
    uint32_t type;
    uint32_t code;
    int32_t value;
    uint64_t time;
} EventPackage;

void PushOnePackage(InputDevice *inputDev, uint32_t type, uint32_t code, int32_t value);

static inline void ReportAbs(InputDevice *inputDev, uint32_t code, int32_t value)
{
    PushOnePackage(inputDev, EV_ABS, code, value);
}

static inline void ReportKey(InputDevice *inputDev, uint32_t code, int32_t value)
{
    PushOnePackage(inputDev, EV_KEY, code, !!value);
}

static inline void ReportRel(InputDevice *inputDev, uint32_t code, int32_t value)
{
    PushOnePackage(inputDev, EV_REL, code, value);
}

static inline void ReportSync(InputDevice *inputDev)
{
    PushOnePackage(inputDev, EV_SYN, SYN_REPORT, 0);
}

static inline void ReportMtSync(InputDevice *inputDev)
{
    PushOnePackage(inputDev, EV_SYN, SYN_MT_REPORT, 0);
}

#endif
