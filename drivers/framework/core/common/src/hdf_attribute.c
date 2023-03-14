/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_attribute_manager.h"
#include "devhost_service_clnt.h"
#include "hcs_tree_if.h"
#include "hdf_host_info.h"
#include "hdf_log.h"
#ifdef LOSCFG_DRIVERS_HDF_USB_PNP_NOTIFY
#include "usb_pnp_manager.h"
#endif

#define ATTR_HOST_NAME "hostName"
#define ATTR_DEV_POLICY "policy"
#define ATTR_DEV_PRIORITY "priority"
#define ATTR_DEV_PRELOAD "preload"
#define ATTR_DEV_PERMISSION "permission"
#define ATTR_DEV_MODULENAME "moduleName"
#define ATTR_DEV_SVCNAME "serviceName"
#define ATTR_DEV_MATCHATTR "deviceMatchAttr"
#define MANAGER_NODE_MATCH_ATTR "hdf_manager"

#define DEFATLT_DEV_PRIORITY 100

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

static const struct DeviceResourceNode *GetHdfManagerNode(const struct DeviceResourceNode *node)
{
    return HcsGetNodeByMatchAttr(node, MANAGER_NODE_MATCH_ATTR);
}

static bool GetHostInfo(const struct DeviceResourceNode *hostNode, struct HdfHostInfo *hostInfo)
{
    uint16_t readNum = 0;
    if ((HcsGetString(hostNode, ATTR_HOST_NAME, &hostInfo->hostName, NULL) != HDF_SUCCESS) ||
        (strcmp(hostInfo->hostName, "") == 0)) {
        HDF_LOGW("%s: get host name failed", __func__);
        return false;
    }
    if ((HcsGetUint16(hostNode, ATTR_DEV_PRIORITY, &readNum, 0) != HDF_SUCCESS) ||
        (readNum > MAX_PRIORITY_NUM)) {
        HDF_LOGW("%s: get host priority failed, priority is: %u", __func__, readNum);
        return false;
    }
    hostInfo->priority = readNum;
    return true;
}

bool HdfAttributeManagerGetHostList(struct HdfSList *hostList)
{
    const struct DeviceResourceNode *hdfManagerNode = NULL;
    const struct DeviceResourceNode *hostNode = NULL;
    uint16_t hostId = 0;
    if (hostList == NULL) {
        return false;
    }

    hdfManagerNode = GetHdfManagerNode(HdfGetHcsRootNode());
    if (hdfManagerNode == NULL) {
        HDF_LOGE("%s: get hdf manager node is null", __func__);
        return false;
    }

    hostNode = hdfManagerNode->child;
    for (; hostNode != NULL; hostNode = hostNode->sibling) {
        struct HdfHostInfo *hostInfo = HdfHostInfoNewInstance();
        if (hostInfo == NULL) {
            HdfSListFlush(hostList, HdfHostInfoDelete);
            HDF_LOGE("%s: new hostInfo is null", __func__);
            return false;
        }
        if (!GetHostInfo(hostNode, hostInfo)) {
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

static const struct DeviceResourceNode *GetHostNode(const char *inHostName)
{
    const struct DeviceResourceNode *hdfManagerNode = NULL;
    const struct DeviceResourceNode *hostNode = NULL;
    const char *hostName = NULL;
    if (inHostName == NULL) {
        return NULL;
    }
    hdfManagerNode = GetHdfManagerNode(HdfGetHcsRootNode());
    if (hdfManagerNode == NULL) {
        return NULL;
    }
    hostNode = hdfManagerNode->child;
    while (hostNode != NULL) {
        if (HcsGetString(hostNode, ATTR_HOST_NAME, &hostName, NULL) != HDF_SUCCESS) {
            hostNode = hostNode->sibling;
            continue;
        }
        if (strcmp(hostName, inHostName) == 0) {
            return hostNode;
        }
        hostNode = hostNode->sibling;
    }
    return NULL;
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

static bool GetDeviceNodeInfo(const struct DeviceResourceNode *deviceNode, struct HdfDeviceInfo *deviceNodeInfo)
{
    HcsGetUint16(deviceNode, ATTR_DEV_POLICY, &deviceNodeInfo->policy, 0);
    HcsGetUint16(deviceNode, ATTR_DEV_PRIORITY, &deviceNodeInfo->priority, DEFATLT_DEV_PRIORITY);
    HcsGetUint16(deviceNode, ATTR_DEV_PRELOAD, &deviceNodeInfo->preload, 0);
    HcsGetUint16(deviceNode, ATTR_DEV_PERMISSION, &deviceNodeInfo->permission, 0);
    HcsGetString(deviceNode, ATTR_DEV_MATCHATTR, &deviceNodeInfo->deviceMatchAttr, NULL);

    if (HcsGetString(deviceNode, ATTR_DEV_MODULENAME, &deviceNodeInfo->moduleName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get module name", __func__);
        return false;
    }

    if (HcsGetString(deviceNode, ATTR_DEV_SVCNAME, &deviceNodeInfo->svcName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to get service name", __func__);
        return false;
    }

    return CheckDeviceInfo(deviceNodeInfo);
}

static bool GetDevcieNodeList(const struct DeviceResourceNode *device,
    struct DevHostServiceClnt *hostClnt, uint16_t deviceIdx)
{
    uint8_t deviceNodeIdx = 1;
    uint16_t hostId = hostClnt->hostId;
    struct HdfDeviceInfo *deviceNodeInfo = NULL;
    const struct DeviceResourceNode *devNodeResource = device->child;

    for (; devNodeResource != NULL; devNodeResource = devNodeResource->sibling) {
        deviceNodeInfo = HdfDeviceInfoNewInstance();
        if (deviceNodeInfo == NULL) {
            return false;
        }
        if (!GetDeviceNodeInfo(devNodeResource, deviceNodeInfo)) {
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

int HdfAttributeManagerGetDeviceList(struct DevHostServiceClnt *hostClnt)
{
    uint16_t deviceIdx = 1;
    const struct DeviceResourceNode *hostNode = NULL;
    const struct DeviceResourceNode *device = NULL;
    int ret = HDF_DEV_ERR_NO_DEVICE;

    if (hostClnt == NULL) {
        return HDF_ERR_INVALID_PARAM;
    }

    hostNode = GetHostNode(hostClnt->hostName);
    if (hostNode == NULL) {
        return ret;
    }

    for (device = hostNode->child; device != NULL; device = device->sibling, deviceIdx++) {
        if (!GetDevcieNodeList(device, hostClnt, deviceIdx)) {
            return ret;
        }
    }

    return HDF_SUCCESS;
}