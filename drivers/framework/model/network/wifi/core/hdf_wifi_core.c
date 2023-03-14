/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "securec.h"
#include "gpio_if.h"
#include "hdf_wifi_core.h"
#include "devsvc_manager_clnt.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "hdf_wifi_cmd.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "osal_thread.h"
#include "message/message_router.h"
#include "hdf_wlan_chipdriver_manager.h"
#include "hdf_wlan_sdio.h"
#include "hdf_wlan_config.h"
#include "hdf_wlan_utils.h"

#define HDF_LOG_TAG HDF_WIFI_CORE
#define SLEEPTIME 50

int32_t HdfWifiGetBusIdx(void)
{
    struct HdfConfigWlanRoot *rootConfig = NULL;
    rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig != NULL) {
        return rootConfig->wlanConfig.deviceList.deviceInst[0].bus.busIdx;
    }
    return -1;
}

/**
 * @brief  as for now, we just support one wlan module in one board cause driver binds to wlan featere
 * due to that reason, once we detected one chip, we stop rescan.
 */
static int HdfWlanBusInit(struct HdfWlanDevice *data, const struct HdfConfigWlanBus *busConfig)
{
    struct BusDev *bus = NULL;
    bus = HdfWlanCreateBusManager(busConfig);
    if (bus == NULL) {
        HDF_LOGE("%s:Create bus manager failed!", __func__);
        return HDF_FAILURE;
    }
    data->bus = bus;
    if (bus->priData.driverName == NULL) {
        HDF_LOGE("%s:get driver name failed!", __func__);
        return HDF_FAILURE;
    }
    data->driverName = bus->priData.driverName;
    HDF_LOGI("%s: driver name = %s", __func__, data->driverName);
    return HDF_SUCCESS;
}

static int32_t HdfWifiDriverBind(struct HdfDeviceObject *dev)
{
    ErrorCode errCode;
    static struct IDeviceIoService wifiService = {
        .object.objectId = 1,
        .Dispatch = DispatchToMessage,
    };

    if (dev == NULL) {
        return HDF_FAILURE;
    }

    errCode = StartMessageRouter(MESSAGE_NODE_CONFIG_DEFAULT);
    if (errCode != ME_SUCCESS) {
        HDF_LOGE("%s:Start message router failed!", __func__);
        return HDF_FAILURE;
    }

    errCode = EnableDefaultDispatcher();
    if (errCode != ME_SUCCESS) {
        (void)ShutdownMessageRouter();
        HDF_LOGE("%s:EnableDefaultDispatcher failed!", __func__);
        return errCode;
    }
    dev->service = &wifiService;
    return HDF_SUCCESS;
}

static struct HdfConfigWlanDeviceList *HdfWlanGetDeviceList(void)
{
    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        HDF_LOGE("%s: HdfWlanGetModuleConfigRoot get failed", __func__);
        return NULL;
    }
    return &rootConfig->wlanConfig.deviceList;
}

/* CreatePowerMgrByConfig,call by HdfWlanMainInit */
struct PowerManager *HdfWlanCreatePowerMgrByConfig(void)
{
    struct HdfConfWlanPowers *powerConfig = NULL;
    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        HDF_LOGE("%s: HdfWlanGetModuleConfigRoot get failed", __func__);
        return NULL;
    }
    powerConfig = &rootConfig->wlanConfig.deviceList.deviceInst[0].powers;
    return HdfWlanCreatePowerManager(powerConfig);
}

struct ResetManager *CreateResetMgrByConfig(void)
{
    uint8_t bootUpTimeOut;
    struct HdfConfWlanRest *resetConfig = NULL;
    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        HDF_LOGE("%s: HdfWlanGetModuleConfigRoot get failed", __func__);
        return NULL;
    }
    resetConfig = &rootConfig->wlanConfig.deviceList.deviceInst[0].reset;
    bootUpTimeOut = rootConfig->wlanConfig.deviceList.deviceInst[0].bootUpTimeOut;
    return HdfWlanCreateResetManager(resetConfig, bootUpTimeOut);
}

