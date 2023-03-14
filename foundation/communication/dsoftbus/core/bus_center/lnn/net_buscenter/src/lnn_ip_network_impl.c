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

#include "lnn_ip_network_impl.h"

#include <securec.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "auth_interface.h"
#include "bus_center_event.h"
#include "bus_center_info_key.h"
#include "bus_center_manager.h"
#include "disc_interface.h"
#include "lnn_discovery_manager.h"
#include "lnn_ip_utils_adapter.h"
#include "lnn_linkwatch.h"
#include "lnn_net_builder.h"
#include "lnn_network_manager.h"
#include "lnn_physical_subnet_manager.h"
#include "softbus_adapter_mem.h"
#include "softbus_adapter_thread.h"
#include "softbus_def.h"
#include "softbus_errcode.h"
#include "softbus_feature_config.h"
#include "softbus_log.h"
#include "trans_tcp_direct_listener.h"

#define IP_DEFAULT_PORT 0

static int32_t GetAvailableIpAddr(const char *ifName, char *ip, uint32_t size)
{
    if (!LnnIsLinkReady(ifName)) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ifName %s link not ready", ifName);
        return SOFTBUS_ERR;
    }

    if (GetNetworkIpByIfName(ifName, ip, NULL, size) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:get network IP by ifName failed!", __func__);
        return SOFTBUS_ERR;
    }

    if (strcmp(ip, LNN_LOOPBACK_IP) == 0) {
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

static int32_t OpenAuthPort(void)
{
    int32_t port = OpenAuthServer();
    if (port < 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "open auth server failed\n");
        return SOFTBUS_ERR;
    }
    return LnnSetLocalNumInfo(NUM_KEY_AUTH_PORT, port);
}

static void CloseAuthPort(void)
{
    CloseAuthServer();
    (void)LnnSetLocalNumInfo(NUM_KEY_AUTH_PORT, IP_DEFAULT_PORT);
}

static int32_t OpenSessionPort(void)
{
    char ipAddr[IP_LEN] = {0};
    int32_t port;

    if (LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr, IP_LEN) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get local ip failed\n");
        return SOFTBUS_ERR;
    }
    port = TransTdcStartSessionListener(ipAddr, 0);
    if (port < 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "open session server failed\n");
        return SOFTBUS_ERR;
    }
    return LnnSetLocalNumInfo(NUM_KEY_SESSION_PORT, port);
}

static void CloseSessionPort(void)
{
    TransTdcStopSessionListener();
    (void)LnnSetLocalNumInfo(NUM_KEY_SESSION_PORT, IP_DEFAULT_PORT);
}

static void OpenProxyPort(void)
{
    LocalListenerInfo listenerInfo = {0};
    char ipAddr[IP_LEN] = {0};
    int32_t port;

    listenerInfo.type = CONNECT_TCP;
    listenerInfo.info.ipListenerInfo.port = 0;
    if (LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr, IP_LEN) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get local ip failed\n");
        return;
    }
    if (strncpy_s(listenerInfo.info.ipListenerInfo.ip, IP_LEN, ipAddr, strlen(ipAddr)) != EOK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "copy ip failed\n");
        return;
    }
    listenerInfo.info.ipListenerInfo.moduleId = PROXY;
    port = ConnStartLocalListening(&listenerInfo);
    if (port < 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "open proxy server failed\n");
        return;
    }
    (void)LnnSetLocalNumInfo(NUM_KEY_PROXY_PORT, port);
}

static void CloseProxyPort(void)
{
    LocalListenerInfo listenerInfo = {0};
    listenerInfo.type = CONNECT_TCP;
    listenerInfo.info.ipListenerInfo.moduleId = PROXY;
    if (ConnStopLocalListening(&listenerInfo) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ConnStopLocalListening fail!\n");
    }
    (void)LnnSetLocalNumInfo(NUM_KEY_PROXY_PORT, IP_DEFAULT_PORT);
}

