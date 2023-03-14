/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef POWER_STATE_TOKEN_IF_H
#define POWER_STATE_TOKEN_IF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    PSM_STATE_IDLE,           /* Idle state */
    PSM_STATE_ACTIVE,         /* Activated state */
    PSM_STATE_INACTIVE,       /* Error state */
} HdfPsmState;

struct IPowerStateToken {
    void (*AcquireWakeLock)(struct IPowerStateToken *);
    void (*ReleaseWakeLock)(struct IPowerStateToken *);
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POWER_STATE_TOKEN_IF_H */