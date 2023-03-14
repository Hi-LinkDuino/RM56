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

#ifndef GAP_LE_IF_H
#define GAP_LE_IF_H

#include "gap_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

/// BLE Roles
#define GAP_LE_ROLE_BROADCASTER (1 << 0)
#define GAP_LE_ROLE_OBSERVER (1 << 1)
#define GAP_LE_ROLE_PREIPHERAL (1 << 2)
#define GAP_LE_ROLE_CENTRAL (1 << 3)

#define GAP_LTK_SIZE 0x10
#define GAP_IRK_SIZE 0x10
#define GAP_CSRK_SIZE 0x10
#define GAP_SIGNATURE_SIZE 12

typedef void (*GenResPriAddrResult)(uint8_t result, const uint8_t addr[BT_ADDRESS_SIZE], void *context);

#define GAP_ADVERTISING_DATA_LENGTH_MAX 31

/// Advertising channel
#define GAP_ADVERTISING_CHANNEL_37 (1 << 0)
#define GAP_ADVERTISING_CHANNEL_38 (1 << 1)
#define GAP_ADVERTISING_CHANNEL_39 (1 << 2)

/// Advertising filter policy
#define GAP_ADVERTISING_NOT_USE_WL (0x00)
#define GAP_ADVERTISING_USE_WL_FOR_SCAN (0x01)
#define GAP_ADVERTISING_USE_WL_FOR_CONNECTION (0x02)
#define GAP_ADVERTISING_USE_WL_FOR_BOTH (0x03)

/// Intended Advertising property
#define GAP_ADVERTISING_PROPERTY_CONNECTABLE (1 << 0)
#define GAP_ADVERTISING_PROPERTY_SCANABLE (1 << 1)
#define GAP_ADVERTISING_PROPERTY_DIRECTED (1 << 2)
#define GAP_ADVERTISING_PROPERTY_HIGH_DUTY_CYCLE_DIRECTED (1 << 3)
#define GAP_ADVERTISING_PROPERTY_USE_LEGACY_PDUS (1 << 4)
#define GAP_ADVERTISING_PROPERTY_ANONYMOUS (1 << 5)
#define GAP_ADVERTISING_PROPERTY_INCLUDE_TXPOWER (1 << 6)

/// Intended Advertising data operation
#define GAP_ADVERTISING_DATA_OPERATION_INTERMEDIATE (0x00)
#define GAP_ADVERTISING_DATA_OPERATION_FIRST (0x01)
#define GAP_ADVERTISING_DATA_OPERATION_LAST (0x02)
#define GAP_ADVERTISING_DATA_OPERATION_COMPLETE (0x03)
#define GAP_ADVERTISING_DATA_OPERATION_UNCHANGED (0x04)

/// Intended Advertising PHY
#define GAP_ADVERTISEMENT_PHY_NONE (0x00)
#define GAP_ADVERTISEMENT_PHY_1M (0x01)
#define GAP_ADVERTISEMENT_PHY_2M (0x02)
#define GAP_ADVERTISEMENT_PHY_CODED (0x03)

/// fragment preference
#define GAP_CONTROLLER_MAY_FRAGMENT (0x00)
#define GAP_CONTROLLER_SHOULD_NOT_FRAGMENT (0x01)

/**
 * @brief       Extended Advertising enable parameter structure
 */
typedef struct {
    uint8_t advHandle;    /// Extended Advertising handle
    uint16_t duration;    /// Advertising duration (n * 10ms, 0 is no limit)
    uint8_t maxExAdvEvt;  /// maximum number of advertising events (0 is no limit)
} GapExAdvSet;

/**
 * @brief       Extended Advertising callback function structure
 */
typedef struct {
    void (*exAdvSetRandAddrResult)(uint8_t status, void *context);
    void (*exAdvSetParamResult)(uint8_t status, uint8_t selectTxPower, void *context);
    void (*exAdvSetDataResult)(uint8_t status, void *context);
    void (*exAdvSetScanRspDataResult)(uint8_t status, void *context);
    void (*exAdvSetEnableResult)(uint8_t status, void *context);
    void (*exAdvRemoveHandleResult)(uint8_t status, void *context);
    void (*exAdvClearHandleResult)(uint8_t status, void *context);
    void (*exAdvScanRequestReceived)(uint8_t advHandle, const BtAddr *scannerAddr, void *context);
    void (*exAdvTerminatedAdvSet)(
        uint8_t status, uint8_t advHandle, uint16_t connectionHandle, uint8_t completedNumber, void *context);
} GapExAdvCallback;

