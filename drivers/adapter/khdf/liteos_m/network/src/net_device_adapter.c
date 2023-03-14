/*
 * Copyright (c) 2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "net_device_adapter.h"

#include <lwip/netif.h>
#include <lwip/pbuf.h>
#include <lwip/opt.h>
#include <lwip/netifapi.h>
#include "net_device.h"
#include "osal_mem.h"
#include "securec.h"

#define HDF_LOG_TAG NetDeviceLite
#define FREE_SPACE_SIZE 2

static int32_t SetNetIfLinkStatus(struct netif *lwipNf, NetIfLinkStatus status)
{
    if (status == NETIF_LINK_DOWN) {
        netif_set_link_down(lwipNf);
    } else if (status == NETIF_LINK_UP) {
        netif_set_link_up(lwipNf);
    } else {
        HDF_LOGE("%s fail: status error!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int32_t LiteNetDevInit(struct NetDeviceImpl *netDeviceImpl)
{
    struct NetDeviceAdapterLite *liteNdPri = NULL;
    if (netDeviceImpl == NULL) {
        HDF_LOGE("%s fail : netdeviceImpl NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    liteNdPri = (struct NetDeviceAdapterLite *)OsalMemCalloc(sizeof(struct NetDeviceAdapterLite));
    if (liteNdPri == NULL) {
        HDF_LOGE("%s fail : malloc fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    (void)memset_s(liteNdPri, sizeof(struct NetDeviceAdapterLite), 0, sizeof(struct NetDeviceAdapterLite));
    netDeviceImpl->osPrivate = (void *)liteNdPri;
    HDF_LOGI("%s Success!", __func__);
    return HDF_SUCCESS;
}

static int32_t LiteNetDevDeInit(struct NetDeviceImpl *netDeviceImpl)
{
    if (netDeviceImpl == NULL) {
        HDF_LOGE("netdevice lite deinit already free.");
        return HDF_SUCCESS;
    }
    if (netDeviceImpl->osPrivate != NULL) {
        OsalMemFree(netDeviceImpl->osPrivate);
        netDeviceImpl->osPrivate = NULL;
    }
    HDF_LOGI("net device lite deinit success!");
    return HDF_SUCCESS;
}
static struct NetBuf *ConverPbuffToNetBuf(const NetDevice *netDev, struct pbuf *lwipBuf)
{
    if (netDev == NULL || lwipBuf == NULL) {
        HDF_LOGE("Convert tot net buff fail : netdev = NULL or lwipBuf = NULL!");
        return NULL;
    }
    return Pbuf2NetBuf(netDev, lwipBuf);
}

static struct pbuf *ConverNetBufToPBuf(const struct NetBuf *netBuff)
{
    if (netBuff == NULL) {
        HDF_LOGE("Conver to pbufff fail : netBuff = NULL!");
        return NULL;
    }
    return NetBuf2Pbuf(netBuff);
}

static int32_t LwipSendCheck(struct netif *netif, struct pbuf *lwipBuf)
{
    if (netif == NULL || lwipBuf == NULL) {
        HDF_LOGE("%s : netif = NUll or lwipBuf = NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct NetDeviceImpl *ndImpl = (struct NetDeviceImpl *)netif->state;
    if (ndImpl == NULL || ndImpl->netDevice == NULL) {
        HDF_LOGE("%s fail : state = NUll or netdevice = NULL!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

void LwipSend(struct netif *netif, struct pbuf *lwipBuf)
{
    if (LwipSendCheck(netif, lwipBuf) != HDF_SUCCESS) {
        return;
    }
    struct NetDeviceImpl *ndImpl = (struct NetDeviceImpl *)netif->state;
    struct NetDevice *netDev = ndImpl->netDevice;
    struct NetDeviceInterFace *netDevIf = NULL;

    struct NetBuf *netBuff = ConverPbuffToNetBuf(netDev, lwipBuf);
    if (netBuff == NULL) {
        return;
    }
    netDevIf = netDev->netDeviceIf;
    if (netDevIf != NULL && netDevIf->xmit != NULL) {
        netDevIf->xmit(netDev, netBuff);
    } else {
        HDF_LOGE("%s fail : netdevIf = null or xmit = null!", __func__);
        NetBufFree(netBuff);
    }
    return;
}

uint8_t LwipSetHwaddr(struct netif *netif, uint8_t *addr, uint8_t len)
{
    if (netif == NULL || addr == NULL) {
        HDF_LOGE("%s fail : netif = null or addr = null!", __func__);
        return (uint8_t)COMMON_ERROR;
    }
    if (len != MAC_ADDR_SIZE) {
        HDF_LOGE("%s fail : len not right", __func__);
        return (uint8_t)COMMON_ERROR;
    }
    struct NetDeviceImpl *ndImpl = (struct NetDeviceImpl *)netif->state;
    struct NetDevice *netDev = NULL;
    struct NetDeviceInterFace *netDevIf = NULL;
    if (ndImpl == NULL || ndImpl->netDevice == NULL) {
        HDF_LOGE("%s fail : state = null or netdevice = null!", __func__);
        return (uint8_t)COMMON_ERROR;
    }
    netDev = ndImpl->netDevice;
    netDevIf = netDev->netDeviceIf;
    if (netDevIf == NULL || netDevIf->setMacAddr == NULL) {
        HDF_LOGE("%s fail : netdevice if = null or setMacAddr = null!", __func__);
        return (uint8_t)COMMON_ERROR;
    }
    return netDevIf->setMacAddr(netDev, addr);
}

void LwipDrvConfig(struct netif *netif, uint32_t conFlags, uint8_t bit)
{
    (void)netif;
    (void)conFlags;
    (void)bit;
    return;
}

static struct netif *CreateLwipNetIf(const struct NetDeviceImpl *netDeviceImpl, const struct NetDevice *netDev)
{
    if (netDev == NULL) {
        HDF_LOGE("%s : netDev = null!", __func__);
        return NULL;
    }
    struct netif *lwipNf = NULL;
    lwipNf = (struct netif *)OsalMemCalloc(sizeof(struct netif));
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : netif malloc fail!", __func__);
        return NULL;
    }
    (void)memset_s(lwipNf, sizeof(struct netif), 0, sizeof(struct netif));

    /* register netif to lwip */
    lwipNf->state = (void *)netDeviceImpl;
    lwipNf->drv_send = LwipSend;
    lwipNf->drv_set_hwaddr = LwipSetHwaddr;
    lwipNf->link_layer_type = netDev->LinkLayerType;
    lwipNf->hwaddr_len = MAC_ADDR_SIZE;
    lwipNf->drv_config = LwipDrvConfig;
