/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef VIBRATOR_HAPTIC_H
#define VIBRATOR_HAPTIC_H

#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "osal_mutex.h"
#include "osal_timer.h"
#include "vibrator_driver_type.h"

#define VIBRATOR_MAX_HAPTIC_SEQ    1024
#define VIBRATOR_MIN_WAIT_TIME     50 // unit:ms

enum VibratorEffectType {
    VIBRATOR_TYPE_EFFECT    = 0, // Preset effect in device
    VIBRATOR_TYPE_TIME      = 1, // Preset effect by time
};

enum VibratorTimeSeqIndex {
    VIBRATOR_TIME_DELAY_INDEX    = 0,
    VIBRATOR_TIME_DURATION_INDEX = 1,
    VIBRATOR_TIME_INDEX_BUTT,
};

struct VibratorEffectNode {
    const char *effect;
    int32_t num;
    uint32_t type;   // preset type
    uint32_t *seq;
    struct DListHead node;
};

struct VibratorEffectCfg {
    enum VibratorConfigMode cfgMode; // References enum VibratorConfigMode
    uint32_t duration;
    const char *effect;
};

struct VibratorHapticData {
    bool supportHaptic;
    struct DListHead effectSeqHead;
    struct OsalMutex mutex;
    OsalTimer timer;
    uint32_t duration[VIBRATOR_TIME_INDEX_BUTT];
    int32_t effectType;
    int32_t seqCount;
    uint32_t *currentEffectSeq;
    int32_t currentSeqIndex;
};

int32_t CreateVibratorHaptic(struct HdfDeviceObject *device);
int32_t StartHaptic(struct VibratorEffectCfg *effectCfg);
int32_t StopHaptic(void);
int32_t DestroyVibratorHaptic(void);

#endif /* VIBRATOR_HAPTIC_H */