/**
 * @brief       Advertising parameter structure
 */
typedef struct {
    uint16_t advIntervalMin;  /// Minimum advertising interval (0x0020-0x4000)
    uint16_t advIntervalMax;  /// Maximum advertising interval (0x0020-0x4000)
    uint8_t advChannelMap;    /// Used channel of Advertising
    const BtAddr *peerAddr;   /// Target bluetooth address of directed advertising
    uint8_t advFilterPolicy;  /// Advertising filter policy
} GapLeAdvParam;

/**
 * @brief       Extended advertising parameter structure
 */
typedef struct {
    uint32_t advIntervalMin;          /// Minimum advertising interval (0x000020-0xFFFFFF)
    uint32_t advIntervalMax;          /// Maximum advertising interval (0x000020-0xFFFFFF)
    uint8_t advChannelMap;            /// Used channel of Advertising
    const BtAddr *peerAddr;           /// Target bluetooth address of directed advertising
    uint8_t advFilterPolicy;          /// Advertising filter policy
    uint8_t primaryAdvPhy;            /// Primary Advertising PHY
    uint8_t secondaryAdvMaxSkip;      /// Maximum number that can be skipped before the AUX_ADV_IND.
    uint8_t secondaryAdvPhy;          /// Secondary Advertising PHY
    uint8_t advSid;                   /// Value of the Advertising SID subfield in the ADI field of the PDU
    uint8_t scanRequestNotifyEnable;  /// Scan request notifications enabled (0x01 is enabled)
} GapLeExAdvParam;

/// BLE legacy advertising type
#define GAP_ADV_TYPE_CONN_UNDIR 0x00
#define GAP_ADV_TYPE_CONN_DIR_HIGH_DUTY 0x01
#define GAP_ADV_TYPE_SCAN_UNDIR 0x02
#define GAP_ADV_TYPE_NON_CONN_UNDIR 0x03
#define GAP_ADV_TYPE_CONN_DIR_LOW_DUTY 0x04

/**
 * @brief       Legacy advertising callback structure
 */
typedef struct {
    void (*advSetParamResult)(uint8_t status, void *context);
    void (*advReadTxPower)(uint8_t status, int8_t txPower, void *context);
    void (*advSetDataResult)(uint8_t status, void *context);
    void (*advSetScanRspDataResult)(uint8_t status, void *context);
    void (*advSetEnableResult)(uint8_t status, void *context);
} GapAdvCallback;

/// Scan filter policy
#define GAP_SCAN_NOT_USE_WL (0x00)
#define GAP_SCAN_USE_WL (0x01)
#define GAP_SCAN_NOT_USE_WL_AND_IDENTITY (0x02)
#define GAP_SCAN_USE_WL_AND_IDENTITY (0x03)

/// Scan PHYs
#define GAP_EX_SCAN_PHY_1M (1 << 0)
#define GAP_EX_SCAN_PHY_CODED (1 << 2)

/**
 * @brief       BLE scan internal and window parameter structure
 */
typedef struct {
    uint16_t scanInterval;  /// scan internal
    uint16_t scanWindow;    /// scan window
} GapLeScanParameter;

/**
 * @brief       BLE scan parameter structure
 */
typedef struct {
    uint8_t scanType;         /// scan type
    GapLeScanParameter param;  /// scan internal and window parameter
} GapLeScanParam;

/**
 * @brief       Advertising report structure
 */
typedef struct {
    uint8_t dataLen;  /// Advertising data len
    uint8_t *data;    /// Advertising data
    int8_t rssi;      /// RSSI (127 is information not available)
} GapAdvReportParam;

/**
 * @brief       BLE scan result callback
 */
typedef struct {
    void (*advertisingReport)(
        uint8_t advType, const BtAddr *addr, GapAdvReportParam reportParam, const BtAddr *currentAddr, void *context);
    void (*scanSetParamResult)(uint8_t status, void *context);
    void (*scanSetEnableResult)(uint8_t status, void *context);
} GapScanCallback;

/**
 * @brief       Extended advertising report structure
 */
