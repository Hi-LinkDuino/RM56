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

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief Generic Access Profile
 *
 */

/**
 * @file gap.h
 *
 * @brief bluetooth gap interface
 *
 */

#ifndef GAP_IF_H
#define GAP_IF_H

#include "gap_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Result callback of set scan mode.
 * @param[in]   status              Result of mode setting.
 * @param[in]   context             The context of the caller.
 * @return      @c void
 */
typedef void (*GapSetScanModeResultCallback)(uint8_t status, void *context);

/**
 * @brief       Discoverability modes configuration parameter structure
 */
typedef struct {
    uint8_t mode;           /// Discoverable Mode
    uint8_t type;           /// Inquiry Scan Type
    uint16_t scanInterval;  /// The amount of time between consecutive inquiry scans.
    uint16_t scanWindow;    /// The amount of time for the duration of the inquiry scan.
} GapDiscoverModeInfo;

/**
 * @brief       Connectionability modes configuration parameter structure
 */
typedef struct {
    uint8_t mode;           /// Connectable Mode
    uint8_t type;           /// Page Scan Type
    uint16_t scanInterval;  /// The amount of time between consecutive page scans.
    uint16_t scanWindow;    /// The amount of time for the duration of the page scan.
} GapConnectableModeInfo;

/**
 * @brief       Service connection direction.
 */
typedef enum {
    OUTGOING,
    INCOMING,
} GAP_ServiceConnectDirection;

#define MAP_MAX_COUNT (13)

/**
 * @brief       Service identity.
 */
typedef enum {
    UNKNOWN_SERVICE,
    GAP,
    SDP,
    GAVDP_INT,
    GAVDP_ACP,
    A2DP_SINK,
    A2DP_SRC,
    AVRCP_CT,
    AVRCP_TG,
    AVRCP_CT_BROWSING,
    AVRCP_TG_BROWSING,
    HFP_HF,
    HFP_AG,
    PBAP_CLIENT,
    PBAP_SERVER,
    MAP_CLIENT_ID_START,
    MAP_CLIENT_ID_END = MAP_CLIENT_ID_START + MAP_MAX_COUNT,
    MAP_SERVER_ID_START,
    MAP_SERVER_ID_END = MAP_SERVER_ID_START + MAP_MAX_COUNT,
    GATT_CLIENT,
    GATT_SERVER,
    RFCOMM,
    SPP_ID_START,
    HID_HOST,
    PAN_BNEP,
    OPP_CLIENT,
    OPP_SERVER,
} GAP_Service;

/**
 * @brief       Multiplexing protocol identity.
 */
typedef enum {
    SEC_PROTOCOL_L2CAP,
    SEC_PROTOCOL_RFCOMM,
} GAP_SecMultiplexingProtocol;

/**
 * @brief       Protocol channel.
 */
typedef union {
    uint16_t l2capPsm;
    uint8_t rfcommChannel;
} GapSecChannel;

/// Security attributes
#define GAP_SEC_IN_AUTHORIZATION (1 << 0)
#define GAP_SEC_IN_AUTHENTICATION (1 << 1)
#define GAP_SEC_IN_ENCRYPTION (1 << 2)
#define GAP_SEC_MITM (1 << 3)
#define GAP_SEC_OUT_AUTHENTICATION (1 << 4)
#define GAP_SEC_OUT_ENCRYPTION (1 << 5)

/**
 * @brief       Security request information structure.
 */
typedef struct {
    GAP_ServiceConnectDirection direction;
    GAP_Service serviceId;
    GAP_SecMultiplexingProtocol protocolId;
    GapSecChannel channelId;
} GapServiceSecurityInfo;

/**
 * @brief       Service security request result callback
 * @param[in]   result              security request result
 * @param[in]   serviceInfo         security request information
 * @param[in]   context             security request context
 * @return      @c void
 *
 */
typedef void (*GapSecurityResultCallback)(uint16_t result, GapServiceSecurityInfo serviceInfo, void *context);

/**
 * @brief       parameter of interface GAP_RequestSecurity.
 */
typedef struct {
    GapServiceSecurityInfo info;         /// Security request information
    GapSecurityResultCallback callback;  /// Service request security result callback function
    void *context;                       /// Service request security result callback function context parameter
} GapRequestSecurityParam;

/**
 * @brief       Security modes of BR/EDR physical transport.
 */
typedef enum {
    SEC_MODE_2,
    SEC_MODE_4,
} GAP_SecurityMode;

/**
 * @brief       Service security verify callback structure of BR/EDR physical transport.
 */
