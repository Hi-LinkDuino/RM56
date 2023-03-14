/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_THREAD_H
#define HDF_THREAD_H

#include <stdint.h>
#include "osal_thread.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfThread {
    struct OsalThread adapter;
    bool status;
    void (*ThreadEntry)(void *);
    bool (*IsRunning)();
    void (*Start)(struct HdfThread *thread);
    void (*Stop)(struct HdfThread *thread);
};

void HdfThreadConstruct(struct HdfThread *thread);
void HdfThreadDestruct(struct HdfThread *thread);
struct HdfThread *HdfThreadNewInstance(void);
void HdfThreadFreeInstance(struct HdfThread *instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_THREAD_H */

