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

/**
 * @file btm.h
 *
 * @brief BTMNG module.
 *
 */

#ifndef BTM_H
#define BTM_H

#include <stdint.h>
#include <stdbool.h>

#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize function for bluetooth stack.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_Initialize();

/**
 * @brief Close function for bluetooth stack.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_Close();

#define BREDR_CONTROLLER 1
#define LE_CONTROLLER 2

/**
 * @brief Enable BR/EDR controller or LE controller
 *
 * @param controller <b>BREDR_CONTROLLER</b> or <b>LE_CONTROLLER</b>.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_Enable(int controller);

/**
 * @brief Disable BR/EDR controller or LE controller
 *
 * @param controller <b>BREDR_CONTROLLER</b> or <b>LE_CONTROLLER</b>.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_Disable(int controller);

/**
 * @brief Get current state for BR/EDR controller or LE controller
 *
 * @param controller <b>BREDR_CONTROLLER</b> or <b>LE_CONTROLLER</b>.
 * @return Returns <b>true</b> if the controller is enabled; returns <b>false</b> if the controller is disabled.
 */
bool BTSTACK_API BTM_IsEnabled(int controller);

typedef struct {
    void (*hciFailure)(void *context);
} BtmCallbacks;

/**
 * @brief Register callback functions.
 *
 * @param callbacks Point to <b>BtmCallbacks</b> struct, the struct must be available before calling to
 *                  <b>BTM_DeregisterCallbacks</b>.
 * @param context The context for callback functions.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_RegisterCallbacks(const BtmCallbacks *callbacks, void *context);

/**
 * @brief Deregister callback functions.
 *
 * @param callbacks Point to <b>BtmCallbacks</b> struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_DeregisterCallbacks(const BtmCallbacks *callbacks);

/**
 * ACL
 */

/**
 * @brief Create ACL connection to BR/EDR device.
 *
 * @param addr The address of BR/EDR device.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_AclConnect(const BtAddr *addr);

/**
 * @brief Cancel the connect operation to BR/EDR device.
 *
 * @param addr The address of BR/EDR device.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_AclCancelConnect(const BtAddr *addr);

/**
 * @brief Create ACL connection to LE device.
 *
 * @param addr The address of LE device.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_LeConnect(const BtAddr *addr);

/**
 * @brief Cancel the connect operation to LE device.
 *
 * @param addr The address of LE device.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_LeCancelConnect(const BtAddr *addr);

/**
 * @brief Set LE connection mode to fast.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetLeConnectionModeToFast();

/**
 * @brief Set LE connection mode to slow.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetLeConnectionModeToSlow();

/**
 * @brief Disconnect an ACL connection.
 *
 * @param connectionHandle The connection handle.
 * @param reason The reason for peer device.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_AclDisconnect(uint16_t connectionHandle, uint8_t reason);

/**
 * @brief Add a reference to the ACL connection.
 *
 * @param connectionHandle The connection handle.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_AclAddRef(uint16_t connectionHandle);

/**
 * @brief Release the reference to the ACL connection.
 *
 * @param connectionHandle The connection handle.
 */
void BTSTACK_API BTM_AclRelease(uint16_t connectionHandle);

/**
 * @brief Get the transpot type of the ACL connection.
 *
 * @param connectionHandle The connection handle.
 * @return <b>TRANSPORT_BREDR</b> or <b>TRANSPORT_LE</b>.
 */
uint8_t BTSTACK_API BTM_GetAclTranspot(uint16_t connectionHandle);

/**
 * @brief Get the RSSI value of remote LE device.
 *
 * @param addr The address of remote LE device.
 * @return The RSSI value.
 */
int BTSTACK_API BTM_ReadRssi(const BtAddr *addr);

#define BTM_ROLE_MASTER 0x00
#define BTM_ROLE_SLAVE 0x01

