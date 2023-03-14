/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_attribute_macro.h"
#include "devhost_service_clnt.h"
#include "hcs_macro.h"
#include "hdf_attribute_manager.h"
#include "hdf_config_macro.h"
#include "hdf_host_info.h"
#include "hdf_log.h"
#include "osal_mem.h"
#define HDF_LOG_TAG hdf_attr_macro
static bool HdfHostListCompare(struct HdfSListNode *listEntryFirst, struct HdfSListNode *listEntrySecond)
{
    struct HdfHostInfo *attrFirst = NULL;
    struct HdfHostInfo *attrSecond = NULL;
    if (listEntryFirst == NULL || listEntrySecond == NULL) {
        return false;
    }
    attrFirst = (struct HdfHostInfo *)listEntryFirst;
    attrSecond = (struct HdfHostInfo *)listEntrySecond;
    return attrFirst->priority <= attrSecond->priority;
}

static bool GetHostInfo(const struct HdfHostType *hostNode, struct HdfHostInfo *hostInfo)
{
    hostInfo->hostName = hostNode->devHostName;
    hostInfo->priority = hostNode->priority;
    if (strcmp(hostInfo->hostName, "") == 0) {
        HDF_LOGW("%s: get host name failed", __func__);
        return false;
    }
    if (hostInfo->priority > MAX_PRIORITY_NUM) {
        HDF_LOGW("%s: get host priority failed, priority is: %u", __func__, hostInfo->priority);
        return false;
    }

    return true;
}

static void AttributeManagerFreeDevHost(struct HdfDevHostMgr *hostMgr)
{
    struct HdfHostType *host = NULL;
    struct HdfHostType *hostTemp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(host, hostTemp, &hostMgr->hosts, struct HdfHostType, hostEntry) {
        OsalMemFree(host);
    }
    OsalMemFree(hostMgr);
}

bool HdfAttributeManagerGetHostList(struct HdfSList *hostList)
{
    struct HdfDevHostMgr *devHost = NULL;
    struct HdfHostType *host = NULL;
    uint16_t hostId = 0;

    if (hostList == NULL) {
        return false;
    }

    devHost = (struct HdfDevHostMgr *)OsalMemCalloc(sizeof(*devHost));
    if (devHost == NULL) {
        return HDF_FAILURE;
    }

    DListHeadInit(&devHost->hosts);

    HCS_FOREACH_CHILD_VARGS(HDF_DEVICE_INFO, HDF_DEAL_HOST, devHost->hosts);

    DLIST_FOR_EACH_ENTRY(host, &devHost->hosts, struct HdfHostType, hostEntry) {
        struct HdfHostInfo *hostInfo = HdfHostInfoNewInstance();
        if (hostInfo == NULL) {
            HdfSListFlush(hostList, HdfHostInfoDelete);
            HDF_LOGE("%s: new hostInfo is null", __func__);
            return false;
        }

        if (!GetHostInfo(host, hostInfo)) {
            HdfHostInfoFreeInstance(hostInfo);
            continue;
        }

        hostInfo->hostId = hostId;
        if (!HdfSListAddOrder(hostList, &hostInfo->node, HdfHostListCompare)) {
            HdfHostInfoFreeInstance(hostInfo);
            continue;
        }
        hostId++;
    }
    AttributeManagerFreeDevHost(devHost);

    return true;
}

static bool HdfDeviceListCompare(struct HdfSListNode *listEntryFirst, struct HdfSListNode *listEntrySecond)
{
    struct HdfDeviceInfo *attrFirst = NULL;
    struct HdfDeviceInfo *attrSecond = NULL;
    if (listEntryFirst == NULL || listEntrySecond == NULL) {
        return false;
    }
    attrFirst = (struct HdfDeviceInfo *)listEntryFirst;
    attrSecond = (struct HdfDeviceInfo *)listEntrySecond;
    return attrFirst->priority <= attrSecond->priority;
}

static bool CheckDeviceInfo(const struct HdfDeviceInfo *deviceNodeInfo)
{
    if (deviceNodeInfo->policy >= SERVICE_POLICY_INVALID) {
        HDF_LOGE("%s: policy %u is invalid", __func__, deviceNodeInfo->policy);
        return false;
    }

    if (deviceNodeInfo->priority > MAX_PRIORITY_NUM) {
        HDF_LOGE("%s: priority %u is invalid", __func__, deviceNodeInfo->priority);
        return false;
    }

    if (deviceNodeInfo->preload >= DEVICE_PRELOAD_INVALID) {
        HDF_LOGE("%s: preload %u is invalid", __func__, deviceNodeInfo->preload);
        return false;
    }

    return (strcmp(deviceNodeInfo->moduleName, "") != 0);
}

