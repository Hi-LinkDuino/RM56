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

#ifndef OHOS_IDL_IWIFI_P2P_IFACE_H
#define OHOS_IDL_IWIFI_P2P_IFACE_H

#include "wifi_error_no.h"
#include "i_wifi_event_p2p_callback.h"
#include "i_wifi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @Description Set the Wifi P 2p Event Callback object
 *
 * @param callback
 */
void SetWifiP2pEventCallback(IWifiEventP2pCallback callback);

/**
 * @Description Get the Wifi P 2p Event Callback object
 *
 * @return IWifiEventP2pCallback*
 */
IWifiEventP2pCallback *GetWifiP2pEventCallback(void);

/**
 * @Description Open P2p
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pStart(void);

/**
 * @Description Close p2p
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pStop(void);

/**
 * @Description Request P2P to set a random MAC address
 *
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetRandomMac(int enable);

/**
 * @Description Requesting P2P Setting Device Name
 *
 * @param name
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetDeviceName(const char *name);

/**
 * @Description Send a P2P request for setting the SSID suffix
 *
 * @param postfixName
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetSsidPostfixName(const char *postfixName);

/**
 * @Description Send a request for setting the WPS primary device type in P2P mode
 *
 * @param type
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWpsDeviceType(const char *type);

/**
 * @Description Send a request for setting the WPS secondary device type in P2P mode
 *
 * @param type
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWpsSecondaryDeviceType(const char *type);

/**
 * @Description Send a request for setting the WPS configuration method to the P2P.
 *
 * @param config
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWpsConfigMethods(const char *config);

/**
 * @Description Send a request for get device address to the P2P.
 *
 * @param deviceAddress - device address
 * @param size - request buffer size
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetDeviceAddress(char *deviceAddress, int size);

/**
 * @Description Send a request for flush to the P2P.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pFlush();

/**
 * @Description Send a request for flush service to the P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pFlushService();

/**
 * @Description Send a request for save config to the P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pSaveConfig();

/**
 * @Description  P2P hal-layer registration event
 *
 * @param callback
 * @return WifiErrorNo
 */
WifiErrorNo RegisterP2pEventCallback(IWifiEventP2pCallback callback);

/**
 * @Description Send a request for setup wps pbc to the P2P
 *
 * @param groupIfc - p2p group
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
 * @Description Send a request for remove a p2p network to the P2P
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo P2pRemoveNetwork(int networkId);

/**
 * @Description Send a request for get p2p network list to the P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pListNetworks(P2pNetworkList *infoList);

/**
 * @Description Send a request for set group max idle to the P2P
 *
 * @param groupIfc
 * @param time
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetGroupMaxIdle(const char *groupIfc, int time);

/**
 * @Description Send a request for set power save to the P2P
 *
 * @param groupIfc
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetPowerSave(const char *groupIfc, int enable);

/**
 * @Description Send a request for set wfd enable to the P2P
 *
 * @param enable
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWfdEnable(int enable);

/**
 * @Description Send a request for set wfd device config to the P2P
 *
 * @param config
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetWfdDeviceConfig(const char *config);

/**
 * @Description Send a request for start p2p find to the P2P
 *
 * @param timeout
 * @return WifiErrorNo
 */
WifiErrorNo P2pStartFind(int timeout);

/**
 * @Description Send a request for stop p2p find to the P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pStopFind();

/**
 * @Description Send a request for set ext listen to the P2P
 *
 * @param enable
 * @param period
 * @param interval
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetExtListen(int enable, int period, int interval);

/**
 * @Description Send a request for set listen channel to the P2P
 *
 * @param channel
 * @param regClass
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetListenChannel(int channel, int regClass);

/**
 * @Description Send a request for connect to the P2P
 *
 */
WifiErrorNo P2pConnect(P2pConnectInfo *info);

