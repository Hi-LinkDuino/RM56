/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DEVICE_H
#define HDF_DEVICE_H

#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_object.h"

/*
 * -----------------+----------------+----------------+----------------+
 * |      BYTE      |      BYTE      |      BYTE      |      BYTE      |
 * +---------------------------------+---------------------------------+
 * |    host id     |             device id           | device node id |
 * -----------------+---------------------------------+----------------+
 */
typedef uint32_t devid_t;

#define HOSTID_BITS 8
#define DEVICEID_BITS 16
#define DEVNODEID_BITS 8
#define DEVNODEID_MASK ((1 << DEVNODEID_BITS) - 1)
#define DEVICEID_MASK ((1 << DEVICEID_BITS) - 1)

#define HOSTID(devid) (((uint32_t)(devid)) >> (DEVICEID_BITS + DEVNODEID_BITS))
#define DEVICEID(devid) ((((uint32_t)(devid)) >> DEVNODEID_BITS) & DEVICEID_MASK)
#define DEVNODEID(devid) (((uint32_t)(devid)) & DEVNODEID_MASK)

#define MK_DEVID(hostId, deviceId, devnodeId) \
    (((hostId) << (DEVICEID_BITS + DEVNODEID_BITS)) | ((deviceId) << DEVNODEID_BITS) | (devnodeId))

struct HdfDeviceNode;

struct IHdfDevice {
    struct HdfObject object;
    int (*Attach)(struct IHdfDevice *device, struct HdfDeviceNode *deviceNode);
    int (*Detach)(struct IHdfDevice *device, struct HdfDeviceNode *deviceNode);
    struct HdfDeviceNode *(*GetDeviceNode)(struct IHdfDevice *device, devid_t devid);
    int (*DetachWithDevid)(struct IHdfDevice *device, devid_t devid);
};

struct HdfDevice {
    struct IHdfDevice super;
    struct DListHead node;
    struct DListHead devNodes;
    devid_t deviceId;
    uint16_t devidIndex;
};
int HdfDeviceDetach(struct IHdfDevice *devInst, struct HdfDeviceNode *devNode);
void HdfDeviceConstruct(struct HdfDevice *device);
void HdfDeviceDestruct(struct HdfDevice *device);
struct HdfObject *HdfDeviceCreate(void);
void HdfDeviceRelease(struct HdfObject *object);
struct HdfDevice *HdfDeviceNewInstance(void);
void HdfDeviceFreeInstance(struct HdfDevice *device);

#endif /* HDF_DEVICE_H */
