/*
 * Copyright (c) 2021-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "eth_chip_driver.h"
#include "eth_device.h"
#include "osal.h"

#define HDF_LOG_TAG eth_core
struct EthConfig *g_ethConfig = NULL;

static struct HdfEthChipDriverFactory *HdfEthGetDriverFactory(const char *driverName)
{
    struct HdfEthChipDriverManager *initMgr = NULL;
    if (driverName == NULL) {
        HDF_LOGE("%s: driverName is ", __func__);
        return NULL;
    }
    initMgr = HdfEthGetChipDriverMgr();
    if (initMgr == NULL) {
        HDF_LOGE("%s: initMgr is NULL", __func__);
        return NULL;
    }
    return initMgr->GetEthChipDriverByName(driverName);
}

static int32_t DeinitEth(struct EthDevice *ethDevice)
{
    HDF_LOGD("%s enter", __func__);
    struct HdfEthChipDriverFactory *ethChipDriverFact = NULL;

    if (ethDevice == NULL) {
        HDF_LOGE("%s the input ethDevice is NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ethChipDriverFact = HdfEthGetDriverFactory(ethDevice->name);
    if (ethChipDriverFact == NULL) {
        HDF_LOGE("%s: get ethChipDriverFact failed! driverName = %s", __func__, ethDevice->name);
        return HDF_FAILURE;
    }

    if (ethChipDriverFact->DeinitEthDriver != NULL) {
        return ethChipDriverFact->DeinitEthDriver(ethDevice);
    }
    return HDF_SUCCESS;
}

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
static int32_t SetEthNetworkAddr(struct NetDevice *netDev)
{
    IpV4Addr ip, netmask, gw;

    ip.addr = 0x0A01a8c0UL;      /* 192, 168, 1. 10 */
    netmask.addr = 0x00ffffffUL; /* 255, 255, 255, 0 */
    gw.addr = 0x0101a8c0UL;      /* 192, 168, 12, 1 */

    if (NetIfSetAddr(netDev, &ip, &netmask, &gw) != HDF_SUCCESS) {
        HDF_LOGE("%s fail: NetIfSetAddr error!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
#endif

static int32_t InitMacInterface(struct EthDevice *ethDevice, struct HdfEthMacChipDriver *macChipDriver)
{
    int32_t ret;

    if (macChipDriver->ethMacOps == NULL) {
        HDF_LOGE("%s:ethMacOps is NULL.", __func__);
        return HDF_FAILURE;
    }
    if (macChipDriver->ethMacOps->MacInit == NULL) {
        HDF_LOGE("%s:MacInit is not implement.", __func__);
        return HDF_FAILURE;
    }
    macChipDriver->ethMacOps->MacInit();

    if (macChipDriver->ethMacOps->PortReset == NULL) {
        HDF_LOGE("%s:PortReset is not implement.", __func__);
        return HDF_FAILURE;
    }
    ret = macChipDriver->ethMacOps->PortReset(ethDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:PortReset failed! ret = %d.", __func__, ret);
        return HDF_FAILURE;
    }

    if (macChipDriver->ethMacOps->PortInit == NULL) {
        HDF_LOGE("%s:PortInit is not implement.", __func__);
        return HDF_FAILURE;
    }
    ret = macChipDriver->ethMacOps->PortInit(ethDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s:PortInit failed! ret = %d.", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t InitEth(struct EthDevice *ethDevice, const uint8_t isSetDefault,
                       struct HdfEthChipDriverFactory *ethChipDriverFact)
{
    int32_t ret;
    struct HdfEthNetDeviceData *data = NULL;
    struct HdfEthMacChipDriver *macChipDriver = NULL;
    unsigned char enaddr[MAC_ADDR_SIZE] = {0};

    macChipDriver = ethChipDriverFact->BuildMacDriver();
    if (macChipDriver == NULL) {
        HDF_LOGE("%s:mac chip driver build fail!", __func__);
        return HDF_FAILURE;
    }
    data = GetEthNetDeviceData(ethDevice->netdev);
    if (data == NULL) {
        HDF_LOGE("%s: data is NULL!", __func__);
        ethChipDriverFact->ReleaseMacDriver(macChipDriver);
        return HDF_FAILURE;
    }
    data->macChipDriver = macChipDriver;

    ret = ethChipDriverFact->InitEthDriver(ethDevice);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: failed to init eth phy", __func__);
        DeinitEth(ethDevice);
        ReleaseEthDevice(ethDevice);
        return ret;
    }

    ret = InitMacInterface(ethDevice, macChipDriver);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InitMacInterface error!", __func__);
        DeinitEth(ethDevice);
        ReleaseEthDevice(ethDevice);
        return HDF_FAILURE;
    }
    OsalMSleep(DELAY_TIME_LONG);

    ethChipDriverFact->GetMacAddr(enaddr, MAC_ADDR_SIZE);
    NetIfSetMacAddr(ethDevice->netdev, enaddr, MAC_ADDR_SIZE);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    ret = SetEthNetworkAddr(ethDevice->netdev);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s set eth network addr fail", __func__);
        DeinitEth(ethDevice);
        ReleaseEthDevice(ethDevice);
        return ret;
    }
#endif
    NetIfSetStatus(ethDevice->netdev, NETIF_UP);
    return HDF_SUCCESS;
}

static int32_t HdfEthDriverInit(struct HdfDeviceObject *deviceObject)
{
    int32_t ret;
    uint8_t i;

    if (deviceObject == NULL) {
        HDF_LOGE("%s deviceObject is NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    g_ethConfig = GetEthConfig(deviceObject->property);
    if (g_ethConfig == NULL) {
        HDF_LOGE("%s failed to get g_ethConfig!", __func__);
        return HDF_FAILURE;
    }
    for (i = 0; i < g_ethConfig->deviceListSize; i++) {
        struct EthDevice *ethDevice = CreateEthDevice(&g_ethConfig->deviceInst[i]);
        if (ethDevice == NULL) {
            return HDF_FAILURE;
        }
        ethDevice->config = &g_ethConfig->deviceInst[i];

        struct HdfEthChipDriverFactory *ethChipDriverFact = HdfEthGetDriverFactory(ethDevice->name);
        if (ethChipDriverFact == NULL) {
            HDF_LOGE("%s: get ethChipDriverFact failed! driverName = %s", __func__, ethDevice->name);
            ReleaseEthDevice(ethDevice);
            return HDF_FAILURE;
        }
        ret = InitEth(ethDevice, g_ethConfig->deviceInst[i].isSetDefault, ethChipDriverFact);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s failed to init eth driver, ret: %d", __func__, ret);
            return ret;
        }
    }
    HDF_LOGE("%s hdf eth driver framework init success", __func__);
    return ret;
}

static int32_t HdfEthDriverBind(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
    return HDF_SUCCESS;
}

static void HdfEthDriverRelease(struct HdfDeviceObject *deviceObject)
{
    (void)deviceObject;
}

struct HdfDriverEntry g_ethEntry = {
    .moduleVersion = 1,
    .Bind = HdfEthDriverBind,
    .Init = HdfEthDriverInit,
    .Release = HdfEthDriverRelease,
    .moduleName = "HDF_ETHERNET"
};

HDF_INIT(g_ethEntry);