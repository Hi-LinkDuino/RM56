/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_EAPOL_H
#define HDF_EAPOL_H

#include "hdf_netbuf.h"
#include "net_device.h"

#define EAPOL_MAX_COUNT 10
#define EAPOL_MAX_ENQUEUE_TIME 300 /* 300 Second */

struct EapolData {
    bool regFlag;  /* is already registered */
    uint16_t count; /* eapol frame count in NetBuffQueue. */
    uint16_t maxCount;
    int64_t enqueueTime;                       /* record eapol frame time for dfx. */
    void (*notify)(const char *name, void *context); /* notify eapol frame enqueue message */
    void *context;
    NetBufQueue eapolQueue;
};

struct Eapol {
    struct EapolInterface *eapolOp;
};

struct EapolEnable {
    void (*notify)(const char *name, void *context);
    void *context;
};

struct EapolRx {
    uint8_t *buff;
    uint32_t len;
};

struct EapolTx {
    uint8_t *buff;
    uint32_t len;
};

struct EapolInterface {
    int32_t (*enableEapol)(const struct NetDevice *netDevice, struct EapolEnable *buff);
    int32_t (*disableEapol)(const struct NetDevice *netDevice);
    int32_t (*getEapol)(const struct NetDevice *netDevice, struct EapolRx *buff);
    int32_t (*sendEapol)(struct NetDevice *netDevice, struct EapolTx *buff);
    int32_t (*writeEapolToQueue)(const struct NetDevice *netDevice, NetBuf *buff);
};

/* API */
const struct Eapol *EapolGetInstance(void);
void DestroyEapolData(struct NetDevice *netDevice);
int32_t CreateEapolData(struct NetDevice *netDevice);

#endif /* HDF_EAPOL_H */