#if LOSCFG_NET_LWIP_SACK_2_0
    if (strncpy_s(lwipNf->name, IFNAMSIZ, netDev->name, IFNAMSIZ - FREE_SPACE_SIZE) != EOK) {
        HDF_LOGE("lite netif add fail : strncpy_s fail!");
        OsalMemFree(lwipNf);
        return NULL;
    }
    lwipNf->name[IFNAMSIZ - FREE_SPACE_SIZE] = '\0';
#endif
    return lwipNf;
}

static void DestroyLwipNetIf(struct netif *lwipNf)
{
    if (lwipNf != NULL) {
        OsalMemFree(lwipNf);
    }
}

static void IpV6SpecialProc(struct NetDevice *lwipNd, struct netif *lwipNf)
{
    Protocol80211IfType ifType = lwipNd->funType.wlanType;

    if (lwipNd->LinkLayerType != WIFI_LINK) {
        return;
    }

    if ((ifType == PROTOCOL_80211_IFTYPE_AP) || (ifType == PROTOCOL_80211_IFTYPE_MESH_POINT)) {
#ifdef _PRE_WLAN_FEATURE_LWIP_IPV6_AUTOCONFIG
        (hi_void)netifapi_set_ip6_autoconfig_disabled(lwipNf);
        (hi_void)netifapi_set_accept_ra(lwipNf, false);
        (hi_void)netifapi_set_ipv6_forwarding(lwipNf, true);
        (hi_void)netifapi_set_ra_enable(lwipNf, true);
#endif
    }
    return;
}