/* get wlan related config */
static int32_t HdfWlanGetConfig(const struct HdfDeviceObject *device)
{
    if (device == NULL) {
        HDF_LOGE("%s: input pointer is null", __func__);
        return HDF_FAILURE;
    }
    if (HdfParseWlanConfig(device->property) != HDF_SUCCESS) {
        HDF_LOGE("%s:ParseWifiConfig failed!", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

#ifndef CONFIG_DRIVERS_HDF_NETDEV_EXT
static int32_t HdfWlanPowerOnProcess(struct PowerManager *powerMgr)
{
    if (powerMgr == NULL) {
        HDF_LOGE("%s:Input is NULL!", __func__);
        return HDF_FAILURE;
    }

    if (powerMgr->On(powerMgr) != HDF_SUCCESS) {
        HDF_LOGE("%s:power on failed!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGW("%s:Chip power on!", __func__);
    return HDF_SUCCESS;
}
static int32_t HdfWlanResetProcess(struct ResetManager *resetMgr)
{
    if (resetMgr == NULL) {
        HDF_LOGE("%s:input is NULL!", __func__);
        return HDF_FAILURE;
    }
    if (resetMgr->Reset(resetMgr) != HDF_SUCCESS) {
        HDF_LOGE("%s:reset failed!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGW("%s:Chip reset success!", __func__);
    return HDF_SUCCESS;
}
#endif

static struct HdfChipDriverFactory *HdfWlanGetDriverFactory(const char *driverName)
{
    struct HdfChipDriverManager *initMgr = NULL;
    if (driverName == NULL) {
        HDF_LOGE("%s: chipName is NULL", __func__);
        return NULL;
    }
    initMgr = HdfWlanGetChipDriverMgr();
    if (initMgr == NULL) {
        HDF_LOGE("%s: initMgr is NULL", __func__);
        return NULL;
    }
    return initMgr->GetChipDriverByName(driverName);
}

static int32_t HdfWlanDeinitInterface(struct HdfWlanDevice *device, const char *ifName,
    struct HdfChipDriverFactory *factory)
{
    int32_t ret;
    struct NetDevice *netdev = NULL;
    struct HdfChipDriver *chipDriver = NULL;

    netdev = NetDeviceGetInstByName(ifName);
    if (netdev == NULL) {
        HDF_LOGW("%s:netif already released!ifName=%s", __func__, ifName);
        return HDF_SUCCESS;
    }
    chipDriver = GetChipDriver(netdev);
    if (chipDriver == NULL) {
        HDF_LOGE("%s:bad netdev!ChipDriver is NULL. ifName=%s", __func__, ifName);
        return HDF_FAILURE;
    }
    ret = chipDriver->deinit(chipDriver, netdev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:chip driver deinit failed!ifName=%s", __func__, ifName);
        return ret;
    }
    // check if chip driver released NetDevice
    if (NetDeviceGetInstByName(ifName) != NULL) {
        ret = ReleasePlatformNetDevice(netdev);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:release netdevice failed!ifName=%s", __func__, ifName);
            return ret;
        }
    } else {
        HDF_LOGW("%s:chip driver release platform netdevice detected!ifName=%s", __func__, ifName);
    }

    if (factory->Release != NULL) {
        factory->Release(chipDriver);
    }
    chipDriver = NULL;

    return HDF_SUCCESS;
}

static int32_t HdfWlanInitInterface(struct HdfWlanDevice *device, struct HdfChipDriverFactory *factory, uint8_t index)
{
    int32_t ret;
    NetDevice *netDev = NULL;
    struct HdfChipDriver *chipDriver = NULL;
    do {
        struct HdfWifiNetDeviceData *data = NULL;
        chipDriver = factory->Build(device, index);
        if (chipDriver == NULL) {
            HDF_LOGE("%s:chip driver %s build fail!", __func__, factory->driverName);
            ret = HDF_FAILURE;
            break;
        }

        if (chipDriver->init == NULL) {
            HDF_LOGI("%s: chip driver %s not 'init' api", __func__, factory->driverName);
            ret = HDF_DEV_ERR_OP;
            break;
        }

        netDev = AllocPlatformNetDevice(device);
        if (netDev == NULL) {
            HDF_LOGE("%s:allocate net device failed!", __func__);
            ret = HDF_FAILURE;
            break;
        }
        data = GetPlatformData(netDev);
        if (data == NULL) {
            HDF_LOGE("%s:platform data is NULL!", __func__);
            ret = HDF_FAILURE;
            break;
        }
        data->device = device;

        ret = chipDriver->init(chipDriver, netDev);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:init chip %s failed! ret=%d", __func__, factory->driverName, ret);
            break;
        }

        data->chipDriver = chipDriver;
        chipDriver = NULL;
    } while (false);

    if (ret != HDF_SUCCESS) {
        if (chipDriver != NULL && factory->Release != NULL) {
            factory->Release(chipDriver);
        }
        if (netDev != NULL) {
            ReleasePlatformNetDevice(netDev);
        }
    }
    return ret;
}

static int32_t HdfWlanDeinitInterfaces(struct HdfWlanDevice *device, struct HdfChipDriverFactory *chipDriverFact)
{
    int32_t ret = HDF_SUCCESS;
    char *ifNames = NULL;
    uint8_t ifNameCount = 0;
    uint8_t i;

    ifNames = HdfWlanGetIfNames(device->id, &ifNameCount);
    if (ifNames == NULL) {
        HDF_LOGE("%s: HdfWlanGetIfNames failed!", __func__);
        return HDF_FAILURE;
    }

    for (i = 0; i < ifNameCount; i++) {
        ret = HdfWlanDeinitInterface(device, ifNames + (i * IFNAMSIZ), chipDriverFact);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Deinit netif %s failed!ret=%d", __func__, ifNames + (i * IFNAMSIZ), ret);
            break;
        }
    }
    OsalMemFree(ifNames);
    ifNames = NULL;
    return ret;
}


/* build chip driver according to the detected chip and the load factory */
static int32_t HdfWlanInitInterfaces(struct HdfWlanDevice *device, struct HdfChipDriverFactory *chipDriverFact)
{
    int32_t ret = HDF_SUCCESS;
    uint8_t i;
    uint8_t maxIFCount = chipDriverFact->GetMaxIFCount(chipDriverFact);
    for (i = 0; i < maxIFCount; i++) {
        ret = HdfWlanInitInterface(device, chipDriverFact, i);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Init NetInterface failed!driverName=%s,portIndex=%hhu", __func__, device->driverName, i);
        }
    }
    HDF_LOGI("%s:%s init interfaces finished!", __func__, chipDriverFact->driverName);
    return ret;
}

static void ReleaseWlanDevice(struct HdfWlanDevice *device)
{
    if (device->powers != NULL && device->powers->Release != NULL) {
        device->powers->Release(device->powers);
        device->powers = NULL;
    }
    if (device->reset != NULL && device->reset->Release != NULL) {
        device->reset->Release(device->reset);
        device->reset = NULL;
    }
    if (device->bus != NULL && device->bus->ops.deInit != NULL) {
        device->bus->ops.deInit(device->bus);
        device->bus = NULL;
    }
    OsalMemFree(device);
}

static struct HdfWlanDevice *ProbeDevice(struct HdfConfigWlanDevInst *deviceConfig)
{
    if (deviceConfig == NULL) {
        return NULL;
    }
    struct HdfWlanDevice *device = NULL;
    int32_t ret;
    struct HdfConfigWlanChipList *tmpChipList = NULL;
    struct HdfConfigWlanRoot *rootConfig = HdfWlanGetModuleConfigRoot();
    if (rootConfig == NULL) {
        HDF_LOGE("%s:ProbeDevice rootConfig NULL", __func__);
        return NULL;
    }
    device = (struct HdfWlanDevice *)OsalMemCalloc(sizeof(struct HdfWlanDevice));
    if (device == NULL) {
        HDF_LOGE("%s:ProbeDevice device NULL", __func__);
        return NULL;
    }

    tmpChipList = &rootConfig->wlanConfig.chipList;
    do {
        device->powers = HdfWlanCreatePowerManager(&deviceConfig->powers);
        device->reset = HdfWlanCreateResetManager(&deviceConfig->reset, deviceConfig->bootUpTimeOut);

#ifndef CONFIG_DRIVERS_HDF_NETDEV_EXT
        ret = HdfWlanPowerOnProcess(device->powers);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:HdfWlanPowerOnProcess failed!", __func__);
            break;
        }

        ret = HdfWlanResetProcess(device->reset);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:HdfWlanResetProcess failed!", __func__);
            break;
        }

        ret = HdfWlanBusInit(device, &deviceConfig->bus);
#else
        ret = HDF_SUCCESS;
        OsalMSleep(SLEEPTIME);
        device->bus = NULL;
        device->driverName = tmpChipList->chipInst[0].driverName;  // from BDH6_DRIVER_NAME
        HDF_LOGW("Do not call GPIO and HdfWlanBusInit");
#endif

        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:NO Sdio Card in hdf wlan init proc", __func__);
            break;
        }
    } while (false);

    if (ret != HDF_SUCCESS) {
        ReleaseWlanDevice(device);
        return NULL;
    }
    return device;
}