typedef struct {
    /// Service authorization verify callback
    void (*authorizeInd)(const BtAddr *addr, GAP_Service service, void *context);
} GapSecurityCallback;

/**
 * @brief       authentication callback structure.
 */
typedef struct {
    void (*userConfirmReq)(const BtAddr *addr, uint32_t number, void *context);
    void (*userPasskeyReq)(const BtAddr *addr, void *context);
    void (*userPasskeyNotification)(const BtAddr *addr, uint32_t number, void *context);
    void (*remoteOobReq)(const BtAddr *addr, void *context);
    void (*pinCodeReq)(const BtAddr *addr, void *context);
    void (*linkKeyReq)(const BtAddr *addr, void *context);
    void (*linkKeyNotification)(
        const BtAddr *addr, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType, void *context);
    void (*simplePairComplete)(const BtAddr *addr, uint8_t status, void *context);
    void (*IOCapabilityReq)(const BtAddr *addr, void *context);
    void (*IOCapabilityRsp)(const BtAddr *addr, uint8_t ioCapability, void *context);
    void (*authenticationComplete)(const BtAddr *addr, uint8_t status, void *context);
    void (*encryptionChangeCallback)(const BtAddr *addr, uint8_t status, void *context);
} GapAuthenticationCallback;

/**
 * @brief       Encryption change result callback
 * @param[in]   addr                encryption change target device address
 * @param[in]   status              encryption change result status
 * @param[in]   context             encryption change context
 * @return      @c void
 */
typedef void (*GapEncryptionChangeCallback)(const BtAddr *addr, uint8_t status, void *context);

/// Inquiry mode
#define GAP_INQUIRY_MODE_GENERAL 0x00
#define GAP_INQUIRY_MODE_LIMITED 0x01

/**
 * @brief       Device discover callback structure.
 */
typedef struct {
    void (*inquiryResult)(const BtAddr *addr, uint32_t classOfDevice, void *context);
    void (*inquiryResultRssi)(const BtAddr *addr, uint32_t classOfDevice, int8_t rssi, void *context);
    void (*extendedInquiryResult)(
        const BtAddr *addr, uint32_t classOfDevice, int8_t rssi, const uint8_t eir[GAP_EIR_SIZE_MAX], void *context);
    void (*remoteName)(uint8_t status, const BtAddr *addr, const uint8_t name[GAP_NAME_SIZE_MAX], void *context);
    void (*inquiryComplete)(uint8_t status, void *context);
} GapDiscoveryCallback;

/**
 * @brief       Set local bluetooth device name. (Used for BR/EDR)
 * @param[in]   name                Bluetooth device name.
 * @param[in]   length              Length of the device name. (248 or less)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_SetLocalName(const char *name, int length);

/**
 * @brief       Set local bluetooth device class.
 * @param[in]   cod                 Bluetooth device class.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_SetClassOfDevice(uint32_t cod);

/**
 * @brief       Set local bluetooth device class.
 * @param[in]   cod                 Bluetooth device class.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_SetExtendedInquiryResponse(const uint8_t eir[GAP_EIR_SIZE_MAX]);

/**
 * @brief       Set scan mode of bluetooth BR/EDR physical transport.
 * @param[in]   discoverInfo        Discoverability modes configuration parameter.
 * @param[in]   connectableInfo     Connectionability modes configuration parameter.
 * @param[in]   callback            Callback function for the result.
 * @param[in]   context             The context of the callback function.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 * @see         BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part C
 *              4.1 DISCOVERABILITY MODES
 *              4.2 CONNECTABILITY MODES
 */
BTSTACK_API int GAPIF_SetScanMode(const GapDiscoverModeInfo *discoverInfo,
    const GapConnectableModeInfo *connectableInfo, GapSetScanModeResultCallback callback, void *context);

/**
 * @brief       Set boneable mode of bluetooth BR/EDR physical transport.
 * @param[in]   isBondable          boneable mode
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 * @see         BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part C
 *              4.3 BONDABLE MODES
 */
BTSTACK_API int GAPIF_SetBondableMode(uint8_t isBondable);

