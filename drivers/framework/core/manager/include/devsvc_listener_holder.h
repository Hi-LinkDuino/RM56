/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef DEVICE_SERVICE_LISTENER_HOLDER_H
#define DEVICE_SERVICE_LISTENER_HOLDER_H

#include "hdf_service_status.h"
#include "hdf_dlist.h"

struct ServStatListenerHolder {
    uint16_t listenClass;
    struct DListHead node;
    uint64_t index;
    int32_t (*NotifyStatus)(struct ServStatListenerHolder *holder, struct ServiceStatus *status);
    void (*Recycle)(struct ServStatListenerHolder *holder);
};

void ServStatListenerHolderinit(void);

struct ServStatListenerHolder *ServStatListenerHolderCreate(uintptr_t listener, uint16_t listenClass);
void ServStatListenerHolderRelease(struct ServStatListenerHolder *holder);
struct ServStatListenerHolder *ServStatListenerHolderGet(uint64_t index);

#endif /* DEVICE_SERVICE_LISTENER_HOLDER_H */
