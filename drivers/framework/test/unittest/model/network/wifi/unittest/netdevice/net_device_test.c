/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "net_device_test.h"
#include "hdf_log.h"
#include "net_device.h"
#include <securec.h>

static struct NetDevice *g_netDevice = NULL;

static uint8_t g_filterData[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x88, 0x11, 0x31, 0xc6, 0xda, 0xdc, 0xff, 0xff
};

static bool WiFiNetDeviceTestEnv(void)
{
    if (g_netDevice == NULL) {
        char devName[IFNAMSIZ] = {0};
        if (strncpy_s(devName, IFNAMSIZ, "wlan_test_0", strlen("wlan_test_0") + 1) != EOK) {
            HDF_LOGE("%s: strcpy_s fail", __func__);
            return false;
        }
        g_netDevice = NetDeviceInit(devName, strlen(devName), WIFI_LINK, LITE_OS);
        if (g_netDevice == NULL) {
            HDF_LOGE("%s fail ", __func__);
            return false;
        }
        g_netDevice->funType.wlanType = PROTOCOL_80211_IFTYPE_STATION;
        HDF_LOGE("%s success ", __func__);
        return true;
    }
    return true;
}

int32_t WiFiNetDviceTestInit(void)
{
    if (g_netDevice != NULL) {
        return HDF_SUCCESS;
    }
    if (!WiFiNetDeviceTestEnv()) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WiFiNetDviceTestDeInit(void)
{
    NetDeviceDeInit(g_netDevice);
    g_netDevice = NULL;
    return HDF_SUCCESS;
}

int32_t WiFiNetDviceTestAdd(void)
{
    if (g_netDevice == NULL) {
        if (!WiFiNetDeviceTestEnv()) {
            HDF_LOGE("%s WiFiNetDeviceTestEnv fail!", __func__);
            return HDF_FAILURE;
        }
    }
    if (NetDeviceAdd(g_netDevice) != HDF_SUCCESS) {
        HDF_LOGE("%s add fail!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WiFiNetDviceTestDelete(void)
{
    if (NetDeviceDelete(g_netDevice) != HDF_SUCCESS) {
        HDF_LOGE("%s NetDeviceDelete fail!", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WiFiNetDviceTestGetDevice(void)
{
    struct NetDevice *temp = NetDeviceGetInstByName(g_netDevice->name);
    if (temp == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WiFiNetDviceTestGetCount(void)
{
    uint32_t count = NetDevGetRegisterCount();
    if (count == 0) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WiFiNetDviceTestGetCap(void)
{
    uint32_t count = NetDeviceGetCap();
    if (count > 0) {
        return HDF_SUCCESS;
    }
    return HDF_FAILURE;
}

int32_t WiFiNetDviceTestSetAddr(void)
{
#ifdef __LITEOS__
    IpV4Addr ip = { 0x010ca8c0UL };      // 192, 168, 12, 1
    IpV4Addr netmask = { 0x00ffffffUL }; // 255, 255, 255, 0
    IpV4Addr gw = { 0x010ca8c0UL };      // 192, 168, 12, 1
    return NetIfSetAddr(g_netDevice, &ip, &netmask, &gw);
#else
    return HDF_SUCCESS;
#endif
}

int32_t WiFiNetDviceTestRx(void)
{
    NetBuf *buff = NULL;

    int count = sizeof(g_filterData);
    buff = NetBufAlloc(count);
    if (buff == NULL) {
        HDF_LOGE("%s fail : NetBufAlloc = null!", __func__);
        return HDF_FAILURE;
    }
    NetBufPush(buff, E_DATA_BUF, count);
    if (memcpy_s(NetBufGetAddress(buff, E_DATA_BUF), count, g_filterData, count) != EOK) {
        NetBufFree(buff);
        buff = NULL;
        HDF_LOGE("%s fail : memcpy_s fail", __func__);
        return HDF_FAILURE;
    }
    return NetIfRx(g_netDevice, buff);
}

int32_t WiFiNetDviceTestSetStatus(void)
{
    return NetIfSetStatus(g_netDevice, NETIF_DOWN);
}

int32_t WiFiNetDviceTestSetLinkStatus(void)
{
#ifdef __LITEOS__
    return NetIfSetLinkStatus(g_netDevice, NETIF_LINK_DOWN);
#else
    return HDF_SUCCESS;
#endif
}

int32_t WifiNetDeviceDhcpClient(void)
{
#ifdef __LITEOS__
    int32_t expectedRet = HDF_SUCCESS;
#else
    int32_t expectedRet = HDF_ERR_INVALID_PARAM;
#endif

    NetIfSetStatus(g_netDevice, NETIF_UP);
    NetIfDhcpStop(g_netDevice);
    if (NetIfDhcpStart(g_netDevice) != expectedRet) {
        return HDF_FAILURE;
    }
    if (NetIfDhcpStop(g_netDevice) != expectedRet) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t WifiNetDeviceDhcpServer(void)
{
#ifdef __LITEOS__
    int32_t expectedRet = HDF_SUCCESS;
#else
    int32_t expectedRet = HDF_ERR_INVALID_PARAM;
#endif

    NetIfSetStatus(g_netDevice, NETIF_UP);
    NetIfDhcpsStop(g_netDevice);
    if (NetIfDhcpsStart(g_netDevice, NULL, 0) != expectedRet) {
        return HDF_FAILURE;
    }
    if (NetIfDhcpsStop(g_netDevice) != expectedRet) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}