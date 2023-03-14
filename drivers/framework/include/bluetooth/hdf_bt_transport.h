/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_BT_TRANSPORT_H
#define HDF_BT_TRANSPORT_H
#include "hdf_device_desc.h"

struct HdfBtTransportOps;

struct HdfBtTransport {
    const struct HdfBtTransportOps *ops;
};

struct HdfBtTransportOps {
    int32_t (*Init)(struct HdfBtTransport *transport);
    int32_t (*GetVfsDevName)(struct HdfBtTransport *transport, char *buf, uint32_t size);
    void (*Deinit)(struct HdfBtTransport *transport);
    void (*Destory)(struct HdfBtTransport *transport);
};

struct HdfBtTransportService {
    struct IDeviceIoService base;
    struct HdfBtTransport *(*CreateTransport)(const struct DeviceResourceNode *node);
};

#endif
