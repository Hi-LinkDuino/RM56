/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef OHOS_BT_GAP_H
#define OHOS_BT_GAP_H

#include "ohos_bt_def.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OHOS_BD_NAME_LEN 32
typedef struct {
    BdAddr addr;
    unsigned char deviceName[OHOS_BD_NAME_LEN]; /* Name of the device record, must be in UTF-8 */
} PairedDeviceInfo;

/* Bluetooth stack state */
typedef enum {
    OHOS_GAP_STATE_TURNING_ON = 0x0,
    OHOS_GAP_STATE_TURN_ON,
    OHOS_GAP_STATE_TURNING_OFF,
    OHOS_GAP_STATE_TURN_OFF
} BtStackState;

/* Bluetooth ACL connection state */
typedef enum {
    OHOS_GAP_ACL_STATE_CONNECTED,
    OHOS_GAP_ACL_STATE_DISCONNECTED,
    OHOS_GAP_ACL_STATE_LE_CONNECTED,
    OHOS_GAP_ACL_STATE_LE_DISCONNECTED
} GapAclState;

/* bondable mode */
typedef enum {
    OHOS_GAP_BONDABLE_MODE_OFF = 0x00,
    OHOS_GAP_BONDABLE_MODE_ON = 0x01
} GapBtBondableMode;

typedef enum {
    OHOS_GAP_ACCESS_UNKNOWN,
    OHOS_GAP_ACCESS_ALLOWED,
    OHOS_GAP_ACCESS_FORBIDDEN,
} GapBtPermissionType;

typedef enum {
    OHOS_GAP_SCAN_MODE_NONE = 0x00,
    OHOS_GAP_SCAN_MODE_CONNECTABLE,
    OHOS_GAP_SCAN_MODE_GENERAL_DISCOVERABLE,
    OHOS_GAP_SCAN_MODE_LIMITED_DISCOVERABLE,
    OHOS_GAP_SCAN_MODE_CONNECTABLE_GENERAL_DISCOVERABLE,
    OHOS_GAP_SCAN_MODE_CONNECTABLE_LIMITED_DISCOVERABLE
} GapBtScanMode;

typedef enum {
    OHOS_PROFILE_HEADSET = 0x0,
    OHOS_PROFILE_A2DP,
    OHOS_PROFILE_OPP,
    OHOS_PROFILE_HID,
    OHOS_PROFILE_PANU,
    OHOS_PROFILE_NAP,
    OHOS_PROFILE_HFP
} GapBtProfileType;

typedef enum {
    OHOS_GAP_PAIR_NONE = 0x01,
    OHOS_GAP_PAIR_PAIRING = 0x02,
    OHOS_GAP_PAIR_PAIRED = 0x03
} GapBtPairedState;

/**
 * @brief Enable ble.
 *
 * @return Returns <b>true</b> if the operation is accepted;
 *         returns <b>false</b> if the operation is rejected.
 */
bool EnableBle(void);

/**
 * @brief Disable ble.
 *
 * @return Returns <b>true</b> if the operation is accepted;
 *         returns <b>false</b> if the operation is rejected.
 */
bool DisableBle(void);

/**
 * @brief Enable classic.
 *
 * @return Returns <b>true</b> if the operation is accepted;
 *         returns <b>false</b> if the operation is rejected.
 */
bool EnableBt(void);

/**
 * @brief Disable classic.
 *
 * @return Returns <b>true</b> if the operation is accepted;
 *         returns <b>false</b> if the operation is rejected.
 */
bool DisableBt(void);

/**
 * @brief Get classic enable/disable state.
 *
 * @return Returns classic enable/disable state, {@link BtStackState}.
 */
int GetBtState();

/**
 * @brief Get ble enable/disable state.
 *
 * @return Returns <b>true</b> if ble is enabled;
 *         returns <b>false</b> if ble is not enabled.
 */
bool IsBleEnabled();

/**
 * @brief Get local host bluetooth address.
 * 
 * @return Returns <b>true</b> if the operation is accepted;
 *         returns <b>false</b> if the operation is rejected.
 */
bool GetLocalAddr(unsigned char *mac, unsigned int len);

/**
 * @brief Set local device name.
 * 
 * @param localName Device name.
 * @param length localName length, The maximum length of the name is {@link OHOS_BD_NAME_LEN}.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetLocalName(unsigned char *localName, unsigned char length);

/**
 * @brief Set device scan mode.
 * @param mode Scan mode, see details {@link GapBtScanMode}.
 * @param duration Scan time.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetBtScanMode(int mode, int duration);

/**
 * @brief Check device pair request reply.
 *
 * @param bdAddr device address.
 * @param transport Transport type, details see {@link BtTransportId}
 * @param accept Set gap accept flag.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool PairRequestReply(const BdAddr *bdAddr, int transport, bool accept);

/**
 * @brief Set device pairing confirmation.
 *
 * @param bdAddr device address.
 * @param transport Transport type, details see {@link BtTransportId}
 * @param accept Set gap accept flag.
 * @return Returns <b>true</b> if the operation is successful;
 *         returns <b>false</b> if the operation fails.
 */
bool SetDevicePairingConfirmation(const BdAddr *bdAddr, int transport, bool accept);

/**
 * @brief Gap state changed callback.
 * 
 * @param transport Transport type when state change, details see {@link BtTransportId}
 * @param status Change to the new state, details see {@link BtStackState}.
 */
typedef void (*GapStateChangedCallback)(const int transport, const int status);