/**
 * @brief Switch role for a BR/EDR connection.
 *
 * @param addr The address device.
 * @param role <b>BTM_ROLE_MASTER</b> or <b>BTM_ROLE_SLAVE</b>.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SwitchRole(const BtAddr *addr, uint8_t role);

/**
 * @brief Determine whether the connection is a secure connection.
 *
 * @param addr The address device.
 * @return Returns <b>true</b> if it's a secure connection; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsSecureConnection(const BtAddr *addr);

typedef void (*BTMRemoteDeviceSupportCallback)(const BtAddr *addr, bool support);

void BTSTACK_API BTM_IsRemoteDeviceSupportHostSecureSimplePairing(
    const BtAddr *addr, BTMRemoteDeviceSupportCallback callback);
void BTSTACK_API BTM_IsRemoteDeviceSupportConnectionParametersRequest(
    const BtAddr *addr, BTMRemoteDeviceSupportCallback callback);
void BTSTACK_API BTM_IsRemoteDeviceSupportEdrAcl2MbMode(const BtAddr *addr, BTMRemoteDeviceSupportCallback callback);
void BTSTACK_API BTM_IsRemoteDeviceSupportEdrAcl3MbMode(const BtAddr *addr, BTMRemoteDeviceSupportCallback callback);

#define LE_CONNECTION_ROLE_MASTER 0x00
#define LE_CONNECTION_ROLE_SLAVE 0x01

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    const BtAddr *addr;
    uint8_t classOfDevice[3];
    bool encyptionEnabled;
} BtmAclConnectCompleteParam;

typedef struct {
    void (*connectionComplete)(const BtmAclConnectCompleteParam *param, void *context);

    void (*disconnectionComplete)(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);

    void (*leConnectionComplete)(
        uint8_t status, uint16_t connectionHandle, const BtAddr *addr, uint8_t role, void *context);

    void (*leDisconnectionComplete)(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);

    void (*readRssiComplete)(uint8_t status, const BtAddr *addr, int8_t rssi, void *context);

    void (*roleChange)(uint8_t status, const BtAddr *addr, uint8_t newRole, void *context);
} BtmAclCallbacks;

/**
 * @brief Register callback functions for ACL features.
 *
 * @param callbacks Point to <b>BtmAclCallbacks</b> struct, the struct must be available before calling to
 *                  <b>BTM_DeregisterAclCallbacks</b>.
 * @param context The context for callback functions.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_RegisterAclCallbacks(const BtmAclCallbacks *callbacks, void *context);

/**
 * @brief Deregister callback functions for ACL features.
 *
 * @param callbacks Point to <b>BtmAclCallbacks</b> struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_DeregisterAclCallbacks(const BtmAclCallbacks *callbacks);

/**
 * @brief Get local address and peer address for LE connection.
 *
 * @param connectionHandle The connection handle.
 * @param localAddr Point to the local address struct.
 * @param peerAddr Point to the peer address struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLeConnectionAddress(uint16_t connectionHandle, BtAddr *localAddr, BtAddr *peerAddr);

#define BTM_LINK_POLICY_DISABLE_All 0x0000
#define BTM_LINK_POLICY_ENABLE_ROLE_SWITCH 0x0001
#define BTM_LINK_POLICY_ENABLE_HOLD_MODE 0x0002
#define BTM_LINK_POLICY_ENABLE_SNIFF_MODE 0x0004

/**
 * @brief Set default Link Policy Settings.
 *
 * @param linkPolicySettings The default Link Policy Settings.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetDefaultLinkPolicySettings(uint16_t linkPolicySettings);

/**
 * @brief Set Link Policy Settings for BR/EDR connection.
 *
 * @param addr Point to the remote address struct.
 * @param linkPolicySettings The default Link Policy Settings.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetLinkPolicySettings(const BtAddr *addr, uint16_t linkPolicySettings);

#define BTM_ACL_PACKET_TYPE_NO_2_DH1 0x0002
#define BTM_ACL_PACKET_TYPE_NO_3_DH1 0x0004
#define BTM_ACL_PACKET_TYPE_DM1 0x0008
#define BTM_ACL_PACKET_TYPE_DH1 0x0010
#define BTM_ACL_PACKET_TYPE_NO_2_DH3 0x0100
#define BTM_ACL_PACKET_TYPE_NO_3_DH3 0x0200
#define BTM_ACL_PACKET_TYPE_DM3 0x0400
#define BTM_ACL_PACKET_TYPE_DH3 0x0800
#define BTM_ACL_PACKET_TYPE_NO_2_DH5 0x1000
#define BTM_ACL_PACKET_TYPE_NO_3_DH5 0x2000
#define BTM_ACL_PACKET_TYPE_DM5 0x4000
#define BTM_ACL_PACKET_TYPE_DH5 0x8000

#define BTM_ACL_PACKET_TYPE_DEFAULT                                                                          \
    (BTM_ACL_PACKET_TYPE_DM1 | BTM_ACL_PACKET_TYPE_DH1 | BTM_ACL_PACKET_TYPE_DM3 | BTM_ACL_PACKET_TYPE_DH3 | \
        BTM_ACL_PACKET_TYPE_DM5 | BTM_ACL_PACKET_TYPE_DH5)

/**
 * @brief Change packet types for BR/EDR connection.
 *
 * @param addr Point to the remote address struct.
 * @param packetType The packet types.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_ChangeConnectionPacketType(const BtAddr *addr, uint16_t packetType);

/**
 * Controller
 */