typedef struct {
    uint8_t primaryPhy;            /// Advertiser PHY
    uint8_t secondaryPhy;          /// Secondary advertiser PHY:
    uint8_t advertisingSid;        /// Value of the Advertising SID subfield in the ADI field of the PDU
    int8_t txPower;                /// TX power (127 is information not available)
    int8_t rssi;                   /// RSSI (127 is information not available)
    uint16_t periodicAdvInterval;  /// Periodic advertising interval
    const BtAddr *directAddr;      /// Directed advertising event address
    uint8_t dataLen;               /// Advertising data len
    uint8_t *data;                 /// Advertising data
} GapExAdvReportParam;

/**
 * @brief       Directed advertising report structure
 */
typedef struct {
    const BtAddr *directAddr;  /// Directed advertising event address
    int8_t rssi;               /// RSSI (127 is information not available)
} GapDirectedAdvReportParam;

/**
 * @brief       Extended scan result structure
 */
typedef struct {
    void (*exAdvertisingReport)(
        uint8_t advType, const BtAddr *addr, GapExAdvReportParam reportParam, const BtAddr *currentAddr, void *context);
    void (*directedAdvertisingReport)(uint8_t advType, const BtAddr *addr, GapDirectedAdvReportParam reportParam,
        const BtAddr *currentAddr, void *context);
    void (*scanExSetParamResult)(uint8_t status, void *context);
    void (*scanExSetEnableResult)(uint8_t status, void *context);
    void (*scanTimeoutEvent)(void *context);
} GapExScanCallback;

/**
 * @brief       BLE link layer control callback structure
 */
typedef struct {
    void (*leConnectionParameterReq)(const BtAddr *addr, uint16_t connIntervalMin, uint16_t connIntervalMax,
        uint16_t connLatency, uint16_t timeout, void *context);
    void (*leConnectionUpdateComplete)(uint8_t status, const BtAddr *addr, uint16_t connInterval, uint16_t connLatency,
        uint16_t timeout, void *context);
    void (*GapleSetHostChannelClassificationResult)(uint8_t result, void *context);
    void (*GapleReadChannelMapResult)(uint8_t result, const BtAddr *addr, uint64_t channelMap, void *context);
} GapLeConnCallback;

/**
 * @brief       BLE connection parameter structure
 */
typedef struct {
    uint16_t connIntervalMin;  /// Minimum value for the connection interval(n * 1.25ms 0x0006-0x0c80)
    uint16_t connIntervalMax;  /// Maximum value for the connection interval(n * 1.25ms 0x0006-0x0c80)
    uint16_t connLatency;      /// Slave latency for the connection (0x0000-0x01F3)
    uint16_t timeout;          /// Supervision timeout for the LE Link (n * 10ms 0x000A-0x0C80)
    uint16_t minCeLen;         /// Minimum length of connection event (n * 0.625ms 0x0000-0xFFFF)
    uint16_t maxCeLen;         /// Maximum length of connection event (n * 0.625ms 0x0000-0xFFFF)
} GapLeConnectionParameter;

/// BLE OOB Authentication data present
#define GAP_LE_OOB_DATA_NOT_PRESENT 0x00
#define GAP_LE_OOB_DATA_PRESENT 0x01

/// BLE authentication requirements
#define GAP_LE_AUTH_REQ_BONDING (1 << 0)
#define GAP_LE_AUTH_REQ_BIT_MITM (1 << 2)
#define GAP_LE_AUTH_REQ_BIT_SC (1 << 3)
#define GAP_LE_AUTH_REQ_BIT_KEYPRESS (1 << 4)
#define GAP_LE_AUTH_REQ_BIT_CT2 (1 << 5)

/// BLE key distribution
#define GAP_LE_KEY_DIST_ENC_KEY (1 << 0)
#define GAP_LE_KEY_DIST_ID_KEY (1 << 1)
#define GAP_LE_KEY_DIST_SIGN_KEY (1 << 2)

/**
 * @brief       BLE pair feature structure
 */
typedef struct {
    uint8_t ioCapability;   /// IO Capability
    uint8_t oobDataFlag;    /// OOB Authentication data present
    uint8_t authReq;        /// Authentication requirements
    uint8_t maxEncKeySize;  /// Maximum of Long Term Key size
    uint8_t initKeyDis;     /// Initiator Key Distribution / Generation
    uint8_t respKeyDis;     /// Responder Key Distribution / Generation
} GapLePairFeature;