/**
 * @brief       Service register security requirements to GAP
 * @param[in]   addr                outgoing attributes to remote device
 * @param[in]   serviceInfo         security requirements information
 * @param[in]   securityMode        Security attributes
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterServiceSecurity(
    const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo, uint16_t securityMode);
BTSTACK_API int GAPIF_RegisterServiceSecurityAsync(
    const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo, uint16_t securityMode);

/**
 * @brief       Service deregister security requirements to GAP
 * @param[in]   addr                outgoing attributes to remote device
 * @param[in]   serviceInfo         security requirements information
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo);
BTSTACK_API int GAPIF_DeregisterServiceSecurityAsync(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo);

/**
 * @brief       Service request security requirements to GAP
 * @param[in]   addr                target device address
 * @param[in]   param               structure of security requirements information and result callback function
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RequestSecurity(const BtAddr *addr, const GapRequestSecurityParam *param);
BTSTACK_API int GAPIF_RequestSecurityAsync(const BtAddr *addr, const GapRequestSecurityParam *param);

/**
 * @brief       Register service security verify callback
 * @param[in]   callback            security verify callback
 * @param[in]   context             security verify callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterSecurityCallback(const GapSecurityCallback *callback, void *context);

/**
 * @brief       Deregister service security verify callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterSecurityCallback(void);

/**
 * @brief       Set security modes of BR/EDR physical transport
 * @param[in]   mode                security modes
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_SetSecurityMode(GAP_SecurityMode mode);

/**
 * @brief       Service authorization verify response
 * @param[in]   addr                target device address
 * @param[in]   service             service identity
 * @param[in]   accept              accept or reject
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_AuthorizeRes(const BtAddr *addr, GAP_Service service, uint8_t accept);

/**
 * @brief       Register authentication callback
 * @param[in]   callback            authentication callback structure
 * @param[in]   context             authentication verify callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterAuthenticationCallback(const GapAuthenticationCallback *callback, void *context);

/**
 * @brief       Deregister authentication callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterAuthenticationCallback(void);

/**
 * @brief       Get current pair originator
 * @param[in]   addr                pairing device address
 * @param[out]  isLocal             is local initiate
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_PairIsFromLocal(const BtAddr *addr, bool *isLocal);

/**
 * @brief       authenticate the remote device associated.
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_AuthenticationReq(const BtAddr *addr);

/**
 * @brief       cancel authenticate the remote device associated.
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_CancelAuthenticationReq(const BtAddr *addr);

/**
 * @brief       Respond IO capability request. Reply callback GapAuthenticationCallback::IOCapabilityReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   ioCapability        local device IO capability
 * @param[in]   oobDataPresent      OOB authentication data from remote device present
 * @param[in]   authReq             Authentication Requirements: MITM protection
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_IOCapabilityRsp(
    const BtAddr *addr, uint8_t accept, uint8_t ioCapability, uint8_t oobDataPresent, uint8_t authReq);

/**
 * @brief       Respond user confirmation request. Reply callback GapAuthenticationCallback::userConfirmReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_UserConfirmRsp(const BtAddr *addr, uint8_t accept);

/**
 * @brief       Respond user passkey request. Reply callback GapAuthenticationCallback::userPasskeyReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   number              user input number (000000 - 999999)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_UserPasskeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number);

/**
 * @brief       Respond remote OOB data request. Reply callback GapAuthenticationCallback::remoteOobReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   data                OOB data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RemoteOobRsp(const BtAddr *addr, uint8_t accept, const GapOOBData *data);

/**
 * @brief       Respond PIN code request. Reply callback GapAuthenticationCallback::pinCodeReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   pinCode             PIN code data
 * @param[in]   pinCodeLength       PIN code data length
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_PinCodeRsp(const BtAddr *addr, uint8_t accept, const uint8_t *pinCode, uint8_t pinCodeLength);

/**
 * @brief       Respond link key request. Reply callback GapAuthenticationCallback::linkKeyReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   linkKey             link key
 * @param[in]   keyType             link key type
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LinkKeyRsp(
    const BtAddr *addr, uint8_t accept, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType);

/**
 * @brief       Register device discover callback
 * @param[in]   callback            device discover callback structure
 * @param[in]   context             device discover callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterDiscoveryCallback(const GapDiscoveryCallback *callback, void *context);

/**
 * @brief       Deregister device discover callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterDiscoveryCallback(void);

/**
 * @brief       discover other nearby BR/EDR Controllers
 * @param[in]   mode                Inquiry mode
 * @param[in]   inquiryLength       Maximum inquiry time.(n * 1.28s)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_Inquiry(uint8_t mode, uint8_t inquiryLength);

/**
 * @brief       Cancel discover other nearby BR/EDR Controllers
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_InquiryCancel(void);

/**
 * @brief       Get remote device name
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_GetRemoteName(const BtAddr *addr);

/**
 * @brief       Cancel get remote device name
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_GetRemoteNameCancel(const BtAddr *addr);

/**
 * @brief       Get local bluetooth address from HCI.
 * @param[out]  addr                Bluetooth address of bluetooth chip.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_GetLocalAddr(BtAddr *addr);

#ifdef __cplusplus
}
#endif

#endif /* GAP_IF_H */
