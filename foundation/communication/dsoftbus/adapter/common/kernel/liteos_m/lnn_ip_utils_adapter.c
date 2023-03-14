/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lnn_ip_utils_adapter.h"

#include "lwip/netif.h"
#include "softbus_adapter_mem.h"

int32_t GetNetworkIpByIfName(const char *ifName, char *ip, char *netmask, uint32_t len)
{
    if (ifName == NULL || ip == NULL) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "ifName or ip buffer is NULL!");
        return SOFTBUS_INVALID_PARAM;
    }

    struct netif *netif = NULL;
    char *ipStr = NULL;
    char *netMaskStr = NULL;
    ip4_addr_t *ipAddr = NULL;
    ip4_addr_t *netMask = NULL;
    ip4_addr_t *gw = NULL;

    netif = netif_find(ifName);
    if (netif == NULL) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "netif is NULL!");
        return SOFTBUS_ERR;
    }
#ifdef HISPARK_PEGASUS_USE_NETIF_GET_ADDR
    netifapi_netif_get_addr(netif, ipAddr, netMask, gw);
#else
    ipAddr = netif_ip4_addr(netif);
    netMask = netif_ip4_netmask(netif);
    gw = netif_ip4_gw(netif);
#endif
    ipStr = ip4addr_ntoa(ipAddr);
    if (strncpy_s(ip, len, ipStr, strlen(ipStr)) != EOK) {
        HILOG_ERROR(SOFTBUS_HILOG_ID, "copy ip failed!");
        return SOFTBUS_ERR;
    }
    if (netmask != NULL) {
        netMaskStr = ip4addr_ntoa(netMask);
        if (strncpy_s(netmask, len, netMaskStr, strlen(netMaskStr)) != EOK) {
            HILOG_ERROR(SOFTBUS_HILOG_ID, "copy netmask failed!");
            return SOFTBUS_ERR;
        }
    }
    return SOFTBUS_OK;
}