/**
 * @brief Determine whether the local controller supports BR/EDR.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportBrEdr();

/**
 * @brief Determine whether the local controller supports LE.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLe();

/**
 * @brief Determine whether the local controller supports RSSI Inquiry Response.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportRssiInquiryResponse();

/**
 * @brief Determine whether the local controller supports EIR Inquiry Response.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportEirInquiryResponse();

/**
 * @brief Determine whether the local controller supports Role Switch.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportRoleSwitch();

/**
 * @brief Determine whether the local controller supports Hold Mode.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportHoldMode();

/**
 * @brief Determine whether the local controller supports Sniff Mode.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportSniffMode();

/**
 * @brief Determine whether the local controller supports eSCO.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportEsco();

/**
 * @brief Determine whether the local controller supports Secure Simple Pairing.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportSecureSimplePairing();

/**
 * @brief Determine whether the local controller supports Secure Connections.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportSecureConnections();

/**
 * @brief Determine whether the local controller supports Non-flushable Packet Boundary Flag.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportNonFlushablePacketBoundaryFlag();

/**
 * @brief Determine whether the local controller supports Connection Parameters Request Procedure.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportConnectionParametersRequestProcedure();

/**
 * @brief Determine whether the local controller supports LE Ping.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLePing();

/**
 * @brief Determine whether the local controller supports LE Data Packet Length Extension.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLeDataPacketLengthExtension();

/**
 * @brief Determine whether the local controller supports LL Privacy.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLlPrivacy();

/**
 * @brief Determine whether the local controller supports LE 2M PHY.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLe2MPhy();

/**
 * @brief Determine whether the local controller supports LE Coded PHY.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLeCodedPhy();

/**
 * @brief Determine whether the local controller supports LE Extended Advertising.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLeExtendedAdvertising();

/**
 * @brief Determine whether the local controller supports LE Periodic Advertising.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportLePeriodicAdvertising();

/**
 * @brief Determine whether the local controller supports Channel Selection Algorithm #2.
 *
 * @return Returns <b>true</b> if supported; otherwise returns <b>false</b>.
 */
bool BTSTACK_API BTM_IsControllerSupportChannelSelectionAlgorithm2();

/**
 * @brief Get local address.
 *
 * @param addr Point to <b>BtAddr</b> struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLocalAddr(BtAddr *addr);

/**
 * @brief Get local supported features.
 *
 * @param features Point to 8 bytes buffer to obtain the supported features.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLocalSupportedFeatures(uint8_t features[8]);

typedef struct {
    uint8_t hciVersion;
    uint16_t hciRevision;
    uint8_t lmpVersion;
    uint16_t manufacturerName;
    uint16_t lmpSubversion;
} BtmLocalVersionInformation;

/**
 * @brief Get local version information.
 *
 * @param localVersion Point to a <b>BtmLocalVersionInformation</b> struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLocalVersionInformation(BtmLocalVersionInformation *localVersion);

typedef struct {
    uint16_t companyID;
    uint16_t vendorDefinedCodecID;
} BtmVendorSpecificCodec;

typedef struct {
    uint8_t numberOfSupportedCodecs;
    uint8_t *supportedCodecs;
    uint8_t numberOfSupportedVendorSpecificCodecs;
    BtmVendorSpecificCodec *vendorSpecificCodecs;
} BtmLocalSupportedCodecs;

/**
 * @brief Get local supported codecs.
 *
 * @param localSupportedCodes Point to a <b>BtmLocalSupportedCodecs</b> pointer. Do not free this pointer.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLocalSupportedCodecs(BtmLocalSupportedCodecs **localSupportedCodes);

/**
 * @brief Get BR/EDR ACL data length.
 *
 * @param aclDataPacketLength Obtain the ACL data length.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetAclDataPacketLength(uint16_t *aclDataPacketLength);

/**
 * @brief Get LE ACL data length.
 *
 * @param length Obtain the ACL data length.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLeAclDataPacketLength(uint16_t *length);

/**
 * LE Security
 */

