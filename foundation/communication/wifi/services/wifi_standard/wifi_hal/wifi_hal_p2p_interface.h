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

#ifndef OHOS_WIFI_HAL_P2P_INTERFACE_H
#define OHOS_WIFI_HAL_P2P_INTERFACE_H

#include "wifi_hal_define.h"
#include "wifi_hal_struct.h"
#include "wifi_hal_p2p_struct.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Enable P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pStart(void);

/**
 * @Description  Disabling P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pStop(void);

/**
 * @Description Forcibly Close P2p
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pForceStop(void);

/**
 * @Description Setting Random MAC Address
 *
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetRandomMac(int enable);

/**
 * @Description  Sets the WPS device name.
 *
 * @param name - DeviceName
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetDeviceName(const char *name);

/**
 * @Description Sets the P2P SSID suffix.
 *
 * @param postfixName
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetSsidPostfixName(const char *postfixName);

/**
 * @Description Setting the WPS Primary Device Type
 *
 * @param type - Primary Device Type
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWpsDeviceType(const char *type);

/**
 * @Description Setting the WPS Secondary Device Type
 *
 * @param type - Secondary Device Type
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWpsSecondaryDeviceType(const char *type);

/**
 * @Description Setting WPS config methods
 *
 * @param config - config methods
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWpsConfigMethods(const char *config);

/**
 * @Description Obtains the MAC address of the working device.
 *
 * @param deviceAddress - request address buffer
 * @param size - request address buffer size
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetDeviceAddress(char *deviceAddress, int size);

/**
 * @Description flush P2P table and status
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pFlush();

/**
 * @Description Flush all services on the current device.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pFlushService();

/**
 * @Description Save Config.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pSaveConfig();

/**
 * @Description Enable WPS PBC
 *
 * @param groupIfc
 * @param address
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetupWpsPbc(const char *groupIfc, const char *address);

/**
 * @Description Enable Wps Pin mode
 *
 * @param groupIfc - p2p group
 * @param address
 * @param pin - pin code
 * @param result - when pin is empty, represent use pin display mode, this return pin code
 * @param resultLen - request result buffer size
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetupWpsPin(const char *groupIfc, const char *address, const char *pin, char *result, int resultLen);

/**
 * @Description Remove P2p Network.
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo P2pRemoveNetwork(int networkId);

/**
 * @Description Get P2p Network List.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pListNetworks(P2pNetworkList *infoList);

/**
 * @Description Set the maximum idle time of a group.
 *
 * @param groupIfc
 * @param time
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetGroupMaxIdle(const char *groupIfc, int time);

/**
 * @Description Setting the Power Saving Mode
 *
 * @param groupIfc
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetPowerSave(const char *groupIfc, int enable);

/**
 * @Description Enable or disable Wi-Fi display.
 *
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWfdEnable(int enable);

/**
 * @Description Sets Wi-Fi display device information.
 *
 * @param config
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWfdDeviceConfig(const char *config);

/**
 * @Description Start P2P service discovery.
 *
 * @param timeout
 * @return WifiErrorNo
 */
WifiErrorNo P2pStartFind(int timeout);

/**
 * @Description Disabling P2P Service Discovery
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pStopFind();

/**
 * @Description Setting listen channel
 *
 * @param enable
 * @param period
 * @param interval
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetExtListen(int enable, int period, int interval);

/**
 * @Description Setting listen channel
 *
 * @param channel
 * @param regClass
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetListenChannel(int channel, int regClass);

/**
 * @Description Connect
 *
 * @param info
 * @return WifiErrorNo
 */
WifiErrorNo P2pConnect(P2pConnectInfo *info);

/**
 * @Description Cancel the ongoing P2P group and join the group.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pCancelConnect();

/**
 * @Description Provision Discovery
 *
 * @param peerBssid
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo P2pProvisionDiscovery(const char *peerBssid, int mode);

/**
 * @Description Add a P2P Group.
 *
 * @param isPersistent
 * @param networkId
 * @param freq - 2/5.  0 - acs
 * @return WifiErrorNo
 */
WifiErrorNo P2pAddGroup(int isPersistent, int networkId, int freq);

/**
 * @Description Terminate a group.
 *
 * @param interface
 * @return WifiErrorNo
 */
WifiErrorNo P2pRemoveGroup(const char *interface);

/**
 * @Description Inviting a Device to Join a Persistent Group.
 *
 * @param peerBssid
 * @param goBssid
 * @param ifname
 * @return WifiErrorNo
 */
WifiErrorNo P2pInvite(int persisitent, const char *peerBssid, const char *goBssid, const char *ifname);

/**
 * @Description persistent group Reinvoke peer
 *
 * @param networkId
 * @param bssid
 * @return WifiErrorNo
 */
WifiErrorNo P2pReinvoke(int networkId, const char *bssid);

/**
 * @Description Obtains the capability of the group to which the deviceAddress belongs.
 *
 * @param bssid
 * @param cap
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetGroupCapability(const char *bssid, int *cap);

/**
 * @Description Add an UPNP/Bonjour service.
 *
 * @param info
 * @return WifiErrorNo
 */
WifiErrorNo P2pAddService(const P2pServiceInfo *info);

/**
 * @Description Deleting an UPNP/Bonjour Service
 *
 * @param info
 * @return WifiErrorNo
 */
WifiErrorNo P2pRemoveService(const P2pServiceInfo *info);

/**
 * @Description Initiate a ServiceDiscovery request.
 *
 * @param bssid
 * @param msg
 * @return WifiErrorNo
 */
WifiErrorNo P2pReqServiceDiscovery(const char *bssid, const char *msg, char *disc, int size);

/**
 * @Description Canceling a ServiceDiscovery Request
 *
 * @param id
 * @return WifiErrorNo
 */
WifiErrorNo P2pCancelServiceDiscovery(const char *id);

/**
 * @Description Set the Miracast mode to the driver.
 *
 * @param type
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetMiracastType(int type);

/**
 * @Description P2p response server discovery
 *
 * @param info
 * @return WifiErrorNo
 */
WifiErrorNo P2pRespServerDiscovery(P2pServDiscReqInfo *info);

/**
 * @Description P2p Set Service Discovery External
 *
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetServDiscExternal(int mode);

/**
 * @Description P2p Set Persistent Reconnect mode.
 *
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetPersistentReconnect(int mode);

/**
     * @Description Show information about known P2P peer
     *
     * @param deviceAddress
     * @param device
     * @return WifiErrorNo
     */
WifiErrorNo P2pGetPeer(const char *bssid, P2pDeviceInfo *peerInfo);

/**
 * @Description Obtains the frequencies supported by a specified frequency band.
 *
 * @param band - Band type.
 * @param frequencies - Numeric group pointer of the int type.
 * @param size - Size of the memory to which the frequencies point and the
 *               number of obtained data.
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetFrequencies(int band, int *frequencies, int *size);

/**
 * @Description Setting the P2P group config.
 *
 * @param networkId
 * @param config
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size);

/**
 * @Description Getting the P2P group config.
 *
 * @param networkId
 * @param config
 * @param size
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size);

/**
 * @Description Add Network interface.
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo P2pAddNetwork(int *networkId);

/**
 * @Description Connect
 *
 * @param info - connection information
 * @return WifiErrorNo
 */
WifiErrorNo P2pHid2dConnect(Hid2dConnectInfo *info);

#ifdef __cplusplus
}
#endif

#endif  // OHOS_WIFI_HAL_P2P_INTERFACE_H