/**
 * @brief       BLE encryption key structure
 */
typedef struct {
    uint8_t ltk[GAP_LTK_SIZE];  /// Long Term Key
    uint64_t rand;              /// Random Number
    uint16_t ediv;              /// Encrypted Diversifier
    uint8_t keySize;            /// Encryption Key Size
} LeEncKey;

/**
 * @brief       BLE identity key structure
 */
typedef struct {
    BtAddr identityAddr;        /// Identity Address
    uint8_t irk[GAP_IRK_SIZE];  /// Identity Resolving Key
} LeIdKey;

/**
 * @brief       BLE signature key structure
 */
typedef struct {
    uint8_t csrk[GAP_CSRK_SIZE];  /// Connection Signature Resolving Key
    uint32_t counter;             /// SignCounter
} LeSignKey;

/**
 * @brief       BLE SignCounter type
 */
typedef enum {
    LOCAL_SIGN_COUNTER,
    REMOTE_SIGN_COUNTER,
} GAP_SignCounterType;

/**
 * @brief       Signing Algorithm Info structure
 */
typedef struct {
    LeSignKey *localKey;
    LeSignKey *remoteKey;
} GapSigningAlgorithmInfo;

/**
 * @brief       BLE security callback structure
 */
typedef struct {
    void (*encryptionComplete)(uint8_t status, const BtAddr *addr, void *context);
    void (*leLocalEncryptionKeyReqEvent)(const BtAddr *addr, uint64_t rand, uint16_t ediv, void *context);
    void (*leRemoteEncryptionKeyReqEvent)(const BtAddr *addr, void *context);
    void (*GapRequestSigningAlgorithmInfo)(const BtAddr *addr, void *context);
    void (*leSignCounterChangeNotification)(
        const BtAddr *addr, GAP_SignCounterType type, uint32_t counter, void *context);
} GapLeSecurityCallback;

/**
 * @brief       level of LE security mode 1
 */
typedef enum {
    LE_MODE_1_LEVEL_1,
    LE_MODE_1_LEVEL_2,
    LE_MODE_1_LEVEL_3,
    LE_MODE_1_LEVEL_4,
} GAP_LeSecMode1Level;

/**
 * @brief       level of LE security mode 2
 */
typedef enum {
    LE_MODE_2_LEVEL_1,
    LE_MODE_2_LEVEL_2,
} GAP_LeSecMode2Level;

/**
 * @brief       BLE security status of le connection
 */
typedef enum {
    GAP_LE_NO_ENCRYPTION,
    GAP_LE_UNAUTHENTICATED_ENCRYPTION,
    GAP_LE_AUTHENTICATED_ENCRYPTION,
} GAP_LeSecurityStatus;

/**
 * @brief       BLE security result callback
 * @param[in]   addr                target device address
 * @param[in]   result              request result
 * @param[in]   status              security status of le connection
 * @param[in]   context             callback context
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
typedef void (*GapLeRequestSecurityResult)(
    const BtAddr *addr, uint8_t result, GAP_LeSecurityStatus status, void *context);

typedef struct {
    LeEncKey *remoteEncKey;
    LeIdKey *remoteIdKey;
    LeSignKey *remoteSignKey;
    LeEncKey *localEncKey;
    LeSignKey *localSignKey;
} LePairedKeys;

/**
 * @brief       BLE pair callback structure
 */
typedef struct {
    void (*lePairFeatureReq)(const BtAddr *addr, bool localPair, void *context);
    void (*lePairFeatureInd)(const BtAddr *addr, GapLePairFeature remoteFrature, void *context);
    void (*lePairMethodNotify)(const BtAddr *addr, uint8_t pairMethod, void *context);
    void (*lePairKeyPressNotification)(const BtAddr *addr, uint8_t pressType, void *context);
    void (*lePairPassKeyReq)(const BtAddr *addr, void *context);
    void (*lePairPassKeyNotification)(const BtAddr *addr, uint32_t number, void *context);
    void (*lePairOobReq)(const BtAddr *addr, void *context);
    void (*lePairScOobReq)(const BtAddr *addr, void *context);
    void (*lePairScOobNotification)(const BtAddr *addr, GapOOBData *oobData, void *context);
    void (*lePairScUserConfirmReq)(const BtAddr *addr, uint32_t number, void *context);
    void (*lePairComplete)(const BtAddr *addr, uint8_t result, uint8_t keyType, void *context);
    void (*lePairKeyNotify)(const BtAddr *addr, LePairedKeys LeKeys, void *context);
} GapLePairCallback;