#define KEY_SIZE 16

typedef struct {
    uint8_t key[KEY_SIZE];
} BtmKey;

typedef struct {
    BtmKey key;
    uint64_t rand;
    uint16_t ediv;
} BtmLeEncryptionKey;

typedef struct {
    BtAddr addr;
    BtmKey remoteIdentityResolvingKey;
    BtAddr remoteIdentityAddress;
} BtmLePairedDevice;

/**
 * @brief Set local Identity Resolving Key.
 *
 * @param key The Identity Resolving Key.
 */
void BTSTACK_API BTM_SetLocalIdentityResolvingKey(const BtmKey *key);

/**
 * @brief Set LE Paired Devices.
 *
 * @param pairedDevices The array of LE Paired Devices.
 * @param count The count of array.
 */
void BTSTACK_API BTM_SetLePairedDevices(const BtmLePairedDevice *pairedDevices, uint16_t count);

/**
 * @brief Add a LE Paired Devices.
 *
 * @param device Point to a <b>BtmLePairedDevice</b> struct.
 */
void BTSTACK_API BTM_AddLePairedDevice(const BtmLePairedDevice *device);

/**
 * @brief Remove a LE Paired Devices.
 *
 * @param addr The address of the removing device.
 */
void BTSTACK_API BTM_RemoveLePairedDevice(const BtAddr *addr);

#define OWN_ADDRESS_TYPE_PUBLIC 0x00
#define OWN_ADDRESS_TYPE_RANDOM 0x01

/**
 * @brief Set local address type for LE.
 *
 * @param ownAddressType Should be <b>OWN_ADDRESS_TYPE_PUBLIC</b> or <b>OWN_ADDRESS_TYPE_RANDOM</b>.
 */
void BTSTACK_API BTM_SetOwnAddressType(uint32_t ownAddressType);

/**
 * @brief Get local address type for LE.
 *
 * @return Return <b>OWN_ADDRESS_TYPE_PUBLIC</b> or <b>OWN_ADDRESS_TYPE_RANDOM</b>.
 */
uint8_t BTM_GetOwnAddressType();

/**
 * @brief Set LE random address.
 *
 * @param addr The random address.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetLeRandomAddress(const BtAddr *addr);

/**
 * @brief Get LE random address.
 *
 * @param addr The random address.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_GetLeRandomAddress(BtAddr *addr);

/**
 * Power Management
 */
typedef struct {
    uint16_t maxInterval;
    uint16_t minInterval;
    uint16_t attempt;
    uint16_t timeout;
} BtmSniffParam;

/**
 * @brief Enter Sniff mode.
 *
 * @param addr The remote device address.
 * @param param The Sniff parameters.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_EnterSniffMode(const BtAddr *addr, const BtmSniffParam *param);

/**
 * @brief Exit Sniff mode.
 *
 * @param addr The remote device address.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_ExitSniffMode(const BtAddr *addr);

typedef struct {
    uint16_t maximumLatency;
    uint16_t minimumRemoteTimeout;
    uint16_t minimumLocalTimeout;
} BtmSniffSubrating;

/**
 * @brief Set Sniff Subrating.
 *
 * @param addr The remote device address.
 * @param subRating The Sniff Subrating parameters.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetSniffSubrating(const BtAddr *addr, const BtmSniffSubrating *subRating);

#define BTM_PM_ACTIVE_MODE 0x00
#define BTM_PM_HOLD_MODE 0x01
#define BTM_PM_SNIFF_MODE 0x02

typedef struct {
    void (*modeChange)(uint8_t status, const BtAddr *addr, uint8_t currentMode, uint16_t interval, void *context);
    void (*setSniffSubratingComplete)(uint8_t status, const BtAddr *addr, void *context);
} BtmPmCallbacks;

/**
 * @brief Register callback functions for Power Management features.
 *
 * @param callbacks Point to <b>BtmPmCallbacks</b> struct, the struct must be available before calling to
 *                  <b>BTM_DeregisterPmCallbacks</b>.
 * @param context The context for callback functions.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_RegisterPmCallbacks(const BtmPmCallbacks *callbacks, void *context);

/**
 * @brief Deregister callback functions for Power Management features.
 *
 * @param callbacks Point to <b>BtmPmCallbacks</b> struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_DeregisterPmCallbacks(const BtmPmCallbacks *callbacks);

/**
 * SCO
 */
