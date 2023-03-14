/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_wlan_config.h"
#include "hdf_log.h"

#define HDF_LOG_TAG HDF_WIFI_CORE
static struct HdfConfigWlanRoot g_configWlanModuleRoot = { 0 };
static int32_t ParseWlanStaConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanStation *staConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || staConfig == NULL) {
        HDF_LOGE("%s: invalid node or staConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "name", &staConfig->name, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GetString fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &staConfig->mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: GetUint8 fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: name=%s, mode=%u!", __func__, staConfig->name, staConfig->mode);
    return HDF_SUCCESS;
}

static int32_t ParseWlanApConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanHostAp *apConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || apConfig == NULL) {
        HDF_LOGE("%s: invalid node or apConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "name", &apConfig->name, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: name fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &apConfig->mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mode fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "vapResNum", &apConfig->vapResNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get vapResNnm fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "userResNum", &apConfig->userResNum, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: get userResNum fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: name=%s, mode=%u, vapResNum=%u, userResNum=%u!", __func__, apConfig->name, apConfig->mode,
        apConfig->vapResNum, apConfig->userResNum);
    return HDF_SUCCESS;
}

static int32_t ParseWlanP2PConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanP2P *p2pConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || p2pConfig == NULL) {
        HDF_LOGE("%s: invalid node or p2pConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "name", &p2pConfig->name, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: name fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &p2pConfig->mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mode fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: name=%s, mode=%u", __func__, p2pConfig->name, p2pConfig->mode);
    return HDF_SUCCESS;
}

static int32_t ParseWlanMac80211Config(const struct DeviceResourceNode *node, struct HdfConfigWlanMac80211 *macConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || macConfig == NULL) {
        HDF_LOGE("%s: invalid node or macConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &macConfig->mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mode fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: mode=%u", __func__, macConfig->mode);
    return HDF_SUCCESS;
}