#define GAP_ENC_KEY_MIN_SIZE 0x07
#define GAP_ENC_KEY_MAX_SIZE 0x10

/**
 * @brief       BLE signature algorithm result
 */
typedef enum {
    GAP_SIGNATURE_OK,
    GAP_SIGNATURE_ERR_EXECUTION,
    GAP_SIGNATURE_ERR_COUNTER,
    GAP_SIGNATURE_ERR_ALGORITHM,
} GAP_SignatureResult;

/**
 * @brief       BLE signature algorithm data PDU
 */
typedef struct {
    const uint8_t *data;
    uint16_t dataLen;
} GapSignatureData;

typedef void (*GAPSignatureGenerationResult)(
    GAP_SignatureResult result, uint8_t signature[GAP_SIGNATURE_SIZE], void *context);

typedef void (*GAPSignatureConfirmationResult)(GAP_SignatureResult result, void *context);

/**
 * @brief       Set BLE Roles
 * @param[in]   role                BLE Roles
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeSetRole(uint8_t role);

/**
 * @brief       Set Static Identity Address if controller does not have a Public Device Address
 * @param[in/out]  addr             Static Identity Address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeSetStaticIdentityAddr(uint8_t addr[BT_ADDRESS_SIZE]);

/**
 * @brief       Generate a local resolvable private address
 * @param[in]   callback            Generate RPA result function
 * @param[in]   context             Generate RPA result function context
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeGenResPriAddr(GenResPriAddrResult callback, void *context);

/**
 * @brief       Read the maximum length of intended advertising data supported by the Controller
 * @param[out]  len                length of intended advertising data or scan response data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvGetMaxDataLen(uint16_t *len);

/**
 * @brief       Read the maximum number of advertising sets supported by the advertising Controller at the same time.
 * @param[out]  num                maximum number of advertising sets
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvGetMaxHandleNum(uint8_t *num);

/**
 * @brief       Register Extended advertising callback function
 * @param[in]   callback            Extended advertising callback structure
 * @param[in]   context             Extended advertising result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterExAdvCallback(const GapExAdvCallback *callback, void *context);

/**
 * @brief       Degegister Extended advertising callback function
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterExAdvCallback(void);

/**
 * @brief       Set the random device address used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   addr                random device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvSetRandAddr(uint8_t advHandle, const uint8_t addr[BT_ADDRESS_SIZE]);

/**
 * @brief       Set the advertising parameter used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   properties          intended Advertising property
 * @param[in]   txPower            advertising TX power
 * @param[in]   advExParam          advertising parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvSetParam(
    uint8_t advHandle, uint8_t properties, int8_t txPower, GapLeExAdvParam advExParam);

/**
 * @brief       Set the advertising data used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   operation           intended Advertising operation
 * @param[in]   fragmentPreference  fragment preference
 * @param[in]   advDataLength       advertising data length
 * @param[in]   advData             advertising data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvSetData(
    uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference, uint8_t advDataLength, const uint8_t *advData);

/**
 * @brief       Set the scan response data used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   operation           intended Advertising operation
 * @param[in]   fragmentPreference  fragment preference
 * @param[in]   scanResponseDataLen scan response data length
 * @param[in]   scanResponseData    scan response data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvSetScanRspData(uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference,
    uint8_t scanResponseDataLen, const uint8_t *scanResponseData);

/**
 * @brief       Set the advertising enable used by advertising.
 * @param[in]   enable              advertising enable (0x00 is disable, 0x01 is enable)
 * @param[in]   numberOfSet         number of parameter (When disabled, 0 is to disable all)
 * @param[in]   advSet              advertising enable parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvSetEnable(uint8_t enable, uint8_t numberOfSet, const GapExAdvSet *advSet);

/**
 * @brief       Remove all advertising set of advertising.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExAdvClearHandle(void);

/**
 * @brief       Register legacy advertising result callback
 * @param[in]   callback            legacy advertising callback
 * @param[in]   context             legacy advertising callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterAdvCallback(const GapAdvCallback *callback, void *context);

/**
 * @brief       Deregister legacy advertising result callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterAdvCallback(void);

/**
 * @brief       Set legacy advertising parameter
 * @param[in]   advType             legacy advertising type
 * @param[in]   advParam            legacy advertising parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeAdvSetParam(uint8_t advType, GapLeAdvParam advParam);

/**
 * @brief       Read legacy advertising TX power
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeAdvReadTxPower(void);

/**
 * @brief       Set legacy advertising data
 * @param[in]   advDataLength       legacy advertising data length
 * @param[in]   advData             legacy advertising data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeAdvSetData(uint8_t advDataLength, const uint8_t *advData);

/**
 * @brief       Set legacy advertising scan response data
 * @param[in]   advDataLength       legacy advertising scan response data length
 * @param[in]   advData             legacy advertising scan response data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeAdvSetScanRspData(uint8_t advDataLength, const uint8_t *advData);

/**
 * @brief       Set legacy advertising enable
 * @param[in]   enable              advertising enable (0x00 is disable, 0x01 is enable)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeAdvSetEnable(uint8_t enable);

/**
 * @brief       Register scan result callback
 * @param[in]   callback            scan result callback
 * @param[in]   context             scan result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterScanCallback(const GapScanCallback *callback, void *context);

/**
 * @brief       Deregister scan result callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterScanCallback(void);

/**
 * @brief       Set scan parameter
 * @param[in]   param               scan parameter
 * @param[in]   scanFilterPolity    scan filter policy
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeScanSetParam(GapLeScanParam param, uint8_t scanFilterPolity);

/**
 * @brief       Set scan enable
 * @param[in]   scanEnable          scan enable (0x00 is disable, 0x01 is enable)
 * @param[in]   filterDuplicates    filter duplicates (0x00 is disable, 0x01 is enable)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates);

/**
 * @brief       Register extended scan result callback
 * @param[in]   callback            extended scan result callback
 * @param[in]   context             extended scan result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterExScanCallback(const GapExScanCallback *callback, void *context);

/**
 * @brief       Deregister extended scan result callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterExScanCallback(void);

/**
 * @brief       Set extended scan parameter
 * @param[in]   scanFilterPolity    scan filter policy
 * @param[in]   scanPhys            scan PHYs
 * @param[in]   param               scan parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExScanSetParam(uint8_t scanFilterPolity, uint8_t scanPhys, const GapLeScanParam param[]);

/**
 * @brief       Set extended scan parameter
 * @param[in]   scanEnable          scan enable (0x00 is disable, 0x01 is enable)
 * @param[in]   filterDuplicates    filter duplicates (0x00 is disable, 0x01 is enable)
 * @param[in]   duration            (n * 10ms, 0 is scan continuously)
 * @param[in]   period              (n * 1.28s, 0 is periodic scanning disabled)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeExScanSetEnable(
    uint8_t scanEnable, uint8_t filterDuplicates, uint16_t duration, uint16_t period);

/**
 * @brief       Register link layer control callback
 * @param[in]   callback            link layer control callback
 * @param[in]   context             link layer control callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterLeConnCallback(const GapLeConnCallback *callback, void *context);

/**
 * @brief       Deregister link layer control callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterLeConnCallback(void);

/**
 * @brief       Send Connection parameter update request
 * @param[in]   addr                target device address
 * @param[in]   connParam           connection parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeConnParamUpdate(const BtAddr *addr, const GapLeConnectionParameter *connParam);

/**
 * @brief       Respond Connection parameter update request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   connParam           connection parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeConnectionParameterRsp(
    const BtAddr *addr, uint8_t accept, const GapLeConnectionParameter *connParam);

/**
 * @brief       Register BLE security callback
 * @param[in]   callback            BLE security callback
 * @param[in]   context             BLE security callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterLeSecurityCallback(const GapLeSecurityCallback *callback, void *context);

/**
 * @brief       Deregister BLE security callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterLeSecurityCallback(void);

/**
 * @brief       Respond remote encryption key of target device
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   remoteEncKey        remote encryption key
 * @param[in]   keyType             remote encryption key type
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeRemoteEncryptionKeyRsp(
    const BtAddr *addr, uint8_t accept, LeEncKey remoteEncKey, uint8_t keyType);

/**
 * @brief       Respond local encryption key of target device
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   localEncKey         local encryption key
 * @param[in]   keyType             local encryption key type
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeLocalEncryptionKeyRsp(
    const BtAddr *addr, uint8_t accept, LeEncKey localEncKey, uint8_t keyType);

/**
 * @brief       Respond signing key of target device
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   info                signing key info
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RequestSigningAlgorithmInfoRsp(const BtAddr *addr, uint8_t accept, GapSigningAlgorithmInfo info);

/**
 * @brief       Set BLE bondable mode
 * @param[in]   mode                boneable mode
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeSetBondMode(uint8_t mode);

/**
 * @brief       Set BLE security mode
 * @param[in]   mode1Level          level of LE security mode 1
 * @param[in]   mode2Level          level of LE security mode 2
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeSetSecurityMode(GAP_LeSecMode1Level mode1Level, GAP_LeSecMode2Level mode2Level);

/**
 * @brief       Get security status of le connection
 * @param[in]   addr                target device address
 * @param[out]  status              security status of le connection
 * @param[out]  encKeySize          encryption Key Size
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeGetSecurityStatus(const BtAddr *addr, GAP_LeSecurityStatus *status, uint8_t *encKeySize);

/**
 * @brief       Request security of le connection
 * @param[in]   addr                target device address
 * @param[in]   status              security status of le connection
 * @param[in]   callback            result callback
 * @param[in]   context             result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeRequestSecurity(
    const BtAddr *addr, GAP_LeSecurityStatus status, GapLeRequestSecurityResult callback, void *context);

/**
 * @brief       BLE bonding procedure
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LePair(const BtAddr *addr);

/**
 * @brief       Cancel BLE bonding procedure
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeCancelPair(const BtAddr *addr);

/**
 * @brief       Set minimum of Long Term Key size
 * @param[in]   minSize             minimum of Long Term Key size
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeSetMinEncKeySize(uint8_t minSize);

/**
 * @brief       Register BLE pair callback
 * @param[in]   callback            BLE pair callback
 * @param[in]   context             BLE pair callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_RegisterLePairCallback(const GapLePairCallback *callback, void *context);

/**
 * @brief       Deregister BLE pair callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_DeregisterLePairCallback(void);

/**
 * @brief       Respond BLE pair feature request
 * @param[in]   addr                target device address
 * @param[in]   localFrature        local BLE pair feature
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LePairFeatureRsp(const BtAddr *addr, GapLePairFeature localFrature);

/**
 * @brief       Respond BLE pair passkey request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @param[in]   number              passkey number (000000-999999)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LePairPassKeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number);

/**
 * @brief       Respond BLE legacy OOB data request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @param[in]   oobData             legacy OOB data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LePairOobRsp(const BtAddr *addr, uint8_t accept, uint8_t oobData[GAP_OOB_DATA_SIZE]);

/**
 * @brief       Respond BLE Secure connection OOB data request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @param[in]   oobDataC            Secure connection OOB confirm data
 * @param[in]   oobDataR            Secure connection OOB random data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LePairScOobRsp(const BtAddr *addr, uint8_t accept,
    const uint8_t oobDataC[GAP_OOB_DATA_CONFIRM_SIZE], const uint8_t oobDataR[GAP_OOB_DATA_RANDOM_SIZE]);

/**
 * @brief       Respond user confirmation request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LePairScUserConfirmRsp(const BtAddr *addr, uint8_t accept);

/**
 * @brief       generation a data signature
 * @param[in]   addr                target device address
 * @param[in]   dataInfo            data PDU
 * @param[in]   callback            result callback function
 * @param[in]   context             result callback function context
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeDataSignatureGenerationAsync(
    const BtAddr *addr, GapSignatureData dataInfo, GAPSignatureGenerationResult callback, void *context);

/**
 * @brief       Confirmation a data signature
 * @param[in]   addr                target device address
 * @param[in]   dataInfo            data PDU
 * @param[in]   signature           signature data
 * @param[in]   callback            result callback function
 * @param[in]   context             result callback function context
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
BTSTACK_API int GAPIF_LeDataSignatureConfirmationAsync(const BtAddr *addr, GapSignatureData dataInfo,
    const uint8_t signature[GAP_SIGNATURE_SIZE], GAPSignatureConfirmationResult callback, void *context);

#ifdef __cplusplus
}
#endif

#endif /* GAP_LE_IF_H */