typedef struct {
    BtAddr addr;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    uint16_t maxLatency;
    uint16_t voiceSetting;
    uint8_t retransmissionEffort;
    uint16_t packetType;
} BtmCreateScoConnectionParam;

/**
 * @brief Create SCO connection.
 *
 * @param param The parameters of creating a SCO connection.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_CreateScoConnection(const BtmCreateScoConnectionParam *param);

typedef struct {
    uint16_t connectionHandle;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    uint16_t maxLatency;
    uint16_t voiceSetting;
    uint8_t retransmissionEffort;
    uint16_t packetType;
} BtmModifyScoConnectionParam;

/**
 * @brief Modify SCO connection.
 *
 * @param param The parameters of modifying SCO connection.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_ModifyScoConnection(const BtmModifyScoConnectionParam *param);

typedef BtmCreateScoConnectionParam BtmAcceptScoConnectionRequestParam;

/**
 * @brief Accept a SCO connection request.
 *
 * @param param The parameters of accepting a SCO connection request.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_AcceptScoConnectionRequest(const BtmAcceptScoConnectionRequestParam *param);

typedef struct {
    BtAddr addr;
    uint8_t reason;
} BtmRejectScoConnectionRequestParam;

/**
 * @brief Reject a SCO connection request.
 *
 * @param param The parameters of rejecting a SCO connection request.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_RejectScoConnectionRequest(const BtmRejectScoConnectionRequestParam *param);

/**
 * @brief Diconnect a SCO/eSco connection request.
 *
 * @param connectionHandle The connection handle.
 * @param reason The disconnect reason for remote device.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_DisconnectScoConnection(uint16_t connectionHandle, uint8_t reason);

#define CODEC_CVSD 0
#define CODEC_MSBC_T1 1
#define CODEC_MSBC_T2 2

typedef struct {
    BtAddr addr;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    uint8_t codec;
    uint16_t maxLatency;
    uint16_t packetType;
    uint8_t retransmissionEffort;
} BtmCreateEscoConnectionParam;

/**
 * @brief Create eSCO connection.
 *
 * @param param The parameters of creating a eSCO connection.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_CreateEscoConnection(const BtmCreateEscoConnectionParam *param);

typedef struct {
    uint16_t connectionHandle;
    uint32_t transmitBandwidth;
    uint32_t receiveBandwidth;
    uint8_t codec;
    uint16_t maxLatency;
    uint16_t packetType;
    uint8_t retransmissionEffort;
} BtmModifyEscoConnectionParam;

/**
 * @brief Modify eSCO connection.
 *
 * @param param The parameters of modifying eSCO connection.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_ModifyEscoConnection(const BtmModifyEscoConnectionParam *param);

typedef BtmCreateEscoConnectionParam BtmAcceptEscoConnectionRequestParam;

/**
 * @brief Accept a eSCO connection request.
 *
 * @param param The parameters of accepting a eSCO connection request.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_AcceptEscoConnectionRequest(const BtmAcceptEscoConnectionRequestParam *param);

#define LINK_TYPE_SCO 0x00
#define LINK_TYPE_ESCO 0x02

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    const BtAddr *addr;
} BtmScoConnectionCompleteParam;

typedef struct {
    uint16_t connectionHandle;
    uint8_t transmissionInterval;
    uint8_t retransmissionWindow;
    uint16_t rxPacketLength;
    uint16_t txPacketLength;
} BtmScoConnectionChangedParam;

typedef struct {
    uint8_t status;
    uint16_t connectionHandle;
    uint8_t reason;
} BtmScoDisconnectionCompleteParam;

typedef struct {
    const BtAddr *addr;
    uint8_t linkType;
} BtmScoConnectionRequestParam;

typedef struct {
    void (*scoConnectionComplete)(const BtmScoConnectionCompleteParam *param, void *context);

    void (*scoConnectionChanged)(const BtmScoConnectionChangedParam *param, void *context);

    void (*scoDisconnectionComplete)(const BtmScoDisconnectionCompleteParam *param, void *context);

    void (*scoConnectionRequest)(const BtmScoConnectionRequestParam *param, void *context);

    void (*writeVoiceSettingComplete)(uint8_t status, void *context);
} BtmScoCallbacks;

/**
 * @brief Register callback functions for SCO/eSCO features.
 *
 * @param callbacks Point to <b>BtmScoCallbacks</b> struct, the struct must be available before calling to
 *                  <b>BTM_DeregisterScoCallbacks</b>.
 * @param context The context for callback functions.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_RegisterScoCallbacks(const BtmScoCallbacks *callbacks, void *context);

/**
 * @brief Deregister callback functions for SCO/eSCO features.
 *
 * @param callbacks Point to <b>BtmScoCallbacks</b> struct.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_DeregisterScoCallbacks(const BtmScoCallbacks *callbacks);

/**
 * @brief Write voice setting.
 *
 * @param voiceSetting The value of voice setting.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_WriteVoiceSetting(uint16_t voiceSetting);

/**
 * Snoop
 */