static int32_t OpenIpLink(void)
{
    int32_t ret = OpenAuthPort();
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "OpenAuthPort fail!\n");
        return SOFTBUS_ERR;
    }
    ret = OpenSessionPort();
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "OpenSessionPort fail!\n");
        CloseAuthPort();
        return SOFTBUS_ERR;
    }
    OpenProxyPort();
    return SOFTBUS_OK;
}

static void CloseIpLink(void)
{
    CloseAuthPort();
    CloseSessionPort();
    CloseProxyPort();
}

static int32_t GetLocalIpInfo(char *ipAddr, uint32_t ipAddrLen, char *ifName, uint32_t ifNameLen)
{
    if (LnnGetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr, ipAddrLen) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get local ip error!\n");
        return SOFTBUS_ERR;
    }
    if (LnnGetLocalStrInfo(STRING_KEY_NET_IF_NAME, ifName, ifNameLen) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get local ifName error!\n");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

static int32_t SetLocalIpInfo(const char *ipAddr, const char *ifName)
{
    if (LnnSetLocalStrInfo(STRING_KEY_WLAN_IP, ipAddr) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "set local ip error!\n");
        return SOFTBUS_ERR;
    }
    if (LnnSetLocalStrInfo(STRING_KEY_NET_IF_NAME, ifName) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "set local ifName error!\n");
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

static void LeaveOldIpNetwork(const char *ifCurrentName)
{
    ConnectionAddrType type = CONNECTION_ADDR_MAX;
    bool addrType[CONNECTION_ADDR_MAX] = {0};

    if (LnnGetAddrTypeByIfName(ifCurrentName, &type) != SOFTBUS_OK) {
        SoftBusLog(
            SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:LnnGetAddrTypeByIfName failed! ifName=%s", __func__, ifCurrentName);
        return;
    }

    if (type == CONNECTION_ADDR_MAX) {
        addrType[CONNECTION_ADDR_WLAN] = true;
        addrType[CONNECTION_ADDR_ETH] = true;
    } else {
        addrType[type] = true;
    }
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "LNN start leave ip network\n");
    if (LnnRequestLeaveByAddrType(addrType, CONNECTION_ADDR_MAX) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "LNN leave ip network fail\n");
    }
}

static int32_t ReleaseMainPort(const char *ifName)
{
    char oldMainIf[NET_IF_NAME_LEN] = {0};
    do {
        if (LnnGetLocalStrInfo(STRING_KEY_NET_IF_NAME, oldMainIf, sizeof(oldMainIf)) != SOFTBUS_OK) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:get local ifName error!\n", __func__);
            break;
        }

        if (strcmp(ifName, oldMainIf) != 0) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "ifName %s is not main port!\n", ifName);
            return SOFTBUS_ERR;
        }
    } while (false);

    if (SetLocalIpInfo(LNN_LOOPBACK_IP, LNN_LOOPBACK_IFNAME) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:set local ip info failed\n", __func__);
        return SOFTBUS_ERR;
    }
    return SOFTBUS_OK;
}

static int32_t RequestMainPort(const char *ifName, const char *address)
{
    if (strcmp(ifName, LNN_LOOPBACK_IFNAME) == 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "loopback ifName not allowed!\n");
        return SOFTBUS_ERR;
    }
    if (strcmp(address, LNN_LOOPBACK_IP) == 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "loopback ip not allowed!\n");
        return SOFTBUS_ERR;
    }

    char oldMainIf[NET_IF_NAME_LEN] = {0};
    if (LnnGetLocalStrInfo(STRING_KEY_NET_IF_NAME, oldMainIf, sizeof(oldMainIf)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get local ifName error!\n");
        return SOFTBUS_ERR;
    }

    if (strcmp(oldMainIf, ifName) != 0 && strcmp(oldMainIf, LNN_LOOPBACK_IFNAME) != 0) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "Only 1 local subnet is allowed!\n");
        return SOFTBUS_ERR;
    }

    if (SetLocalIpInfo(address, ifName) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "set local ip info failed\n");
        return SOFTBUS_ERR;
    }

    return SOFTBUS_OK;
}

