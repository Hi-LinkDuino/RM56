/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "net_device.h"
#include "securec.h"

#include "osal_mem.h"
#include "net_device_impl.h"
#include "net_device_adapter.h"

#define HDF_LOG_TAG "NetDevice"

static struct NetDeviceImpl *g_netDeviceImplTable[MAX_NETDEVICE_COUNT] = {NULL};

static bool FindAvailableTable(uint32_t *index)
{
    uint32_t i;

    if (index == NULL) {
        HDF_LOGE("%s Find Available table index error!", __func__);
        return false;
    }
    for (i = 0; i < MAX_NETDEVICE_COUNT; i++) {
        if (g_netDeviceImplTable[i] == NULL) {
            *index = i;
            return true;
        }
    }
    return false;
}

static bool AddNetDeviceImplToTable(uint32_t index, struct NetDeviceImpl *netDeviceImpl)
{
    if (index >= MAX_NETDEVICE_COUNT) {
        HDF_LOGE("%s error because of not enough space!", __func__);
        return false;
    }
    g_netDeviceImplTable[index] = netDeviceImpl;
    return true;
}

static void DeleteNetDeviceImplFromTable(const struct NetDeviceImpl *netDeviceImpl)
{
    int32_t i;

    for (i = 0; i < MAX_NETDEVICE_COUNT; i++) {
        if (g_netDeviceImplTable[i] == netDeviceImpl) {
            g_netDeviceImplTable[i] = NULL;
            return;
        }
    }
    return;
}

static struct NetDeviceImpl *InitNetDeviceImpl(NetDevice *nd, NetIfCategory ifCategory)
{
    struct NetDeviceImpl *ndImpl = NULL;
    if (nd == NULL) {
        return NULL;
    }
    ndImpl = (struct NetDeviceImpl *)OsalMemCalloc(sizeof(struct NetDeviceImpl));
    if (ndImpl == NULL) {
        HDF_LOGE("%s fail: OsalMemCalloc fail!", __func__);
        return NULL;
    }

    ndImpl->netDevice = nd;
    if (RegisterNetDeviceImpl(ndImpl) != HDF_SUCCESS) {
        HDF_LOGE("%s fail: resiter lite impl fail!", __func__);
        OsalMemFree(ndImpl);
        ndImpl = NULL;
    }
    return ndImpl;
}

static void DeInitNetDeviceImpl(struct NetDeviceImpl *netDeviceImpl)
{
    if (netDeviceImpl == NULL) {
        HDF_LOGE("%s success : already free!", __func__);
        return;
    }

    /* release osPrivate */
    if (netDeviceImpl->interFace != NULL && netDeviceImpl->interFace->deInit != NULL) {
        netDeviceImpl->interFace->deInit(netDeviceImpl);
    }

    /* step 1 : release interFace. step 2 : release netdevice */
    if (netDeviceImpl->netDevice != NULL) {
        UnRegisterNetDeviceImpl(netDeviceImpl);
        OsalMemFree(netDeviceImpl->netDevice);
        netDeviceImpl->netDevice = NULL;
    }

    /* last release netDeviceImpl */
    OsalMemFree(netDeviceImpl);
    HDF_LOGI("%s success!", __func__);
    return;
}

static struct NetDeviceImpl *GetImplByNetDevice(const struct NetDevice *netDevice)
{
    struct NetDeviceImpl *ndImpl = NULL;
    int32_t i;

    for (i = 0; i < MAX_NETDEVICE_COUNT; i++) {
        if (g_netDeviceImplTable[i] != NULL && g_netDeviceImplTable[i]->netDevice != NULL) {
            if (netDevice == g_netDeviceImplTable[i]->netDevice) {
                ndImpl = g_netDeviceImplTable[i];
                return ndImpl;
            }
        }
    }
    HDF_LOGE("%s Get Impl by netdevice failed", __func__);
    return ndImpl;
}

#ifdef CONFIG_DRIVERS_HDF_NETDEV_EXT
struct net_device *GetLinuxInfByNetDevice(const struct NetDevice *netDevice)
{
    struct NetDeviceImpl *impl = NULL;
    struct FullNetDevicePriv *priv = NULL;
    