int32_t HdfWifiDeinitDevice(struct HdfWlanDevice *device)
{
    struct HdfChipDriverFactory *chipDriverFact = NULL;
    int32_t ret;
    if (device == NULL) {
        HDF_LOGE("%s:Input is NULL!", __func__);
        return HDF_FAILURE;
    }
    chipDriverFact = HdfWlanGetDriverFactory(device->driverName);
    if (chipDriverFact == NULL) {
        HDF_LOGE("%s: get chipDriverFact failed! driverName=%s", __func__, device->driverName);
        return HDF_FAILURE;
    }
    ret = HdfWlanDeinitInterfaces(device, chipDriverFact);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:Deinit interface of device %d failed!ret=%d", __func__, device->id, ret);
        return ret;
    }

    if (chipDriverFact->DeinitChip != NULL) {
        return chipDriverFact->DeinitChip(device);
    }
    return HDF_SUCCESS;
}

int32_t HdfWifiInitDevice(struct HdfWlanDevice *device)
{
    int32_t ret;
    struct HdfChipDriverFactory *chipDriverFact = NULL;

    chipDriverFact = HdfWlanGetDriverFactory(device->driverName);
    if (chipDriverFact == NULL) {
        HDF_LOGE("%s: get chipDriverFact failed! driverName=%s", __func__, device->driverName);
        return HDF_FAILURE;
    }
    if (chipDriverFact->InitChip != NULL) {
        ret = chipDriverFact->InitChip(device);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:Init chip failed! ret=%d!", __func__, ret);
            return ret;
        }
    }

    ret = HdfWlanInitInterfaces(device, chipDriverFact);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:init interfaces failed! ret=%d!", __func__, ret);
    } else {
        HDF_LOGD("%s:init interfaces success! netIfMap=%x.", __func__, device->netIfMap);
    }
    return ret;
}