static int32_t EnableIpSubnet(LnnPhysicalSubnet *subnet)
{
    char address[IP_LEN] = {0};

    int32_t ret = GetAvailableIpAddr(subnet->ifName, address, sizeof(address));
    if (ret != SOFTBUS_OK) {
        SoftBusLog(
            SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get available Ip failed!ifName=%s, ret=%d", subnet->ifName, ret);
        return ret;
    }

    if (RequestMainPort(subnet->ifName, address)) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "request main port failed!ifName=%s", subnet->ifName);
        return SOFTBUS_ERR;
    }

    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_INFO, "open ip link and start discovery\n");
    if (OpenIpLink() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "open ip link failed\n");
    }
    DiscLinkStatusChanged(LINK_STATUS_UP, COAP);
    if (LnnStartPublish() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "start publish failed\n");
    }
    if (LnnIsAutoNetWorkingEnabled() && LnnStartDiscovery() != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "start discovery failed\n");
    }
    SetCallLnnStatus(true);
    return SOFTBUS_OK;
}

static int32_t DisableIpSubnet(LnnPhysicalSubnet *subnet)
{
    if (subnet->status == LNN_SUBNET_RUNNING) {
        CloseIpLink();
        LnnStopPublish();
        LnnStopDiscovery();
        DiscLinkStatusChanged(LINK_STATUS_DOWN, COAP);
        LeaveOldIpNetwork(subnet->ifName);
        ReleaseMainPort(subnet->ifName);
    }
    return SOFTBUS_OK;
}

static int32_t ChangeIpSubnetAddress(LnnPhysicalSubnet *subnet)
{
    CloseIpLink();
    LnnStopPublish();
    LnnStopDiscovery();
    DiscLinkStatusChanged(LINK_STATUS_DOWN, COAP);
    LeaveOldIpNetwork(subnet->ifName);
    return SOFTBUS_OK;
}

static void DestroyIpSubnetManager(LnnPhysicalSubnet *subnet)
{
    if (subnet->status == LNN_SUBNET_RUNNING) {
        DisableIpSubnet(subnet);
    }
    SoftBusFree(subnet);
}

typedef enum {
    SUBNET_MANAGER_EVENT_IF_READY,
    SUBNET_MANAGER_EVENT_IF_DOWN,    // addr change from avaliable to
    SUBNET_MANAGER_EVENT_IF_CHANGED, // addr changed
    SUBNET_MANAGER_EVENT_MAX
} SubnetManagerEvent;

typedef enum {
    EVENT_RESULT_ACCEPTED = 0,
    EVENT_RESULT_REJECTED,
    EVENT_RESULT_OPTION_COUNT
} SubnetManagerEventResultOptions;

static void TransactIpSubnetState(LnnPhysicalSubnet *subnet, SubnetManagerEvent event, bool isAccepted)
{
    LnnPhysicalSubnetStatus transactMap[][EVENT_RESULT_OPTION_COUNT] = {
        [SUBNET_MANAGER_EVENT_IF_READY] = {LNN_SUBNET_RUNNING, LNN_SUBNET_IDLE},
        [SUBNET_MANAGER_EVENT_IF_DOWN] = {LNN_SUBNET_SHUTDOWN, subnet->status },
        [SUBNET_MANAGER_EVENT_IF_CHANGED] = {LNN_SUBNET_RESETTING, subnet->status }
    };
    subnet->status = transactMap[event][isAccepted ? EVENT_RESULT_ACCEPTED : EVENT_RESULT_REJECTED];
    SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_DBG, "subnet [%s, %d] state change to %d", subnet->ifName,
        subnet->protocolType, subnet->status);
}

static SubnetManagerEvent GetEventInOther(LnnPhysicalSubnet *subnet)
{
    char currentIfAddress[IP_LEN] = {0};
    int32_t ret = GetAvailableIpAddr(subnet->ifName, currentIfAddress, sizeof(currentIfAddress));
    if (ret == SOFTBUS_OK) {
        return SUBNET_MANAGER_EVENT_IF_READY;
    } else {
        return subnet->status != LNN_SUBNET_SHUTDOWN ? SUBNET_MANAGER_EVENT_IF_DOWN : SUBNET_MANAGER_EVENT_MAX;
    }
}

