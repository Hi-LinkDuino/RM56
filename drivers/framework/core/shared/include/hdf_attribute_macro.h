/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_ATTRIBUTE_MACRO_H
#define HDF_ATTRIBUTE_MACRO_H

#include "hdf_log.h"
#include "hdf_dlist.h"

struct HdfDevHostMgr {
    struct DListHead hosts;
};

struct HdfHostType {
    const char *devHostName;
    uint16_t priority;
    struct DListHead devices;
    struct DListHead hostEntry;
};

struct HdfDeviceType {
    struct DListHead deviceNodes;
    struct DListHead deviceEntry;
};

struct HdfDeviceNodeType {
    uint16_t policy;
    uint16_t priority;
    uint16_t preload;
    uint16_t permission;
    const char *moduleName;
    const char *svcName;
    const char *deviceMatchAttr;
    struct DListHead deviceNodeEntry;
};

#define HDF_DEAL_DEVICE_NODE(node, deviceNodes) \
    do { \
        deviceNode = (struct HdfDeviceNodeType *)OsalMemCalloc(sizeof(*deviceNode)); \
        if (deviceNode == NULL) { \
            HDF_LOGE("%s malloc fail", __func__); \
            return false; \
        } \
        deviceNode->policy = HCS_PROP(node, policy); \
        deviceNode->priority = HCS_PROP(node, priority); \
        deviceNode->preload = HCS_PROP(node, preload); \
        deviceNode->permission = HCS_PROP(node, permission); \
        deviceNode->moduleName = HCS_PROP(node, moduleName); \
        deviceNode->svcName = HCS_PROP(node, serviceName); \
        deviceNode->deviceMatchAttr = HCS_PROP(node, deviceMatchAttr); \
        DListInsertTail(&deviceNode->deviceNodeEntry, &(deviceNodes)); \
    } while (0)

#define HDF_DEAL_DEVICE(node, devices) \
    do { \
        device = (struct HdfDeviceType *)OsalMemCalloc(sizeof(*device)); \
        if (device == NULL) { \
            HDF_LOGE("%s malloc fail", __func__); \
            return false; \
        } \
        DListHeadInit(&device->deviceNodes); \
        node##_foreach_child_vargs(HDF_DEAL_DEVICE_NODE, device->deviceNodes); \
        DListInsertTail(&device->deviceEntry, &(devices)); \
    } while (0)

#define HDF_FIND_HOST(node, name, host) \
    do { \
        if (strcmp(HCS_PROP(node, hostName), name) == 0) { \
            (host)->devHostName = HCS_PROP(node, hostName); \
            (host)->priority = HCS_PROP(node, priority); \
            node##_foreach_child_vargs(HDF_DEAL_DEVICE, host->devices); \
            break; \
        } \
    } while (0)

#define HDF_DEAL_HOST(node, hosts) \
    do { \
        host = (struct HdfHostType *)OsalMemCalloc(sizeof(*host)); \
        if (host == NULL) { \
            HDF_LOGE("%s malloc fail", __func__); \
            return false; \
        } \
        host->devHostName = HCS_PROP(node, hostName); \
        host->priority = HCS_PROP(node, priority); \
        DListInsertTail(&(host)->hostEntry, &(hosts)); \
    } while (0)

#define HDF_DEVICE_INFO HCS_NODE(HCS_ROOT, device_info)

#endif /* HDF_ATTRIBUTE_MACRO_H */