static void LiteNetifLinkChangeCallback(struct netif *netif)
{
    if (netif == NULL || netif->state == NULL) {
        return;
    }
    struct NetDeviceImpl *netDeviceImpl = (struct NetDeviceImpl *)netif->state;
    if (netDeviceImpl->netDevice != NULL && netDeviceImpl->netDevice->netDeviceIf != NULL &&
        netDeviceImpl->netDevice->netDeviceIf->linkStatusChanged != NULL) {
            netDeviceImpl->netDevice->netDeviceIf->linkStatusChanged(netDeviceImpl->netDevice);
    }
}

static int32_t LiteNetDevAdd(struct NetDeviceImpl *netDeviceImpl)
{
    if (netDeviceImpl == NULL || netDeviceImpl->osPrivate == NULL || netDeviceImpl->netDevice == NULL) {
        HDF_LOGE("Lite netif add fail : impl = null , osPrivate = null, or netdevice = null!");
        return HDF_ERR_INVALID_PARAM;
    }

    struct NetDeviceAdapterLite *liteNdPri = (struct NetDeviceAdapterLite *)netDeviceImpl->osPrivate;
    struct NetDevice *lwipNd = netDeviceImpl->netDevice;
    struct netif *lwipNf = NULL;
    ip4_addr_t gw, ipaddr, netmask;
    IP4_ADDR(&gw, 0, 0, 0, 0);
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);
    IP4_ADDR(&netmask, 0, 0, 0, 0);
    int ret = 0;
    lwipNf = CreateLwipNetIf(netDeviceImpl, lwipNd);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : CreateLwipNetIf fail!", __func__);
        return HDF_FAILURE;
    }
    if ((ret = netifapi_netif_add(lwipNf, &ipaddr, &netmask, &gw, (void *)liteNdPri,
        driverif_input, tcpip_input)) != ERR_OK) {
        HDF_LOGE("%s : netifapi_netif_add fail!,ret=%d", __func__, ret);
        DestroyLwipNetIf(lwipNf);
        return HDF_FAILURE;
    }
    HDF_LOGI("netifapi_netif_add success!");
#if LOSCFG_NET_LWIP_SACK_2_0
    /* update netdevice name -  wipNf->name + lwipNf->num */
    int32_t num = snprintf_s(lwipNd->name, IFNAMSIZ, (IFNAMSIZ - 1), "%s%" U16_F, lwipNf->name, lwipNf->num);
    if (num < 0) {
        HDF_LOGI("Lite netif add fail : snprintf_s fail!");
        netifapi_netif_remove(lwipNf);
        DestroyLwipNetIf(lwipNf);
        return HDF_FAILURE;
    }
#endif
    /* copy MAC ADDR TO LWIP */
    if (memcpy_s(lwipNf->hwaddr, NETIF_MAX_HWADDR_LEN, lwipNd->macAddr, MAC_ADDR_SIZE) != EOK) {
        HDF_LOGI("%s fail : memcpy_s fail!", __func__);
        netifapi_netif_remove(lwipNf);
        DestroyLwipNetIf(lwipNf);
        return HDF_FAILURE;
    }

    liteNdPri->lwipNetif = lwipNf;
    IpV6SpecialProc(lwipNd, lwipNf);
    /* set netif default status */
    netifapi_netif_set_default(lwipNf);
    netif_set_link_callback(lwipNf, LiteNetifLinkChangeCallback);
    HDF_LOGI("%s success!!", __func__);
    return HDF_SUCCESS;
}

static struct netif *GetNetIfFromDevImpl(const struct NetDeviceImpl *netDeviceImpl)
{
    if (netDeviceImpl == NULL || netDeviceImpl->osPrivate == NULL) {
        HDF_LOGE("%s fail : impl = null or osPrivate = null!", __func__);
        return NULL;
    }
    struct NetDeviceAdapterLite *liteNdPri = (struct NetDeviceAdapterLite *)netDeviceImpl->osPrivate;
    return liteNdPri->lwipNetif;
}