static bool GetDeviceNodeInfo(const struct HdfDeviceNodeType *deviceNode, struct HdfDeviceInfo *deviceNodeInfo)
{
    deviceNodeInfo->policy = deviceNode->policy;
    deviceNodeInfo->priority = deviceNode->priority;
    deviceNodeInfo->preload = deviceNode->preload;
    deviceNodeInfo->permission = deviceNode->permission;
    deviceNodeInfo->deviceMatchAttr = deviceNode->deviceMatchAttr;
    deviceNodeInfo->moduleName = deviceNode->moduleName;
    deviceNodeInfo->svcName = deviceNode->svcName;

    return CheckDeviceInfo(deviceNodeInfo);
}

static bool GetDevcieNodeList(const struct HdfDeviceType *device,
    struct DevHostServiceClnt *hostClnt, uint16_t deviceIdx)
{
    uint8_t deviceNodeIdx = 1;
    uint16_t hostId = hostClnt->hostId;
    struct HdfDeviceInfo *deviceNodeInfo = NULL;
    const struct HdfDeviceNodeType *devNode = NULL;

    DLIST_FOR_EACH_ENTRY(devNode, &device->deviceNodes, struct HdfDeviceNodeType, deviceNodeEntry) {
        deviceNodeInfo = HdfDeviceInfoNewInstance();
        if (deviceNodeInfo == NULL) {
            return false;
        }
        if (!GetDeviceNodeInfo(devNode, deviceNodeInfo)) {
            HdfDeviceInfoFreeInstance(deviceNodeInfo);
            HDF_LOGE("%s: failed to parse device node info, ignore", __func__);
            continue;
        }

        deviceNodeInfo->deviceId = MK_DEVID(hostId, deviceIdx, deviceNodeIdx);
        if (deviceNodeInfo->preload != DEVICE_PRELOAD_DISABLE) {
            if (!HdfSListAddOrder(&hostClnt->unloadDevInfos, &deviceNodeInfo->node, HdfDeviceListCompare)) {
                HDF_LOGE("%s: failed to add device info to list %s", __func__, deviceNodeInfo->svcName);
                HdfDeviceInfoFreeInstance(deviceNodeInfo);
                continue;
            }
        } else {
            HdfSListAdd(&hostClnt->dynamicDevInfos, &deviceNodeInfo->node);
        }

        deviceNodeIdx++;
    }
    return deviceNodeIdx > 1;
}

static void AttributeManagerFreeHost(struct HdfHostType *host)
{
    struct HdfDeviceType *device = NULL;
    struct HdfDeviceType *deviceTemp = NULL;
    struct HdfDeviceNodeType *devNode = NULL;
    struct HdfDeviceNodeType *devNodeTemp = NULL;

    DLIST_FOR_EACH_ENTRY_SAFE(device, deviceTemp, &host->devices, struct HdfDeviceType, deviceEntry) {
        DLIST_FOR_EACH_ENTRY_SAFE(devNode, devNodeTemp, &device->deviceNodes, struct HdfDeviceNodeType,
                                  deviceNodeEntry) {
            OsalMemFree(devNode);
        }
        OsalMemFree(device);
    }
    OsalMemFree(host);
}

int HdfAttributeManagerGetDeviceList(struct DevHostServiceClnt *hostClnt)
{
    struct HdfHostType *host = NULL;
    struct HdfDeviceType *device = NULL;
    struct HdfDeviceNodeType *deviceNode = NULL;

    uint16_t deviceIdx = 1;
    int ret = HDF_DEV_ERR_NO_DEVICE;

    if (hostClnt == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    host = (struct HdfHostType *)OsalMemCalloc(sizeof(*host));
    if (host == NULL) {
        return HDF_FAILURE;
    }

    DListHeadInit(&host->devices);

    HCS_FOREACH_CHILD_VARGS(HDF_DEVICE_INFO, HDF_FIND_HOST, hostClnt->hostName, host);

    DLIST_FOR_EACH_ENTRY(device, &host->devices, struct HdfDeviceType, deviceEntry) {
        if (!GetDevcieNodeList(device, hostClnt, deviceIdx)) {
            return ret;
        }
        deviceIdx++;
    }
    AttributeManagerFreeHost(host);
    return HDF_SUCCESS;
}