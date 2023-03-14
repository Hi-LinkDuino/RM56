/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_DEVICE_NODE_H
#define HDF_DEVICE_NODE_H

#include "hdf_device.h"
#include "hdf_device_info.h"
#include "hdf_device_desc.h"
#include "hdf_dlist.h"
#include "hdf_driver.h"
#include "hdf_pm.h"

struct HdfDeviceNode;
struct DevHostService;

struct IDeviceNode {
    struct HdfObject object;
    int (*PublishService)(struct HdfDeviceNode *devNode);
    int (*RemoveService)(struct HdfDeviceNode *devNode);
    int (*LaunchNode)(struct HdfDeviceNode *devNode);
    void (*UnlaunchNode)(struct HdfDeviceNode *devNode);
};

struct HdfDeviceNode {
    struct IDeviceNode super;
    struct DListHead entry;
    struct PowerStateToken *powerToken;
    struct DevHostService *hostService;
    struct HdfDeviceObject deviceObject;
    struct IHdfDeviceToken *token;
    struct HdfDriver *driver;
    struct HdfDevice *device;
    char *servName;
    const char *servInfo;
    char *driverName;
    devid_t devId;
    uint16_t policy;
    uint16_t permission;
    uint8_t devStatus;
    bool servStatus;
    char *interfaceDesc;
};

enum DevNodeStaus {
    DEVNODE_NONE = 0,
    DEVNODE_INITED,
    DEVNODE_LAUNCHED,
};

int HdfDeviceNodeAddPowerStateListener(
    struct HdfDeviceNode *devNode, const struct IPowerEventListener *listener);
void HdfDeviceNodeRemovePowerStateListener(
    struct HdfDeviceNode *devNode, const struct IPowerEventListener *listener);
void HdfDeviceNodeConstruct(struct HdfDeviceNode *devNode);
void HdfDeviceNodeDestruct(struct HdfDeviceNode *devNode);
struct HdfDeviceNode *HdfDeviceNodeNewInstance(const struct HdfDeviceInfo *deviceInfo, struct HdfDriver *driver);
void HdfDeviceNodeFreeInstance(struct HdfDeviceNode *devNode);
void HdfDeviceNodeDelete(struct HdfDeviceNode *devNode);
int HdfDeviceNodePublishPublicService(struct HdfDeviceNode *devNode);
int HdfDeviceNodeRemoveService(struct HdfDeviceNode *devNode);
int DeviveDriverBind(struct HdfDeviceNode *devNode);

#endif /* HDF_DEVICE_NODE_H */
