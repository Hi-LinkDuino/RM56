/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "eth_device.h"
#include <string.h>
#include "eth_chip_driver.h"
#include "device_resource_if.h"
#include "hdf_log.h"
#include "los_vm_zone.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG eth_device

int32_t GetEthIfName(const struct ConfigEthDevList *configEthDevList, char *ifName, uint32_t ifNameSize)
{
    if (configEthDevList == NULL || ifName == NULL || ifNameSize == 0) {
        HDF_LOGE("%s:para is null!", __func__);
        return HDF_FAILURE;
    }
    if (snprintf_s(ifName, ifNameSize, ifNameSize - 1, "eth%d", configEthDevList->deviceInstId) < 0) {
        HDF_LOGE("%s:format ifName failed!deviceInstId = %d.", __func__, configEthDevList->deviceInstId);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

struct EthDevice *CreateEthDevice(const struct ConfigEthDevList *configEthDevList)
{
    int32_t ret;
    struct HdfEthNetDeviceData *data = NULL;
    struct NetDevice *netDevice = NULL;
    struct EthDevice *ethDevice = NULL;
    char ethIfName[IFNAMSIZ] = {0};

    if (configEthDevList == NULL) {
        HDF_LOGE("%s input is NULL!", __func__);
        return NULL;
    }
    data = (struct HdfEthNetDeviceData *)OsalMemCalloc(sizeof(struct HdfEthNetDeviceData));
    if (data == NULL) {
        HDF_LOGE("%s failed to OsalMemCalloc HdfEthNetDeviceData", __func__);
        return NULL;
    }
    ret = GetEthIfName(configEthDevList, ethIfName, IFNAMSIZ);
    if (ret != HDF_SUCCESS) {
        OsalMemFree(data);
        return NULL;
    }
    netDevice = NetDeviceInit(ethIfName, strlen(ethIfName), ETHERNET_LINK, LITE_OS);
    if (netDevice == NULL) {
        HDF_LOGE("%s failed to init netDevice", __func__);
        OsalMemFree(data);
        return NULL;
    }
    ethDevice = (struct EthDevice *)OsalMemCalloc(sizeof(struct EthDevice));
    if (ethDevice == NULL) {
        HDF_LOGE("%s failed to OsalMemCalloc ethDevice", __func__);
        NetDeviceDeInit(netDevice);
        OsalMemFree(data);
        return NULL;
    }
    netDevice->mlPriv = ethDevice;
    ethDevice->netdev = netDevice;
    ethDevice->netdev->classDriverPriv = data;
    ethDevice->name = configEthDevList->driverName;
    return ethDevice;
}

static int32_t ParseEthMacConfig(const struct DeviceResourceNode *node, struct HdfConfigEthMac *ethMacConfig)
{
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL || ethMacConfig == NULL) {
        HDF_LOGE("%s: invalid node or ethMacConfig!", __func__);
        return HDF_FAILURE;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32(node, "regBase", &ethMacConfig->regBase, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read regBase fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32(node, "irqVector", &ethMacConfig->irqVector, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read irqVector fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "mdioFrqDiv", &ethMacConfig->mdioFrqDiv, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read mdioFrqDiv fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "txBusy", &ethMacConfig->txBusy, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read txBusy fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32(node, "iobase", &ethMacConfig->iobase, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read iobase fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint32(node, "regOffSize", &ethMacConfig->regOffSize, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read regOffSize fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ParseEthPhyConfig(const struct DeviceResourceNode *node, struct HdfConfigEthPhy *ethPhyConfig)
{
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL || ethPhyConfig == NULL) {
        HDF_LOGE("%s: invalid node or ethPhyConfig!", __func__);
        return HDF_FAILURE;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "phyMode", &ethPhyConfig->phyMode, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read phyMode fail", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ParseEthDevInstConfig(const struct DeviceResourceNode *node, struct ConfigEthDevList *devLstConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *ethMacNode = NULL;
    const struct DeviceResourceNode *ethPhyNode = NULL;
    if (node == NULL || devLstConfig == NULL) {
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "deviceInstId", &devLstConfig->deviceInstId, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: deviceInstId fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "isSetDefault", &devLstConfig->isSetDefault, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: isSetDefault fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "driverName", &devLstConfig->driverName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: driverName fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "hwXmitq", &devLstConfig->hwXmitq, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read hwXmitq fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "qSize", &devLstConfig->qSize, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read qSize fail", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "port", &devLstConfig->port, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: read port fail", __func__);
        return HDF_FAILURE;
    }

    ethMacNode = drsOps->GetChildNode(node, "MAC");
    if (ethMacNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseEthMacConfig(ethMacNode, &devLstConfig->ethMac) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ethPhyNode = drsOps->GetChildNode(node, "PHY");
    if (ethPhyNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseEthPhyConfig(ethPhyNode, &devLstConfig->ethPhy) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ParseEthDevListNode(const struct DeviceResourceNode *node, struct EthConfig *ethConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    uint32_t index = 0;

    if (node == NULL || ethConfig == NULL) {
        HDF_LOGE("%s: invalid node or ethConfig!", __func__);
        return HDF_FAILURE;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseEthDevInstConfig(childNode, &ethConfig->deviceInst[index]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        index++;
        ethConfig->deviceListSize++;
    }
    HDF_LOGD("%s: deviceListSize=%d", __func__, ethConfig->deviceListSize);
    return HDF_SUCCESS;
}

static int32_t ParseConfigFromProperty(const struct DeviceResourceNode *node, struct EthConfig *config)
{
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *devListNode = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    devListNode = drsOps->GetChildNode(node, "ethList");
    if (devListNode == NULL) {
        HDF_LOGE("%s: get child node fail!", __func__);
        return HDF_FAILURE;
    }
    return ParseEthDevListNode(devListNode, config);
}

struct EthConfig *GetEthConfig(const struct DeviceResourceNode *node)
{
    if (node == NULL) {
        HDF_LOGE("%s input is NULL", __func__);
        return NULL;
    }

    struct EthConfig *config = NULL;

    config = (struct EthConfig *)OsalMemCalloc(sizeof(*config));
    if (config == NULL) {
        HDF_LOGE("%s failed to OsalMemCalloc config", __func__);
        return NULL;
    }

    if (ParseConfigFromProperty(node, config) != HDF_SUCCESS) {
        HDF_LOGE("%s failed to parse config from property", __func__);
        OsalMemFree(config);
        return NULL;
    }
    return config;
}

struct HdfEthNetDeviceData *GetEthNetDeviceData(const struct NetDevice *netDev)
{
    if (netDev == NULL) {
        return NULL;
    }
    return (struct HdfEthNetDeviceData *)(netDev->classDriverPriv);
}

int32_t ReleaseEthDevice(struct EthDevice *ethDevice)
{
    int32_t ret;
    struct HdfEthNetDeviceData *data = NULL;

    if (ethDevice == NULL) {
        HDF_LOGE("%s:NULL ptr!", __func__);
        return HDF_FAILURE;
    }

    data = GetEthNetDeviceData(ethDevice->netdev);
    if (data == NULL) {
        HDF_LOGE("%s: GetEthNetDeviceData failed!", __func__);
        return HDF_FAILURE;
    }
    struct HdfEthMacChipDriver *macChipDriver = data->macChipDriver;
    if (macChipDriver != NULL) {
        OsalMemFree(macChipDriver);
        macChipDriver = NULL;
    }
    ret = NetDeviceDeInit(ethDevice->netdev);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    OsalMemFree(ethDevice);
    OsalMemFree(data);
    return HDF_SUCCESS;
}