/**
 * @brief ACL state changed callback.
 * 
 * @param bdAddr device address.
 * @param state details of acl state changed.
 * @param reason clarify detals of REASON CODE from stack - If possible, suggest using the standard HCI error code.
 */
typedef void (*GapAclStateChangedCallback)(const BdAddr *bdAddr, GapAclState state, unsigned int reason);

/**
 * @brief Discovery state changed callback.
 * 
 * @param status DISCOVERY_STARTED/DISCOVERYING/DISCOVERY_STOPED
 */
typedef void (*GapDiscoveryStateChangedCallback)(int status);

/**
 * @brief Discovery state changed observer.
 * 
 * @param status Device discovery status.
 */
typedef void (*GapDiscoveryResultCallback)(const BdAddr *bdAddr);

/**
 * @brief Pair request observer.
 * 
 * @param bdAddr Remote device address.
 * @param transport Transport type, details see {@link BtTransportId}
 */
typedef void (*GapPairRequestedCallback)(const BdAddr *bdAddr, int transport);

/**
 * @brief Pair confirmed observer.
 * 
 * @param bdAddr Remote device address.
 * @param transport Transport type, details see {@link BtTransportId}
 * @param reqType Pair type.
 * @param number Paired passkey.
 */
typedef void (*GapPairConfirmedCallback)(const BdAddr *bdAddr, int transport, int reqType, int number);

/**
 * @brief Scan mode changed observer.
 * 
 * @param mode Device scan mode.
 */
typedef void (*GapScanModeChangedCallback)(int mode);

/**
 * @brief Device name changed observer.
 * 
 * @param deviceName Device name.
 */
typedef void (*GapLocalDeviceNameChangedCallback)(const unsigned char *deviceName, unsigned char length);

/**
 * @brief Device address changed observer.
 * 
 * @param bdAddr Device address.
 */
typedef void (*GapLocalDeviceAddrChangedCallback)(const BdAddr *bdAddr);

/**
 * @brief Pair status changed observer.
 * 
 * @param bdAddr Device address.
 * @param status Remote device pair status.
 */
typedef void (*GapPairStatusChangedCallback)(const BdAddr *bdAddr, int status);

/**
 * @brief Remote uuid changed observer.
 * 
 * @param bdAddr Device address.
 * @param uuid Remote device uuids.
 */
typedef void (*GapRemoteUuidChangedCallback)(const BdAddr *bdAddr, BtUuid uuid);

/**
 * @brief Remote name changed observer.
 * 
 * @param bdAddr Device address.
 * @param deviceName Remote device name.
 */
typedef void (*GapRemoteNameChangedCallback)(const BdAddr *bdAddr,
    const unsigned char *deviceName, unsigned char length);

/**
 * @brief Remote alias changed observer.
 * 
 * @param bdAddr Device address.
 * @param alias Remote device alias.
 */
typedef void (*GapRemoteAliasChangedCallback)(const BdAddr *bdAddr, const unsigned char *alias, unsigned char length);

/**
 * @brief Remote cod changed observer.
 * 
 * @param bdAddr Device address.
 * @param cod Remote device cod.
 */
typedef void (*GapRemoteCodChangedCallback)(const BdAddr *bdAddr, int cod);

/**
 * @brief Remote battery level changed observer.
 * 
 * @param bdAddr Device address.
 * @param cod Remote device battery Level.
 */
typedef void (*GapRemoteBatteryLevelChangedCallback)(const BdAddr *bdAddr, int batteryLevel);

/**
 * @brief Remote rssi event observer.
 * 
 * @param bdAddr Device address.
 * @param rssi Remote device rssi.
 * @param status Read status.
 */
typedef void (*GapReadRemoteRssiEventCallback)(const BdAddr *bdAddr, int rssi, int status);

/**
 * @brief Query application whether to accept the connection.
 * 
 * @param bdAddr Device address.
 * @param res Application decision, 0 is reject, 1 is accept.
 */
typedef void (*GapIsAcceptConnOnSafeModeCallback)(const BdAddr *bdAddr, bool *res);

/**
 * @brief Defines callbacks for the GAP
 */
typedef struct {
    GapStateChangedCallback stateChangeCallback;
    GapAclStateChangedCallback aclStateChangedCallbak;
    GapScanModeChangedCallback scanModeChangedCallback;
    GapPairStatusChangedCallback pairStatusChangedCallback;
    GapLocalDeviceNameChangedCallback deviceNameChangedCallback;
    GapLocalDeviceAddrChangedCallback deviceAddrChangedCallback;
    GapPairRequestedCallback pairRequestedCallback;
    GapPairConfirmedCallback pairConfiremedCallback;
    GapDiscoveryStateChangedCallback discoveryStateChangedCallback;
    GapDiscoveryResultCallback discoveryResultCallback;
    GapRemoteUuidChangedCallback remoteUuidChangedCallback;
    GapRemoteNameChangedCallback remoteNameChangedCallback;
    GapRemoteAliasChangedCallback remoteAliasChangedCallback;
    GapRemoteCodChangedCallback remoteCodChangedCallback;
    GapRemoteBatteryLevelChangedCallback remoteBatteryLevelChangedCallback;
    GapReadRemoteRssiEventCallback readRemoteRssiEventCallback;
    GapIsAcceptConnOnSafeModeCallback isAcceptConnOnSafeModeCallback;
} BtGapCallBacks;

/**
 * @brief Registers GAP callbacks.
 * 
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the callbacks are registered;
 * returns an error code defined in {@link BtStatus} otherwise.
 */
int GapRegisterCallbacks(BtGapCallBacks *func);

#ifdef __cplusplus
}
#endif
#endif