static int32_t ParseWlanPhyConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanPhy *phyConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;

    if (node == NULL || phyConfig == NULL) {
        HDF_LOGE("%s: invalid node or macConfig!\n", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!\n", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint8(node, "mode", &phyConfig->mode, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: mode fail!\n", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: mode=%d\n", __func__, phyConfig->mode);
    return HDF_SUCCESS;
}

static int32_t ParseWlanModuleConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanModuleConfig *modConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    int32_t ret;
    const struct DeviceResourceNode *staConfigNode = NULL;
    const struct DeviceResourceNode *apConfigNode = NULL;
    const struct DeviceResourceNode *p2pConfigNode = NULL;
    const struct DeviceResourceNode *macConfigNode = NULL;
    const struct DeviceResourceNode *phyConfigNode = NULL;

    if (node == NULL || modConfig == NULL) {
        HDF_LOGE("%s: invalid node or moduleConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL || drsOps->GetString == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetUint32(node, "featureMap", &modConfig->featureMap, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: featureMap fail!", __func__);
        return HDF_FAILURE;
    }
    ret = drsOps->GetString(node, "msgName", &modConfig->msgName, NULL);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: msgName fail!", __func__);
        return HDF_FAILURE;
    }

    staConfigNode = drsOps->GetChildNode(node, "Station");
    apConfigNode = drsOps->GetChildNode(node, "HostAp");
    p2pConfigNode = drsOps->GetChildNode(node, "P2P");
    macConfigNode = drsOps->GetChildNode(node, "Mac80211");
    phyConfigNode = drsOps->GetChildNode(node, "Phy");
    if (staConfigNode == NULL || apConfigNode == NULL || p2pConfigNode == NULL || macConfigNode == NULL ||
        phyConfigNode == NULL) {
        HDF_LOGE("%s: get child node fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanStaConfig(staConfigNode, &modConfig->station) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (ParseWlanApConfig(apConfigNode, &modConfig->hostAp) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (ParseWlanP2PConfig(p2pConfigNode, &modConfig->p2p) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (ParseWlanMac80211Config(macConfigNode, &modConfig->mac80211) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (ParseWlanPhyConfig(phyConfigNode, &modConfig->Phy) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    HDF_LOGD("%s: featureMap=%0x, msgName=%s", __func__, modConfig->featureMap, modConfig->msgName);
    return HDF_SUCCESS;
}
/* BEGIN for WLAN2.1 : Added by hdf-wlan */
static int32_t ParseWlanPowerConfig(const struct DeviceResourceNode *node,
    struct HdfConfigWlanPower *primaryPowerConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    if (node == NULL || primaryPowerConfig == NULL) {
        HDF_LOGE("%s: one of the input para is NULL!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "powerSeqDelay", &primaryPowerConfig->powerSeqDelay, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: powersSeqDelay fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "powerType", &primaryPowerConfig->powerType, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: powerType fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "gpioId", &primaryPowerConfig->gpioId, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: gpioId fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "activeLevel", &primaryPowerConfig->activeLevel, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: activeLevel fail!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

/* BEGIN for WLAN2.1 to get powers config: Added by hdf-wlan */
static int32_t ParseWlanPowersConfig(const struct DeviceResourceNode *node, struct HdfConfWlanPowers *powersConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *fstPowerNode = NULL;
    const struct DeviceResourceNode *secPowerNode = NULL;
    if (node == NULL || powersConfig == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    fstPowerNode = drsOps->GetChildNode(node, "power0");
    if (fstPowerNode == NULL) {
        HDF_LOGE("%s: get power0 config fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanPowerConfig(fstPowerNode, &powersConfig->power0) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    secPowerNode = drsOps->GetChildNode(node, "power1");
    if (secPowerNode == NULL) {
        HDF_LOGE("%s: get power1 config fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanPowerConfig(secPowerNode, &powersConfig->power1) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t ParseWlanResetConfig(const struct DeviceResourceNode *node, struct HdfConfWlanRest *resetConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    if (node == NULL || resetConfig == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "resetType", &resetConfig->resetType, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: powersSeqDelay fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "gpioId", &resetConfig->gpioId, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: gpioId fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "activeLevel", &resetConfig->activeLevel, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: powerType fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "resetHoldTime", &resetConfig->resetHoldTime, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: activeLevel fail!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ParseWlanBusConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanBus *busConfig)
{
    struct DeviceResourceIface *drsOps = NULL;

    if (node == NULL || busConfig == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL || drsOps->GetUint8Array == NULL || drsOps->GetUint16 == NULL ||
        drsOps->GetUint32 == NULL || drsOps->GetElemNum == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "busType", &busConfig->busType, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: busType fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8(node, "busIdx", &busConfig->busIdx, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: busType fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint8Array(node, "funcNum", busConfig->funcNum, BUS_FUNC_MAX, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: funcNum fail!", __func__);
        return HDF_FAILURE;
    }
    busConfig->funcNumSize = drsOps->GetElemNum(node, "funcNum");

    if (drsOps->GetUint16(node, "timeout", &busConfig->timeout, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: timeout fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint16(node, "blockSize", &busConfig->blockSize, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: timeout fail!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ParseWlanDevInstConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanDevInst *devLstConfig)
{
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *devPowerNode = NULL;
    const struct DeviceResourceNode *chipConfigNode = NULL;
    const struct DeviceResourceNode *resetNode = NULL;
    if (node == NULL || devLstConfig == NULL) {
        HDF_LOGE("%s: invalid node or devLstConfig!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint8 == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "deviceInstId", &devLstConfig->deviceInstId, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: deviceinstId fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint8(node, "bootUpTimeOut", &devLstConfig->bootUpTimeOut, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: bootUpTimeOut fail!", __func__);
        return HDF_FAILURE;
    }

    devPowerNode = drsOps->GetChildNode(node, "powers");
    if (devPowerNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanPowersConfig(devPowerNode, &devLstConfig->powers) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    resetNode = drsOps->GetChildNode(node, "reset");
    if (resetNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanResetConfig(resetNode, &devLstConfig->reset) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    chipConfigNode = drsOps->GetChildNode(node, "bus");
    if (chipConfigNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanBusConfig(chipConfigNode, &devLstConfig->bus) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t ParseWlanDevListConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanDeviceList *devConfig)
{
    struct DeviceResourceNode *childNode = NULL;
    uint32_t index = 0;
    if (node == NULL || devConfig == NULL) {
        HDF_LOGE("%s: invalid node or devConfig!", __func__);
        return HDF_FAILURE;
    }
    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseWlanDevInstConfig(childNode, &devConfig->deviceInst[index]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        index++;
        devConfig->deviceListSize++;
    }
    HDF_LOGD("%s: deviceListSize=%d", __func__, devConfig->deviceListSize);
    return HDF_SUCCESS;
}

/* BEGIN for WLAN2.1 to get chips configures: Added by hdf-wlan */
static int32_t ParseWlanChipSdioConfig(const struct DeviceResourceNode *node, struct HdfConfWlanSdioArgs *sdioArgs)
{
    struct DeviceResourceIface *drsOps = NULL;
    if (node == NULL || sdioArgs == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint16 == NULL || drsOps->GetUint16Array == NULL) {
        HDF_LOGE("%s: at least one of the paras is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetUint16(node, "vendorId", &sdioArgs->vendorId, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: vendorId fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetUint16Array(node, "deviceId", sdioArgs->deviceId, CHIP_SDIO_DEVICE_ID_COUNT, 0) != HDF_SUCCESS) {
        HDF_LOGE("%s: deviceId fail!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t ParseWlanChipsCompsConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanChipInst *chipInst)
{
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *chipBusNode = NULL;
    if (node == NULL || chipInst == NULL) {
        HDF_LOGE("%s: invalid node or busRegsConfig!\n", __func__);
        return HDF_FAILURE;
    }
    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }

    if (drsOps->GetString(node, "driverName", &chipInst->driverName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: vendorId fail!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: driverName=%s", __func__, chipInst->driverName);

    chipBusNode = drsOps->GetChildNode(node, "sdio");
    if (chipBusNode == NULL) {
        HDF_LOGE("%s: GetChildNode fail!", __func__);
        return HDF_FAILURE;
    }
    if (ParseWlanChipSdioConfig(chipBusNode, &chipInst->chipSdio) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}
/* parse wlan chipList config */
static int32_t ParseWlanChipsConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanChipList *chipList)
{
    struct DeviceResourceNode *childNode = NULL;
    uint32_t cnt = 0;
    if (node == NULL || chipList == NULL) {
        HDF_LOGE("%s: invalid node or chipList!\n", __func__);
        return HDF_FAILURE;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(node, childNode)
    {
        if (ParseWlanChipsCompsConfig(childNode, &chipList->chipInst[cnt]) != HDF_SUCCESS) {
            return HDF_FAILURE;
        }
        cnt++;
        chipList->chipInstSize++;
    }
    HDF_LOGD("%s: chipSize=%hu", __func__, chipList->chipInstSize);

    return HDF_SUCCESS;
}
/* END for WLAN2.1 chips configuration parse : Added by hdf-wlan */

static int32_t ParseWlanConfig(const struct DeviceResourceNode *node, struct HdfConfigWlanConfig *wlanConfig)
{
    int32_t ret;
    struct DeviceResourceIface *drsOps = NULL;
    const struct DeviceResourceNode *moduleConfigNode = NULL;
    const struct DeviceResourceNode *devListNode = NULL;
    const struct DeviceResourceNode *chipsNode = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetChildNode == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!", __func__);
        return HDF_FAILURE;
    }
    if (drsOps->GetString(node, "hostChipName", &wlanConfig->hostChipName, NULL) != HDF_SUCCESS) {
        HDF_LOGE("%s: hostChipName fail!", __func__);
        return HDF_FAILURE;
    }
    moduleConfigNode = drsOps->GetChildNode(node, "moduleConfig");
    devListNode = drsOps->GetChildNode(node, "deviceList");
    chipsNode = drsOps->GetChildNode(node, "chipList");
    if (moduleConfigNode == NULL || devListNode == NULL || chipsNode == NULL) {
        HDF_LOGE("%s: get child node fail!", __func__);
        return HDF_FAILURE;
    }
    ret = ParseWlanModuleConfig(moduleConfigNode, &wlanConfig->moduleConfig);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ret = ParseWlanDevListConfig(devListNode, &wlanConfig->deviceList);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ret = ParseWlanChipsConfig(chipsNode, &wlanConfig->chipList);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return ret;
}

int32_t HdfParseWlanConfig(const struct DeviceResourceNode *node)
{
    if (node == NULL) {
        HDF_LOGE("%s: node = null!\n", __func__);
        return HDF_FAILURE;
    }
    return ParseWlanConfig(node, &g_configWlanModuleRoot.wlanConfig);
}

struct HdfConfigWlanRoot *HdfWlanGetModuleConfigRoot(void)
{
    return &g_configWlanModuleRoot;
}