/* thread callback function */
static int32_t HdfWlanInitThread(void *para)
{
    const uint32_t initDelaySec = 15;
    struct HdfDeviceObject *device = (struct HdfDeviceObject *)para;
    struct SubscriberCallback callback = { NULL };
    struct HdfConfigWlanDeviceList *devList = NULL;
    struct HdfWlanDevice *wlanDevice = NULL;
    uint16_t i;
    int32_t ret;

    OsalSleep(initDelaySec);
    HDF_LOGW("Driver HdfWiFi initing...");
    if (device == NULL) {
        return HDF_FAILURE;
    }

    devList = HdfWlanGetDeviceList();
    if (devList == NULL) {
        HDF_LOGW("%s:No device defined.", __func__);
        return HDF_SUCCESS;
    }
    for (i = 0; i < devList->deviceListSize; i++) {
#ifndef CONFIG_DRIVERS_HDF_NETDEV_EXT
        ret = HdfWlanConfigSDIO(devList->deviceInst[i].bus.busIdx);
#else
        ret = HDF_SUCCESS;
#endif
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:HdfWlanConfigSDIO %d failed!ret=%d", __func__, devList->deviceInst[i].bus.busIdx, ret);
            continue;
        }

        wlanDevice = ProbeDevice(&(devList->deviceInst[i]));
        if (wlanDevice == NULL) {
            HDF_LOGE("%s:Device %d detect failed!", __func__, i);
            continue;
        }

        ret = HdfWlanAddDevice(wlanDevice);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s:add device failed!", __func__);
            ReleaseWlanDevice(wlanDevice);
            continue;
        }

        // Load chip driver
        (void)DevSvcManagerClntSubscribeService(wlanDevice->driverName, callback);
        (void)HdfWifiInitDevice(wlanDevice);
    }

    HDF_LOGV("%s:finished.", __func__);
    return HDF_SUCCESS;
}
/* load chip factory thread */
static int32_t HdfWlanScanAndInitThread(struct HdfDeviceObject *device)
{
    struct OsalThread factoryTread;
    struct OsalThreadParam chipLoadCfg = {
        .name = "chip_factory_load",
        .priority = OSAL_THREAD_PRI_DEFAULT,
        .stackSize = 0x2000,
    };

    int32_t ret = OsalThreadCreate(&factoryTread, HdfWlanInitThread, (void *)device);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: fail to create thread", __func__);
        return HDF_ERR_THREAD_CREATE_FAIL;
    }
    ret = OsalThreadStart(&factoryTread, &chipLoadCfg);
    if (ret != HDF_SUCCESS) {
        /* lay stress on the use of threadcreate */
        HDF_LOGE("LoadChipFactoryThread: start LoadChipFactoryThread thread fail:%d", ret);
        return HDF_DEV_ERR_DEV_INIT_FAIL;
    }
    return HDF_SUCCESS;
}