    impl = GetImplByNetDevice(netDevice);
    if (impl == NULL || impl->osPrivate == NULL) {
        return NULL;
    }
    
    priv = (struct FullNetDevicePriv *)impl->osPrivate;
    return priv->dev;
}

struct NetDevice *GetHdfNetDeviceByLinuxInf(struct net_device *dev)
{
    struct NetDevice *netDev = NULL;
    struct FullNetDevicePriv *priv = (struct FullNetDevicePriv *)netdev_priv(dev);
    netDev = priv->impl->netDevice;
    return netDev;
}
#endif

struct NetDevice *NetDeviceInit(const char *ifName, uint32_t len, NetLinkType type, NetIfCategory ifCategory)
{
    NetDevice *netDevice = NULL;
    struct NetDeviceImpl *ndImpl = NULL;
    uint32_t index = 0;
    int32_t ret;

    if ((ifName == NULL) || (strlen(ifName) != len) || (strlen(ifName) > IFNAMSIZ - 1)) {
        HDF_LOGE("%s fail: ifName = null or len not right!", __func__);
        return NULL;
    }
    netDevice = (NetDevice *)OsalMemCalloc(sizeof(NetDevice));
    if (netDevice == NULL) {
        HDF_LOGE("%s fail: OsalMemCalloc fail!", __func__);
        return NULL;
    }
    if (strcpy_s(netDevice->name, IFNAMSIZ, ifName) != EOK) {
        HDF_LOGE("%s fail: strcpy_s fail!", __func__);
        OsalMemFree(netDevice);
        return NULL;
    }
    netDevice->netifCateg = ifCategory;
    ndImpl = InitNetDeviceImpl(netDevice, ifCategory);
    if (ndImpl == NULL) {
        HDF_LOGE("%s fail: InitNetDeviceImpl fail!", __func__);
        OsalMemFree(netDevice);
        return NULL;
    }
    if (FindAvailableTable(&index)) {
        AddNetDeviceImplToTable(index, ndImpl);
    } else {
        DeInitNetDeviceImpl(ndImpl);
        HDF_LOGE("%s fail: Not extra table.", __func__);
        return NULL;
    }
    /* INIT OSPrivate */
    ret = HDF_FAILURE;
    if (ndImpl->interFace != NULL && ndImpl->interFace->init != NULL) {
        ret = ndImpl->interFace->init(ndImpl);
    }
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s fail : interface->init fail!", __func__);
        DeleteNetDeviceImplFromTable(ndImpl);
        DeInitNetDeviceImpl(ndImpl);
        return NULL;
    }
    netDevice->LinkLayerType = type;
    HDF_LOGI("Init Net Device success!");
    return netDevice;
}

int32_t NetDeviceDeInit(struct NetDevice *netDevice)
{
    struct NetDeviceImpl *ndImpl = NULL;

    if (netDevice == NULL) {
        HDF_LOGI("%s success: already deinit!", __func__);
        return HDF_SUCCESS;
    }
    ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl == NULL) {
        HDF_LOGI("%s success: already free.", __func__);
        OsalMemFree(netDevice);
        return HDF_SUCCESS;
    }
    DeleteNetDeviceImplFromTable(ndImpl);
    DeInitNetDeviceImpl(ndImpl);
    return HDF_SUCCESS;
}

