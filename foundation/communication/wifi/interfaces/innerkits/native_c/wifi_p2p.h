/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_C_P2P_H
#define OHOS_C_P2P_H

#include "wifi_error_code.h"
#include "wifi_p2p_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*P2pStateChangedCallback)(P2pState state);
typedef void (*P2pPersistentGroupsChangedCallback)(void);
typedef void (*P2pConnectionChangedCallback)(const WifiP2pLinkedInfo info);
typedef void (*P2pPeersChangedCallback)(WifiP2pDevice* devices, int len);

/**
 * @Description Enabling the P2P Mode.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode EnableP2p();

/**
 * @Description Disable the P2P mode.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode DisableP2p();

/**
 * @Description Get p2p enable status
 *
 * @param state - enable status
 * @return WifiErrorCode - operate result
 */
WifiErrorCode GetP2pEnableStatus(P2pState* state);

/**
 * @Description Start Wi-Fi P2P device search.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode DiscoverDevices();

/**
 * @Description Stop Wi-Fi P2P device search.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode StopDiscoverDevices();

/**
 * @Description Start the search for the Wi-Fi P2P service.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode DiscoverServices();

/**
 * @Description Stop the search for the Wi-Fi P2P service.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode StopDiscoverServices();

/**
 * @Description Enable Wi-Fi P2P listening.
 *
 * @param period - period
 * @param interval - interval
 * @return WifiErrorCode - operate result
 */
WifiErrorCode StartP2pListen(int period, int interval);

/**
 * @Description Disable Wi-Fi P2P listening.
 *
 * @return ErrCode - operate result
 */
WifiErrorCode StopP2pListen();

/**
 * @Description Creating a P2P Group.
 *
 * @param config - WifiP2pConfig object
 * @return WifiErrorCode - operate result
 */
WifiErrorCode CreateGroup(const WifiP2pConfig* config);

/**
 * @Description Remove a P2P Group.
 *
 * @param config - WifiP2pConfig object
 * @return WifiErrorCode - operate result
 */
WifiErrorCode RemoveGroup();

/**
 * @Description Delete a p2p Group.
 *
 * @param group - WifiP2pGroupInfo object
 * @return ErrCode - operate result
 */
WifiErrorCode DeleteGroup(const WifiP2pGroupInfo* group);

/**
 * @Description P2P connection.
 *
 * @param config - WifiP2pConfig object
 * @return WifiErrorCode - operate result
 */
WifiErrorCode P2pConnect(const WifiP2pConfig* config);

/**
 * @Description Canceling a P2P connection.
 *
 * @return WifiErrorCode - operate result
 */
WifiErrorCode P2pCancelConnect();

/**
 * @Description Get the Current Group object.
 *
 * @param groupInfo - the WifiP2pGroupInfo object
 * @return WifiErrorCode - operate result
 */
WifiErrorCode GetCurrentGroup(WifiP2pGroupInfo* groupInfo);

/**
 * @Description Obtains the P2P connection status.
 *
 * @param status - the P2P connection status
 * @return WifiErrorCode - operate result
 */
WifiErrorCode GetP2pConnectedStatus(int* status);

/**
 * @Description Query the information about the found devices.
 *
 * @param clientDevices - pre-allocate memory for client devices
 * @param size - the allocate size for clientDevices
 * @param retSize - the queryed size of the client devices, used for return.
 * @return WifiErrorCode - operate result
 */
WifiErrorCode QueryP2pDevices(WifiP2pDevice* clientDevices, int size, int* retSize);

/**
 * @Description Query the information about the found groups.
 *
 * @param groupInfo - pre-allocate memory for group size
 * @param size - the allocate size for groupInfo
 * @return ErrCode - operate result
 */
WifiErrorCode QueryP2pGroups(WifiP2pGroupInfo* groupInfo, int size);

/**
 * @Description register p2p state changed event
 *
 * @param callback - callback function
 * @return ErrCode - operate result
 */
WifiErrorCode RegisterP2pStateChangedCallback(const P2pStateChangedCallback callback);

/**
 * @Description register p2p persistent group change event
 *
 * @param callback - callback function
 * @return ErrCode - operate result
 */
WifiErrorCode RegisterP2pPersistentGroupsChangedCallback(const P2pPersistentGroupsChangedCallback callback);

/**
 * @Description register p2p connection change event
 *
 * @param callback - callback function
 * @return ErrCode - operate result
 */
WifiErrorCode RegisterP2pConnectionChangedCallback(const P2pConnectionChangedCallback callback);

/**
 * @Description register p2p peers change event
 *
 * @param callback - callback function
 * @return ErrCode - operate result
 */
WifiErrorCode RegisterP2pPeersChangedCallback(const P2pPeersChangedCallback callback);

#ifdef __cplusplus
}
#endif

#endif
