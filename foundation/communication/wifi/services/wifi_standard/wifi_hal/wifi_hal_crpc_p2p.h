/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_HAL_CRPC_P2P_H
#define OHOS_WIFI_HAL_CRPC_P2P_H

#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @Description Enable P2P
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pStart(RpcServer *server, Context *context);

/**
 * @Description Disabling P2P
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pStop(RpcServer *server, Context *context);

/**
 * @Description Setting Random MAC Address
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetRandomMac(RpcServer *server, Context *context);

/**
 * @Description Sets the WPS device name.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetDeviceName(RpcServer *server, Context *context);

/**
 * @Description Sets the P2P SSID suffix.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetSsidPostfixName(RpcServer *server, Context *context);

/**
 * @Description Setting the WPS Primary Device Type
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetWpsDeviceType(RpcServer *server, Context *context);

/**
 * @Description Setting the WPS Secondary Device Type
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetWpsSecondaryDeviceType(RpcServer *server, Context *context);

/**
 * @Description Setting WPS config methods
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetWpsConfigMethods(RpcServer *server, Context *context);

/**
 * @Description Obtains the MAC address of the working device.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pGetDeviceAddress(RpcServer *server, Context *context);

/**
 * @Description flush P2P table and status
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pFlush(RpcServer *server, Context *context);

/**
 * @Description Flush all services on the current device.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pFlushService(RpcServer *server, Context *context);

/**
 * @Description Save config.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSaveConfig(RpcServer *server, Context *context);

/**
 * @Description Enable WPS PBC
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetupWpsPbc(RpcServer *server, Context *context);

/**
 * @Description Enable WPS PIN
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetupWpsPin(RpcServer *server, Context *context);

/**
 * @Description Remove p2p Network.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pRemoveNetwork(RpcServer *server, Context *context);

/**
 * @Description Get network list.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pListNetworks(RpcServer *server, Context *context);

/**
 * @Description Set the maximum idle time of a group.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetGroupMaxIdle(RpcServer *server, Context *context);

/**
 * @Description Setting the Power Saving Mode
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetPowerSave(RpcServer *server, Context *context);

/**
 * @Description Enable or disable Wi-Fi display.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetWfdEnable(RpcServer *server, Context *context);

/**
 * @Description Sets Wi-Fi display device information.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetWfdDeviceConfig(RpcServer *server, Context *context);

/**
 * @Description Start P2P service discovery.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pStartFind(RpcServer *server, Context *context);

/**
 * @Description Disabling P2P Service Discovery
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pStopFind(RpcServer *server, Context *context);

/**
 * @Description Setting the Extended Listening Timing
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetExtListen(RpcServer *server, Context *context);

/**
 * @Description Setting listen channel
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetListenChannel(RpcServer *server, Context *context);

/**
 * @Description P2P Connect.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pConnect(RpcServer *server, Context *context);

/**
 * @Description Cancel the ongoing P2P group and join the group.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pCancelConnect(RpcServer *server, Context *context);

/**
 * @Description Provision Discovery.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pProvisionDiscovery(RpcServer *server, Context *context);

/**
 * @Description Add P2P Group.
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pAddGroup(RpcServer *server, Context *context);

/**
 * @Description Terminate a group.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pRemoveGroup(RpcServer *server, Context *context);

/**
 * @Description Invite
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pInvite(RpcServer *server, Context *context);

/**
 * @Description Persistent group Reinvoke peer
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pReinvoke(RpcServer *server, Context *context);

/**
 * @Description Obtains the capability of the group to which the deviceAddress belongs.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pGetGroupCapability(RpcServer *server, Context *context);

/**
 * @Description Add an UPNP/Bonjour service.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Fail.
 */
int RpcP2pAddService(RpcServer *server, Context *context);

/**
 * @Description Deleting an UPNP/Bonjour Service
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Fail.
 */
int RpcP2pRemoveService(RpcServer *server, Context *context);

/**
 * @Description Initiate a ServiceDiscovery request.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pReqServiceDiscovery(RpcServer *server, Context *context);

/**
 * @Description Canceling a ServiceDiscovery Request
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pCancelServiceDiscovery(RpcServer *server, Context *context);

/**
 * @Description Set the Miracast mode to the driver.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetMiracastType(RpcServer *server, Context *context);

/**
 * @Description Set the P2P mode of persistent reconnect.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetPersistentReconnect(RpcServer *server, Context *context);

/**
 * @Description P2p response server discovery
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pRespServerDiscovery(RpcServer *server, Context *context);

/**
 * @Description P2p Set Service Discovery External
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetServDiscExternal(RpcServer *server, Context *context);

/**
 * @Description Show information about known P2P peer.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pGetPeer(RpcServer *server, Context *context);

/**
 * @Description Obtains the frequencies supported by a specified frequency band.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pGetFrequencies(RpcServer *server, Context *context);

/**
 * @Description Setting the P2P Group Configure.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pSetGroupConfig(RpcServer *server, Context *context);

/**
 * @Description Getting the P2P Group Configure.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pGetGroupConfig(RpcServer *server, Context *context);

/**
 * @Description Add Network interface.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pAddNetwork(RpcServer *server, Context *context);

/**
 * @Description P2P Connect.
 *
 * @param server - Pointer to the global structure of the communication server.
 * @param context - Pointer to the global communication context structure of the server.
 * @return int - 0 Success, -1 Failed.
 */
int RpcP2pHid2dConnect(RpcServer *server, Context *context);

#ifdef __cplusplus
}
#endif
#endif