/**
 * @brief Set snoop file output path.
 *
 * @param path Point to the path string.
 * @param length The length of path.
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_SetSnoopFilePath(const char *path, uint16_t length);

/**
 * @brief Enable snoop file output.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_EnableSnoopFileOutput(bool filter);

/**
 * @brief Disable snoop file output.
 *
 * @return Returns <b>BT_NO_ERROR</b> if the operation is successful; returns others if the operation fails.
 */
int BTSTACK_API BTM_DisableSnoopFileOutput();

/**
 * Log
 */
int BTSTACK_API BTM_EnableHciLogOutput(bool filter);
int BTSTACK_API BTM_DisableHciLogOutput();

/**
 * Snoop filter
 */

#define BTM_HCI_LOG_FILTER_MODULE_HFP 0x01
#define BTM_HCI_LOG_FILTER_MODULE_PBAP 0x02
#define BTM_HCI_LOG_FILTER_MODULE_MAP 0x03
#define BTM_HCI_LOG_FILTER_MODULE_RFCOMM 0x04
#define BTM_HCI_LOG_FILTER_MODULE_AVCTP 0x05
#define BTM_HCI_LOG_FILTER_MODULE_AVCTP_BROWSING 0x06
#define BTM_HCI_LOG_FILTER_MODULE_AVDTP 0x07
#define BTM_HCI_LOG_FILTER_MODULE_SPP 0x08
#define BTM_HCI_LOG_FILTER_MODULE_ATT 0x09
#define BTM_HCI_LOG_FILTER_MODULE_SM 0x0A

void BTSTACK_API BTM_AddLocalL2capPsmForLogging(uint8_t module, uint16_t psm);
void BTSTACK_API BTM_AddRemoteL2capPsmForLogging(uint8_t module, uint16_t psm, const BtAddr *remoteAddr);
void BTSTACK_API BTM_RemoveLocalL2capPsmForLogging(uint8_t module, uint16_t psm);
void BTSTACK_API BTM_RemoveRemoteL2capPsmForLogging(uint8_t module, uint16_t psm, const BtAddr *remoteAddr);

void BTSTACK_API BTM_AddLocalRfcommScnForLogging(uint8_t module, uint8_t scn);
void BTSTACK_API BTM_AddRemoteRfcommScnForLogging(uint8_t module, uint8_t scn, const BtAddr *remoteAddr);
void BTSTACK_API BTM_RemoveLocalRfcommScnChannelForLogging(uint8_t module, uint8_t scn);
void BTSTACK_API BTM_RemoveRemoteRfcommScnChannelForLogging(uint8_t module, uint8_t scn, const BtAddr *remoteAddr);

#ifdef __cplusplus
}
#endif

#endif  // BTM_H