static int32_t LiteNetDevDelete(struct NetDeviceImpl *netDeviceImpl)
{
    if (netDeviceImpl == NULL || netDeviceImpl->osPrivate == NULL) {
        HDF_LOGE("%s fail : impl = null or osPrivate = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct NetDeviceAdapterLite *liteNdPri = (struct NetDeviceAdapterLite *)netDeviceImpl->osPrivate;
    struct netif *lwipNf = liteNdPri->lwipNetif;
    if (lwipNf != NULL) {
        netifapi_netif_remove(lwipNf);
        OsalMemFree(lwipNf);
        liteNdPri->lwipNetif = NULL;
    }
    HDF_LOGI("%s success!", __func__);
    return HDF_SUCCESS;
}

static int32_t LiteNetDevSetStatus(struct NetDeviceImpl *netDeviceImpl, NetIfStatus status)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : netif = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = 0;
    if (status == NETIF_DOWN) {
        ret = netifapi_netif_set_down(lwipNf);
    } else if (status == NETIF_UP) {
        ret = netifapi_netif_set_up(lwipNf);
    } else {
        HDF_LOGE("%s fail : status error!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (ret == ERR_OK) {
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s fail ret = %d!", __func__, ret);
    return HDF_FAILURE;
}

static int32_t LiteNetDevSetLinkStatus(struct NetDeviceImpl *netDeviceImpl, NetIfLinkStatus status)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : netif = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return SetNetIfLinkStatus(lwipNf, status);
}

int32_t LiteNetDevGetLinkStatus(struct NetDeviceImpl *netDeviceImpl, NetIfLinkStatus *status)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : netif = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (status == NULL) {
        HDF_LOGE("%s fail : status is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (netif_is_link_up(lwipNf)) {
        *status = NETIF_LINK_UP;
    } else {
        *status = NETIF_LINK_DOWN;
    }
    return HDF_SUCCESS;
}

static ProcessingResult LiteNetDevDataFilter(struct NetDeviceImpl *netDeviceImpl, struct NetBuf *buff)
{
    struct NetDevice *lwipNd = NULL;

    if (netDeviceImpl == NULL || buff == NULL) {
        HDF_LOGE("%s fail : buff = null!", __func__);
        return PROCESSING_ERROR;
    }
    lwipNd = netDeviceImpl->netDevice;

    struct EtherHeader *header = (struct EtherHeader *)NetBufGetAddress(buff, E_DATA_BUF);
    uint16_t etherType = ntohs(header->etherType);
    if ((lwipNd->LinkLayerType == ETHERNET_LINK) || (etherType == ETHER_TYPE_IP) || (etherType == ETHER_TYPE_ARP) ||
        (etherType == ETHER_TYPE_RARP) || (etherType == ETHER_TYPE_IPV6) || (etherType == ETHER_TYPE_6LO) ||
        (etherType == ETHER_TYPE_PAE)) {
        return PROCESSING_CONTINUE;
    } else {
        NetBufFree(buff);
        return PROCESSING_COMPLETE;
    }
}

static int32_t LiteNetDevDataReceive(struct NetDeviceImpl *netDeviceImpl, struct NetBuf *buff)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        NetBufFree(buff);
        HDF_LOGE("%s fail : lwipnf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    struct pbuf *pBuff = ConverNetBufToPBuf(buff);
    if (pBuff == NULL) {
        NetBufFree(buff);
        HDF_LOGE("%s fail : pBuff = null!", __func__);
        return HDF_FAILURE;
    }
    driverif_input(lwipNf, pBuff);
    NetBufFree(buff);
    return HDF_SUCCESS;
}

static int32_t LiteNetDevReceive(struct NetDeviceImpl *netDeviceImpl, struct NetBuf *buff, ReceiveFlag flag)
{
    if (flag >= MAX_RECEIVE_FLAG) {
        HDF_LOGE("%s fail : flag = %d.", __func__, flag);
        return HDF_ERR_INVALID_PARAM;
    }
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL || buff == NULL) {
        HDF_LOGE("%s fail : lwipnf = null or buff = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ProcessingResult ret = LiteNetDevDataFilter(netDeviceImpl, buff);
    if (ret == PROCESSING_CONTINUE) {
        return LiteNetDevDataReceive(netDeviceImpl, buff);
    } else if (ret == PROCESSING_COMPLETE) {
        return HDF_SUCCESS;
    } else {
        return HDF_FAILURE;
    }
}

int32_t LiteNetSetIpAddr(struct NetDeviceImpl *netDeviceImpl, const IpV4Addr *ipAddr, const IpV4Addr *netMask,
    const IpV4Addr *gw)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    netifapi_netif_set_addr(lwipNf, (ip4_addr_t *)ipAddr, (ip4_addr_t *)netMask, (ip4_addr_t *)gw);
    return HDF_SUCCESS;
}

int32_t LiteNetDhcpsStart(struct NetDeviceImpl *netDeviceImpl, char *ip, uint16_t ipNum)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = 0;
    if ((ret = netifapi_dhcps_start(lwipNf, ip, ipNum)) == ERR_OK) {
        HDF_LOGI("%s success!", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s fail, ret = %d!", __func__, ret);
    return HDF_FAILURE;
}

int32_t LiteNetDhcpsStop(struct NetDeviceImpl *netDeviceImpl)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = 0;
    if (ERR_OK == (ret = netifapi_dhcps_stop(lwipNf))) {
        HDF_LOGI("%s success!", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s fail, ret = %d!", __func__, ret);
    return HDF_FAILURE;
}

int32_t LiteNetDhcpStart(struct NetDeviceImpl *netDeviceImpl)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = 0;
    if ((ret = netifapi_dhcp_start(lwipNf)) == ERR_OK) {
        HDF_LOGI("%s success!", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s fail, ret = %d!", __func__, ret);
    return HDF_FAILURE;
}

int32_t LiteNetDhcpStop(struct NetDeviceImpl *netDeviceImpl)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = 0;
    if ((ret = netifapi_dhcp_stop(lwipNf)) == ERR_OK) {
        dhcp_cleanup(lwipNf);
        HDF_LOGI("%s success!", __func__);
        return HDF_SUCCESS;
    }
    HDF_LOGE("%s fail, ret = %d!", __func__, ret);
    return HDF_FAILURE;
}

int32_t LiteNetDhcpIsBound(struct NetDeviceImpl *netDeviceImpl)
{
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (dhcp_is_bound(lwipNf) == ERR_OK) {
        HDF_LOGI("%s success!", __func__);
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t LiteNetChangeMacAddr(struct NetDeviceImpl *netDeviceImpl)
{
    struct NetDevice *lwipNd = NULL;
    struct netif *lwipNf = GetNetIfFromDevImpl(netDeviceImpl);
    if (lwipNf == NULL) {
        HDF_LOGE("%s fail : lwipNf = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    lwipNd = netDeviceImpl->netDevice;
    if (memcpy_s(lwipNf->hwaddr, NETIF_MAX_HWADDR_LEN, lwipNd->macAddr, MAC_ADDR_SIZE) != EOK) {
        HDF_LOGE("%s fail : memcpy_s fail!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static struct NetDeviceImplOp g_liteNdImplOps = {
    .init = LiteNetDevInit,
    .deInit = LiteNetDevDeInit,
    .add = LiteNetDevAdd,
    .delete = LiteNetDevDelete,
    .setStatus = LiteNetDevSetStatus,
    .setLinkStatus = LiteNetDevSetLinkStatus,
    .getLinkStatus = LiteNetDevGetLinkStatus,
    .receive = LiteNetDevReceive,
    .setIpAddr = LiteNetSetIpAddr,
    .dhcpsStart = LiteNetDhcpsStart,
    .dhcpsStop = LiteNetDhcpsStop,
    .dhcpStart = LiteNetDhcpStart,
    .dhcpStop = LiteNetDhcpStop,
    .dhcpIsBound = LiteNetDhcpIsBound,
    .changeMacAddr = LiteNetChangeMacAddr,
};

int32_t RegisterNetDeviceImpl(struct NetDeviceImpl *ndImpl)
{
    if (ndImpl == NULL) {
        HDF_LOGE("%s fail : impl = null!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    ndImpl->interFace = &g_liteNdImplOps;
    HDF_LOGI("register lite netdevice impl success.");
    return HDF_SUCCESS;
}
int32_t UnRegisterNetDeviceImpl(struct NetDeviceImpl *ndImpl)
{
    if (ndImpl == NULL) {
        HDF_LOGI("%s already unregister.", __func__);
        return HDF_SUCCESS;
    }

    ndImpl->interFace = NULL;
    HDF_LOGI("%s success.", __func__);
    return HDF_SUCCESS;
}