/**
 * @Description Send a request for cancel connect to the P2P
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pCancelConnect();

/**
 * @Description Send a request for Provision Discovery to the P2P
 *
 * @param peerBssid
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo P2pProvisionDiscovery(const char *peerBssid, int mode);

/**
 * @Description Send a request for add a P2P group to the P2P
 *
 * @param isPersistent
 * @param networkId
 * @param freq
 * @return WifiErrorNo
 */
WifiErrorNo P2pAddGroup(int isPersistent, int networkId, int freq);

/**
 * @Description Send a request for remove group to the P2P
 *
 * @param interface
 * @return WifiErrorNo
 */
WifiErrorNo P2pRemoveGroup(const char *interface);

/**
 * @Description Send a request for invite to the P2P
 *
 * @param peerBssid
 * @param goBssid
 * @param ifname
 * @return WifiErrorNo
 */
WifiErrorNo P2pInvite(int persisitent, const char *peerBssid, const char *goBssid, const char *ifname);

/**
 * @Description Send a request for reinvoke to the P2P
 *
 * @param networkId
 * @param bssid
 * @return WifiErrorNo
 */
WifiErrorNo P2pReinvoke(int networkId, const char *bssid);

/**
 * @Description Send a request for get group capability to the P2P
 *
 * @param bssid
 * @param cap
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetGroupCapability(const char *bssid, int *cap);

/**
 * @Description Send a request for adding a service to the P2P
 *
 * @param info
 * @return WifiErrorNo
 */
WifiErrorNo P2pAddService(const P2pServiceInfo *info);

/**
 * @Description Send a request for removing a service to the P2P
 *
 * @param info
 * @return WifiErrorNo
 */
WifiErrorNo P2pRemoveService(const P2pServiceInfo *info);

/**
 * @Description Send a request for request service discovery to the P2P
 *
 * @param bssid
 * @param msg
 * @return WifiErrorNo
 */
WifiErrorNo P2pReqServiceDiscovery(const char *bssid, const char *msg, char *retMsg, int size);

/**
 * @Description Send a request for cancel request service discovery to the P2P
 *
 * @param id
 * @return WifiErrorNo
 */
WifiErrorNo P2pCancelServiceDiscovery(const char *id);

/**
 * @Description Send a request for set the miracast type to the P2P
 *
 * @param type
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetMiracastType(int type);

/**
 * @Description Set the Persistent Reconnect mode.
 *
 * @param mode
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetPersistentReconnect(int mode);

/**
 * @Description
 *
 * @param deviceAddress
 * @param frequency
 * @param dialogToken
 * @param tlvs
 * @return WifiErrorNo
 */
WifiErrorNo P2pRespServerDiscovery(const char *deviceAddress, int frequency, int dialogToken, const char *tlvs);

/**
 * @Description Set P2p server discovery external.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetServDiscExternal(int mode);

/**
 * @Description Show information about known P2P peer.
 *
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetPeer(const char *deviceAddress, P2pDeviceInfo *peerInfo);

/**
 * @Description Obtains the frequencies supported by a specified frequency band.
 *
 * @param band - Band type.
 * @param frequencies - Numeric group pointer of the int type.
 * @param size - Size of the memory to which the frequencies point and the number of obtained data.
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetFrequencies(int32_t band, int *frequencies, int32_t *size);

/**
 * @Description Setting the P2P group config.
 *
 * @param networkId
 * @param config
 * @return WifiErrorNo
 */
WifiErrorNo P2pSetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size);

/**
 * @Description Getting the P2P group config.
 *
 * @param networkId
 * @param config
 * @return WifiErrorNo
 */
WifiErrorNo P2pGetGroupConfig(int networkId, P2pGroupConfig *pConfig, int size);

/**
 * @Description Request to obtain the next network ID.
 *
 * @param networkId
 * @return WifiErrorNo
 */
WifiErrorNo P2pAddNetwork(int *networkId);

/**
 * @Description Send a request for hid2d connect
 *
 * @param info - configuration for the connection
 * @return WifiErrorNo
 */
WifiErrorNo Hid2dConnect(Hid2dConnectInfo *info);

#ifdef __cplusplus
}
#endif
#endif