static SubnetManagerEvent GetEventInRunning(LnnPhysicalSubnet *subnet)
{
    char currentIfAddress[IP_LEN] = {0};
    int32_t ret = GetAvailableIpAddr(subnet->ifName, currentIfAddress, sizeof(currentIfAddress));
    if (ret != SOFTBUS_OK) {
        return SUBNET_MANAGER_EVENT_IF_DOWN;
    }

    char localIpAddr[IP_LEN] = {0};
    char localNetifName[NET_IF_NAME_LEN] = {0};
    if (GetLocalIpInfo(localIpAddr, sizeof(localIpAddr), localNetifName, sizeof(localNetifName)) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "get main ip info failed\n");
        return SUBNET_MANAGER_EVENT_IF_READY;
    }

    if (strcmp(localNetifName, subnet->ifName) != 0) {
        return SUBNET_MANAGER_EVENT_IF_READY;
    }

    if (strcmp(localIpAddr, currentIfAddress) == 0) {
        return SUBNET_MANAGER_EVENT_MAX;
    } else {
        return SUBNET_MANAGER_EVENT_IF_CHANGED;
    }
}

static void OnSoftbusNetworkDisconnected(LnnPhysicalSubnet *subnet)
{
    if (subnet->status == LNN_SUBNET_RESETTING || subnet->status == LNN_SUBNET_IDLE) {
        int32_t ret = EnableIpSubnet(subnet);
        TransactIpSubnetState(subnet, SUBNET_MANAGER_EVENT_IF_READY, (ret == SOFTBUS_OK));
    }
}

static void OnNetifStatusChanged(LnnPhysicalSubnet *subnet)
{
    SubnetManagerEvent event = SUBNET_MANAGER_EVENT_MAX;

    if (subnet->status == LNN_SUBNET_RUNNING) {
        event = GetEventInRunning(subnet);
    } else {
        event = GetEventInOther(subnet);
    }

    int32_t ret = SOFTBUS_ERR;
    switch (event) {
        case SUBNET_MANAGER_EVENT_IF_READY: {
            ret = EnableIpSubnet(subnet);
            break;
        }
        case SUBNET_MANAGER_EVENT_IF_DOWN: {
            ret = DisableIpSubnet(subnet);
            break;
        }
        case SUBNET_MANAGER_EVENT_IF_CHANGED: {
            ret = ChangeIpSubnetAddress(subnet);
            break;
        }

        default:
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_WARN, "discard unexpected event %d\n", event);
            return;
    }

    TransactIpSubnetState(subnet, event, (ret == SOFTBUS_OK));
}

static LnnPhysicalSubnet *CreateIpSubnetManager(const char *ifName)
{
    LnnPhysicalSubnet *subnet = (LnnPhysicalSubnet *)SoftBusCalloc(sizeof(LnnPhysicalSubnet));
    if (subnet == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:oom\n", __func__);
        return NULL;
    }

    do {
        subnet->Destroy = DestroyIpSubnetManager;
        subnet->protocolType = LNN_PROTOCOL_IP;
        subnet->status = LNN_SUBNET_IDLE;
        subnet->OnNetifStatusChanged = OnNetifStatusChanged;
        subnet->OnSoftbusNetworkDisconnected = OnSoftbusNetworkDisconnected;

        int32_t ret = strcpy_s(subnet->ifName, sizeof(subnet->ifName), ifName);
        if (ret != EOK) {
            SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:copy ifName failed!ret=%d\n", __func__, ret);
            break;
        }
        return subnet;
    } while (false);

    subnet->Destroy((LnnPhysicalSubnet *)subnet);
    return NULL;
}

static VisitNextChoice NotifyIpAddressChanged(const LnnPhysicalSubnet *subnet, void *data)
{
    (void)data;
    if (subnet->protocolType == LNN_PROTOCOL_IP) {
        LnnNotifyPhysicalSubnetAddressChanged(subnet->ifName, LNN_PROTOCOL_IP);
    }
    return CHOICE_VISIT_NEXT;
}