/* main init process including config, powers, load the factory, and chip init */
static int32_t HdfWlanMainInit(struct HdfDeviceObject *device)
{
    struct HdfConfigWlanRoot *rootConfig = NULL;
    const struct HdfConfigWlanModuleConfig *moduleConfig = NULL;

    HDF_LOGV("%s:start..", __func__);
    if (device == NULL) {
        return HDF_FAILURE;
    }
    if (HdfWlanGetConfig(device) != HDF_SUCCESS) {
        HDF_LOGE("%s:HdfWlanGetConfig get wlan config failed!", __func__);
        return HDF_FAILURE;
    }
    /* feature init */
    rootConfig = HdfWlanGetModuleConfigRoot();
    moduleConfig = &rootConfig->wlanConfig.moduleConfig;
    if (HdfWlanInitProduct(device, moduleConfig) != HDF_SUCCESS) {
        HDF_LOGE("%s:HdfWlanInitProduct failed!", __func__);
        return HDF_FAILURE;
    }
    if (HdfWlanScanAndInitThread(device) != HDF_SUCCESS) {
        HDF_LOGE("%s: LoadChipFactoryThread failed, the load process failed!", __func__);
        return HDF_FAILURE;
    }
    HDF_LOGV("%s:finished.", __func__);
    return HDF_SUCCESS;
}
/* release the resource created in the init process */
static void HdfWlanDriverRelease(struct HdfDeviceObject *object)
{
    ErrorCode errCode;
    HDF_LOGW("Driver HdfWiFi deiniting...");
    (void)object;

    errCode = ShutdownMessageRouter();
    if (errCode != ME_SUCCESS) {
        HDF_LOGE("%s:shutdown router failed!err=%d", __func__, errCode);
    }
    HDF_LOGW("Driver HdfWiFi deinited.");
}

struct HdfDriverEntry g_hdfWifiEntry = {
    .moduleVersion = 1,
    .Bind = HdfWifiDriverBind,
    .Init = HdfWlanMainInit,
    .Release = HdfWlanDriverRelease,
    .moduleName = "HDF_WIFI"
};

HDF_INIT(g_hdfWifiEntry);