int32_t NetDeviceAdd(struct NetDevice *netDevice)
{
    struct NetDeviceImplOp *op = NULL;
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);

    if (ndImpl == NULL) {
        HDF_LOGE("%s fail: netDevice not exist!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    op = ndImpl->interFace;
    if (op == NULL || op->add == NULL) {
        HDF_LOGE("%s fail: Impl Add not exist.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return op->add(ndImpl);
}

int32_t NetDeviceDelete(struct NetDevice *netDevice)
{
    struct NetDeviceImplOp *op = NULL;
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);

    if (ndImpl == NULL) {
        HDF_LOGE("%s fail: netDevice not exist!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    op = ndImpl->interFace;
    if (op == NULL || op->delete == NULL) {
        HDF_LOGE("%s fail: Impl op Delete exist.", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return op->delete(ndImpl);
}

struct NetDevice *NetDeviceGetInstByName(const char *name)
{
    int32_t i;

    if (name == NULL) {
        HDF_LOGE("%s fail: name = NULL.", __func__);
        return NULL;
    }
    for (i = 0; i < MAX_NETDEVICE_COUNT; i++) {
        if (g_netDeviceImplTable[i] != NULL && g_netDeviceImplTable[i]->netDevice != NULL) {
            if (strcmp(g_netDeviceImplTable[i]->netDevice->name, name) == 0) {
                return g_netDeviceImplTable[i]->netDevice;
            }
        }
    }
    HDF_LOGE("%s fail: %s: name not exist.", __func__, name);
    return NULL;
}

bool NetDeviceIsAnyInstRunning(void)
{
    struct NetDevice *netDev = NULL;
    int32_t i;

    for (i = 0; i < MAX_NETDEVICE_COUNT; i++) {
        if (g_netDeviceImplTable[i] != NULL && g_netDeviceImplTable[i]->netDevice != NULL) {
            netDev = g_netDeviceImplTable[i]->netDevice;
            if ((GET_NET_DEV_FLAGS(netDev) & NET_DEVICE_IFF_RUNNING) != 0) {
                return true;
            }
        }
    }
    return false;
}

bool NetDeviceIsInstRunning(const struct NetDevice *netDevice)
{
    if (netDevice == NULL) {
        return false;
    }
    if ((GET_NET_DEV_FLAGS(netDevice) & NET_DEVICE_IFF_RUNNING) != 0) {
        return true;
    }
    return false;
}

uint32_t NetDevGetRegisterCount(void)
{
    uint32_t count = 0;
    int32_t i;

    for (i = 0; i < MAX_NETDEVICE_COUNT; i++) {
        if (g_netDeviceImplTable[i] != NULL && g_netDeviceImplTable[i]->netDevice != NULL) {
            count++;
        }
    }
    return count;
}

uint32_t NetDeviceGetCap(void)
{
    return MAX_NETDEVICE_COUNT;
}

struct NetDevice *NetDeviceGetInstByIndex(uint32_t index)
{
    if (index < MAX_NETDEVICE_COUNT) {
        if (g_netDeviceImplTable[index] != NULL) {
            return g_netDeviceImplTable[index]->netDevice;
        } else {
            return NULL;
        }
    }
    return NULL;
}

int32_t NetIfSetAddr(const struct NetDevice *netDevice, const IpV4Addr *ipAddr, const IpV4Addr *netMask,
    const IpV4Addr *gw)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->setIpAddr != NULL) {
        return ndImpl->interFace->setIpAddr(ndImpl, ipAddr, netMask, gw);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

static int32_t NetIfRxImpl(const struct NetDevice *netDevice, NetBuf *buff, ReceiveFlag flag)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    ProcessingResult ret = PROCESSING_CONTINUE;

    if (ndImpl == NULL || ndImpl->interFace == NULL || ndImpl->interFace->receive == NULL) {
        HDF_LOGE("%s: NetIfRxImpl fail : netdevice not exist!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    /* to do driver special process */
    if (netDevice->netDeviceIf != NULL && netDevice->netDeviceIf->specialEtherTypeProcess != NULL) {
        ret = netDevice->netDeviceIf->specialEtherTypeProcess(netDevice, buff);
    }
    /* Sent to TCP/IP Stack. */
    if (ret == PROCESSING_CONTINUE) {
        return ndImpl->interFace->receive(ndImpl, buff, flag);
    } else if (ret == PROCESSING_COMPLETE) {
        HDF_LOGI("NetIfRxImpl specialEtherType Process not need TCP/IP stack!");
        return HDF_SUCCESS;
    } else {
        HDF_LOGE("%s: NetIfRxImpl specialEtherType Process error", __func__);
        return HDF_FAILURE;
    }
}

int32_t NetIfSetMacAddr(struct NetDevice *netDevice, const unsigned char *macAddr, unsigned char length)
{
    HDF_STATUS ret;
    struct NetDeviceImpl *ndImpl = NULL;

    if (macAddr == NULL || length != MAC_ADDR_SIZE) {
        HDF_LOGE("%s fail: input param error!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (memcpy_s(netDevice->macAddr, MAC_ADDR_SIZE, macAddr, MAC_ADDR_SIZE) != EOK) {
        HDF_LOGE("%s fail : memcpy_s fail!", __func__);
        return HDF_FAILURE;
    }
    if (netDevice->netDeviceIf != NULL && netDevice->netDeviceIf->setMacAddr != NULL) {
        ret = netDevice->netDeviceIf->setMacAddr(netDevice, (void*)macAddr);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s fail : setMacAddr fail!", __func__);
            return ret;
        }
    }
    ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->changeMacAddr != NULL) {
        return ndImpl->interFace->changeMacAddr(ndImpl);
    }
    return HDF_FAILURE;
}

int32_t NetIfRx(const struct NetDevice *netDevice, NetBuf *buff)
{
    return NetIfRxImpl(netDevice, buff, IN_INTERRUPT);
}

int32_t NetIfRxNi(const struct NetDevice *netDevice, NetBuf *buff)
{
    return NetIfRxImpl(netDevice, buff, NO_IN_INTERRUPT);
}

int32_t NetIfSetStatus(const struct NetDevice *netDevice, NetIfStatus status)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->setStatus != NULL) {
        return ndImpl->interFace->setStatus(ndImpl, status);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfSetLinkStatus(const struct NetDevice *netDevice, NetIfLinkStatus status)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->setLinkStatus != NULL) {
        return ndImpl->interFace->setLinkStatus(ndImpl, status);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfGetLinkStatus(const struct NetDevice *netDevice, NetIfLinkStatus *status)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->getLinkStatus != NULL) {
        return ndImpl->interFace->getLinkStatus(ndImpl, status);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfDhcpsStart(const struct NetDevice *netDevice, char *ip, uint16_t ipNum)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->dhcpsStart != NULL) {
        return ndImpl->interFace->dhcpsStart(ndImpl, ip, ipNum);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfDhcpsStop(const struct NetDevice *netDevice)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->dhcpsStop != NULL) {
        return ndImpl->interFace->dhcpsStop(ndImpl);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfDhcpStart(const struct NetDevice *netDevice)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->dhcpStart != NULL) {
        return ndImpl->interFace->dhcpStart(ndImpl);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfDhcpStop(const struct NetDevice *netDevice)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->dhcpStop != NULL) {
        return ndImpl->interFace->dhcpStop(ndImpl);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

int32_t NetIfDhcpIsBound(const struct NetDevice *netDevice)
{
    struct NetDeviceImpl *ndImpl = GetImplByNetDevice(netDevice);
    if (ndImpl != NULL && ndImpl->interFace != NULL && ndImpl->interFace->dhcpIsBound != NULL) {
        return ndImpl->interFace->dhcpIsBound(ndImpl);
    }
    HDF_LOGE("%s: netDevice not init or already free.", __func__);
    return HDF_ERR_INVALID_PARAM;
}

/*
 * Alloc a net buffer for the net device and reserve headroom depended on net device setting
 *
 * @param  : dev The net device
 *           size The net buffer size
 * @return : A new net buffer on success or NULL on fail
 */
NetBuf *NetBufDevAlloc(const struct NetDevice *dev, uint32_t size)
{
    uint32_t reserve = 0;
    NetBuf *nb = NULL;

    if (dev != NULL) {
        reserve = dev->neededHeadRoom + dev->neededTailRoom;
    }

    size += reserve;

    nb = NetBufAlloc(size);
    if (nb == NULL) {
        return NULL;
    }

    if (dev != NULL) {
        nb->dev = (void *)dev;
        NetBufPop(nb, E_TAIL_BUF, dev->neededHeadRoom);
        NetBufPop(nb, E_DATA_BUF, dev->neededHeadRoom);
    }

    return nb;
}