static void IpAddrChangeEventHandler(const LnnEventBasicInfo *info)
{
    if (info == NULL || info->event != LNN_EVENT_IP_ADDR_CHANGED) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "not interest event");
        return;
    }
    const LnnMonitorAddressChangedEvent *event = (const LnnMonitorAddressChangedEvent *)info;
    if (strlen(event->ifName) != 0) {
        LnnNotifyPhysicalSubnetAddressChanged(event->ifName, LNN_PROTOCOL_IP);
    } else {
        (void)LnnVisitPhysicalSubnet(NotifyIpAddressChanged, NULL);
    }
}

static VisitNextChoice NotifyWlanAddressChanged(const LnnNetIfMgr *netifManager, void *data)
{
    if (netifManager->type == LNN_NETIF_TYPE_WLAN) {
        LnnNotifyPhysicalSubnetAddressChanged(netifManager->ifName, LNN_PROTOCOL_IP);
    }
    return CHOICE_VISIT_NEXT;
}

static void WifiStateChangeEventHandler(const LnnEventBasicInfo *info)
{
    if (info == NULL || info->event != LNN_EVENT_WIFI_STATE_CHANGED) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:not interest event", __func__);
        return;
    }
    (void)LnnVisitNetif(NotifyWlanAddressChanged, NULL);
}

int32_t LnnInitIpProtocol(struct LnnProtocolManager *self)
{
    int32_t ret = SOFTBUS_OK;
    if (LnnRegisterEventHandler(LNN_EVENT_IP_ADDR_CHANGED, IpAddrChangeEventHandler) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "register ip addr change event handler failed\n");
        return SOFTBUS_ERR;
    }
    if (LnnRegisterEventHandler(LNN_EVENT_WIFI_STATE_CHANGED, WifiStateChangeEventHandler) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "register wifi state change event handler failed\n");
        return SOFTBUS_ERR;
    }
    if (SetLocalIpInfo(LNN_LOOPBACK_IP, LNN_LOOPBACK_IFNAME) != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "init local ip as loopback failed!\n");
        return SOFTBUS_ERR;
    }
    DiscLinkStatusChanged(LINK_STATUS_DOWN, COAP);
    return ret;
}

int32_t LnnEnableIpProtocol(struct LnnProtocolManager *self, LnnNetIfMgr *netifMgr)
{
    if (netifMgr == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:null ptr!\n", __func__);
        return SOFTBUS_ERR;
    }
    LnnPhysicalSubnet *manager = CreateIpSubnetManager(netifMgr->ifName);
    if (manager == NULL) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:oom!\n", __func__);
        return SOFTBUS_ERR;
    }

    int ret = LnnRegistPhysicalSubnet(manager);
    if (ret != SOFTBUS_OK) {
        SoftBusLog(SOFTBUS_LOG_LNN, SOFTBUS_LOG_ERROR, "%s:regist subnet manager failed!ret=%d\n", __func__, ret);
        manager->Destroy(manager);
        return ret;
    }
    return SOFTBUS_OK;
}

void LnnDeinitIpNetwork(struct LnnProtocolManager *self)
{
    LnnUnregisterEventHandler(LNN_EVENT_IP_ADDR_CHANGED, IpAddrChangeEventHandler);
    LnnUnregisterEventHandler(LNN_EVENT_WIFI_STATE_CHANGED, WifiStateChangeEventHandler);
    LnnUnregistPhysicalSubnetByType(LNN_PROTOCOL_IP);
    SoftBusLog(SOFTBUS_LOG_COMM, SOFTBUS_LOG_WARN, "%s:ip network deinited", __func__);
}

static LnnProtocolManager g_ipProtocol = {
    .id = 0,
    .pri = 0,
    .supportedNetif = LNN_NETIF_TYPE_ETH | LNN_NETIF_TYPE_WLAN,
    .Init = LnnInitIpProtocol,
    .Deinit = LnnDeinitIpNetwork,
    .Enable = LnnEnableIpProtocol,
    .Disable = NULL,
};

int32_t RegistIPProtocolManager(void)
{
    return LnnRegistProtocol(&g_ipProtocol);
}
