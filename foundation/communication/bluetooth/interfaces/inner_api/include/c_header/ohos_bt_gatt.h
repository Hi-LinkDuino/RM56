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
 * @brief Provides basic Bluetooth capabilities.
 *
 * This module allows you to enable and disable Bluetooth, and access basic Bluetooth capabilities.\n
 * Bluetooth uses profiles such as BT-GAP, BLE, BLE-GATT, BT-data transmission, HFP, A2DP, AVRCP, MAP, and PBAP.
 *
 * @since 6
 */

/**
 * @file ohos_bt_gatt.h
 *
 * @brief Declares basic GATT data structures and functions, such as advertising and scan functions.
 *
 * @since 6
 */


#ifndef OHOS_BT_GATT_H
#define OHOS_BT_GATT_H

#include "ohos_bt_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerates advertising filtering parameters.
 *
 * The parameters specify whether the advertiser uses a whitelist to filter scan or connection requests from scanners.
 *
 * @since 6
 */
typedef enum {
    /** Processing scan and connection requests from all devices */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY = 0x00,
    /** Processing connection requests from all devices and only the scan requests from devices in the whitelist */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_ANY = 0x01,
    /** Processing scan requests from all devices and only the connection requests from devices in the whitelist */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_ANY_CON_WLST = 0x02,
    /** Processing only the scan and connection requests from devices in the whitelist */
    OHOS_BLE_ADV_FILTER_ALLOW_SCAN_WLST_CON_WLST = 0x03,
} BleAdvFilter;

/**
 * @brief Enumerates advertisement types.
 *
 * @since 6
 */
typedef enum {
    /** Connectable and scannable undirected advertisement (default) */
    OHOS_BLE_ADV_IND = 0x00,
    /** Connectable directed advertisement with a high duty ratio */
    OHOS_BLE_ADV_DIRECT_IND_HIGH = 0x01,
    /** Scannable undirected advertisement */
    OHOS_BLE_ADV_SCAN_IND = 0x02,
    /** Unconnectable undirected advertisement */
    OHOS_BLE_ADV_NONCONN_IND = 0x03,
    /** Connectable directed advertisement with a low duty ratio */
    OHOS_BLE_ADV_DIRECT_IND_LOW  = 0x04,
} BleAdvType;

/**
 * @brief Enumerates I/O capability modes.
 *
 * @since 6
 */
typedef enum {
    /** <b>DisplayOnly</b>: display capability only */
    OHOS_BLE_IO_CAP_OUT = 0x00,
    /** <b>DisplayYesNo</b>: YES/NO input and display capabilities */
    OHOS_BLE_IO_CAP_IO,
    /**
     * <b>KeyboardOnly</b>: Input of a number from 0 to 9, the confirmation key, and YES/NO,
     * with no display capability
     */
    OHOS_BLE_IO_CAP_IN,
    /** <b>NoInputNoOutput</b>: no I/O capability */
    OHOS_BLE_IO_CAP_NONE,
    /**
     * <b>KeyboardDisplay</b>: Input of a number from 0 to 9, the confirmation key, and YES/NO,
     * with the display capability
     */
    OHOS_BLE_IO_CAP_KBDISP
} BleIoCapMode;

/**
 * @brief Enumerates authentication modes for secure connection requests.
 *
 * @since 6
 */
typedef enum {
    /** No bonding */
    OHOS_BLE_AUTH_NO_BOND = 0x00,
    /** Bonding */
    OHOS_BLE_AUTH_BOND,
    /** MITM only */
    OHOS_BLE_AUTH_REQ_MITM,
    /** Secure connection only */
    OHOS_BLE_AUTH_REQ_SC_ONLY,
    /** Secure connection and bonding */
    OHOS_BLE_AUTH_REQ_SC_BOND,
    /** Secure connection and MITM */
    OHOS_BLE_AUTH_REQ_SC_MITM,
    /** Secure connection, MITM, and bonding */
    OHOS_BLE_AUTH_REQ_SC_MITM_BOND
} BleAuthReqMode;

/**
 * @brief Enumerates BLE scan types.
 *
 * @since 6
 */
typedef enum {
    /** A passive scan with no scan request */
    OHOS_BLE_SCAN_TYPE_PASSIVE = 0x00,
    /** An active scan that may contain a scan request */
    OHOS_BLE_SCAN_TYPE_ACTIVE,
} BleScanType;

/**
 * @brief Enumerates BLE scan modes.
 *
 * @since 6
 */
typedef enum {
    /** Low power */
    OHOS_BLE_SCAN_MODE_LOW_POWER = 0x00,
    /** Balance */
    OHOS_BLE_SCAN_MODE_BALANCED = 0x01,
    /** Low latency */
    OHOS_BLE_SCAN_MODE_LOW_LATENCY = 0x02,
    /** Duty cycle 2 */
    OHOS_BLE_SCAN_MODE_OP_P2_60_3000 = 0x03,
    /** Duty cycle 10 */
    OHOS_BLE_SCAN_MODE_OP_P10_60_600 = 0x04,
    /** Duty cycle 25 */
    OHOS_BLE_SCAN_MODE_OP_P25_60_240 = 0x05,
    /** Duty cycle 100 */
    OHOS_BLE_SCAN_MODE_OP_P100_1000_1000 = 0x06
} BleScanMode;

/**
 * @brief Enumerates policies for filtering advertisements in a BLE scan.
 *
 * @since 6
 */
typedef enum {
    /**
     * Accepting all advertisements except the directed advertisements
     * that are not sent to the current device (default)
     */
    OHOS_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL = 0x00,
    /**
     * Accepting advertisements from devices in the whitelist and ignoring the directed advertisements
     * that are not sent to the current device */
    OHOS_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST,
    /**
     * Accepting all undirected advertisements, directed advertisements sent by advertisers with
     * resolvable private addresses, and all the directed advertisements sent to the current device
     */
    OHOS_BLE_SCAN_FILTER_POLICY_ACCEPT_ALL_AND_RPA,
    /**
     * Accepting all undirected advertisements from the devices in the whitelist,
     * directed advertisements sent by advertisers with resolvable private addresses,
     * and all the directed advertisements sent to the current device
     */
    OHOS_BLE_SCAN_FILTER_POLICY_ONLY_WHITE_LIST_AND_RPA
} BleScanFilterPolicy;

/**
 * @brief Enumerates advertisement types in the BLE scan result.
 *
 * @since 6
 */
typedef enum {
    /** Extended, unconnectable, unscannable, and undirected advertisement */
    OHOS_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE = 0x00,
    /** Extended, unconnectable, unscannable, and directed advertisement */
    OHOS_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED = 0x04,
    /** Extended, connectable, and undirected advertisement */
    OHOS_BLE_EVT_CONNECTABLE = 0x01,
    /** Extended, connectable, and directed advertisement */
    OHOS_BLE_EVT_CONNECTABLE_DIRECTED = 0x05,
    /** Extended, scannable, and undirected advertisement */
    OHOS_BLE_EVT_SCANNABLE = 0x02,
    /** Extended, scannable, and directed advertisement */
    OHOS_BLE_EVT_SCANNABLE_DIRECTED = 0x06,

    /** Legacy, unconnectable, and undirected advertisement */
    OHOS_BLE_EVT_LEGACY_NON_CONNECTABLE = 0x10,
    /** Legacy, scannable, and undirected advertisement */
    OHOS_BLE_EVT_LEGACY_SCANNABLE = 0x12,
    /** Legacy, connectable, scannable, and undirected advertisement */
    OHOS_BLE_EVT_LEGACY_CONNECTABLE = 0x13,
    /** Legacy, connectable, and directed advertisement */
    OHOS_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED = 0x15,
    /** Legacy scan response corresponding to <b>ADV_SCAN_IND</b> */
    OHOS_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN = 0x1A,
    /** Legacy scan response corresponding to <b>ADV_IND</b> */
    OHOS_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV = 0x1B
} BleScanResultEvtType;

/**
 * @brief Enumerates data integrity types for a BLE scan result.
 *
 * @since 6
 */
typedef enum {
    /** Complete data or the last segment */
    OHOS_BLE_DATA_COMPLETE = 0x00,
    /** Incomplete data, with more data to come */
    OHOS_BLE_DATA_INCOMPLETE_MORE_TO_COME = 0x01,
    /** Incomplete truncated data, with no more data to come */
    OHOS_BLE_DATA_INCOMPLETE_TRUNCATED = 0x02,
} BleScanResultDataStatus;

/**
 * @brief Enumerates address types for a BLE scan result.
 *
 * @since 6
 */
typedef enum {
    /** Public device address */
    OHOS_BLE_PUBLIC_DEVICE_ADDRESS = 0x00,
    /** Random device address */
    OHOS_BLE_RANDOM_DEVICE_ADDRESS = 0x01,
    /** Public identity address */
    OHOS_BLE_PUBLIC_IDENTITY_ADDRESS = 0x02,
    /** Random (static) identity address */
    OHOS_BLE_RANDOM_STATIC_IDENTITY_ADDRESS = 0x03,
    /** Unresolvable random device address */
    OHOS_BLE_UNRESOLVABLE_RANDOM_DEVICE_ADDRESS = 0xFE,
    /** No address (anonymous advertisement) */
    OHOS_BLE_NO_ADDRESS = 0xFF,
} BleScanResultAddrType;

/**
 * @brief Enumerates PHY types for a BLE scan result.
 *
 * @since 6
 */
typedef enum {
    /** No advertisement packet */
    OHOS_BLE_SCAN_PHY_NO_PACKET = 0x00,
    /** 1M PHY */
    OHOS_BLE_SCAN_PHY_1M = 0x01,
    /** 2M PHY */
    OHOS_BLE_SCAN_PHY_2M = 0x02,
    /** Coded PHY */
    OHOS_BLE_SCAN_PHY_CODED = 0x03
} BleScanResultPhyType;

/**
 * @brief Defines BLE advertising parameters.
 *
 * @since 6
 */
typedef struct {
    /**
     * Minimum advertising interval.
     * It is calculated as follows: [N * 0.625 ms], where N indicates the value of this field.
     */
    int minInterval;
    /**
     * Maximum advertising interval.
     * It is calculated as follows: [N * 0.625 ms], where N indicates the value of this field.
     */
    int maxInterval;
    /** Advertising type */
    BleAdvType advType;
    /** Local address type */
    unsigned char ownAddrType;
    /** Peer address type */
    unsigned char peerAddrType;
    /** Peer address */
    BdAddr peerAddr;
    /**
     * Advertising channel to be used. For example, <b>0x01</b> indicates that channel 37 is to be used,
     * and <b>0x07</b> indicates that channels 37, 38, and 39 are used.
     */
    int channelMap;
    /** Advertisement filtering policy based on a whitelist */
    BleAdvFilter advFilterPolicy;
    /** Transmit power (dBm) */
    int txPower;
    /** Advertising duration. It is calculated as follows: [N * 10 ms], where N indicates the value of this field. */
    int duration;
} BleAdvParams;

/**
 * @brief Defines BLE scan parameters.
 *
 * @since 6
 */
typedef struct {
    /** Scan interval. It is calculated as follows: [N * 0.625 ms], where N indicates the value of this field. */
    unsigned short scanInterval;
    /** Scan window. It is calculated as follows: [N * 0.625 ms], where N indicates the value of this field. */
    unsigned short scanWindow;
    /** Scan type, as enumerated in {@link BleScanType} */
    unsigned char scanType;
    /**
     * PHY on which the advertising packets are received.
     * <b>bit0</b> indicates 1M PHY and <b>bit2</b> indicates LE Coded PHY.
     * <b>bit0</b> and <b>bit2</b> can both be set to <b>1</b>. All other bits are reserved for future use.
     */
    unsigned char scanPhy;
    /** Policy for filtering the scan result, as enumerated in {@link BleScanFilterPolicy} */
    unsigned char scanFilterPolicy;
} BleScanParams;

/**
 * @brief Defines BLE scan native filter.
 *
 * @since 6
 */
typedef struct {
    /** Handling advertisments sent by advertisers with specific address */
    char *address;
    /** Handling advertisments sent by advertisers with specific deviceName */
    char *deviceName;
    /** The length of the service uuid */
    unsigned int serviceUuidLength;
    /** Handling advertisments sent by advertisers with specific service uuid */
    unsigned char *serviceUuid;
    /** Handling advertisments sent by advertisers with specific service uuid mask */
    unsigned char *serviceUuidMask;
    /** The length of the service data */
    unsigned int serviceDataLength;
    /** Handling advertisments sent by advertisers with specific serviceData */
    unsigned char *serviceData;
    /** Handling advertisments sent by advertisers with specific serviceDataMask */
    unsigned char *serviceDataMask;
    /** The length of the manufacture data */
    unsigned int manufactureDataLength;
    /** Handling advertisments sent by advertisers with specific manufactureData */
    unsigned char *manufactureData;
    /** Handling advertisments sent by advertisers with specific manufactureDataMask */
    unsigned char *manufactureDataMask;
    /** Handling advertisments sent by advertisers with specific manufactureId */
    unsigned short manufactureId;
} BleScanNativeFilter;

/**
 * @brief Defines BLE scan configurations.
 *
 * @since 6
 */
typedef struct {
    /** Repport delay time */
    long reportDelayMillis;
    /** Scan mode */
    int scanMode;
    /** Legacy */
    bool legacy;
    /** Phy */
    int phy;
} BleScanConfigs;

/**
 * @brief Defines raw data for the BLE advertising and scan response.
 *
 * This structure is available for system applications only.
 *
 * @since 6
 */
typedef struct {
    /** Advertising data */
    unsigned char *advData;
    /** Advertising data length */
    unsigned int advDataLen;
    /** Scan response data */
    unsigned char *rspData;
    /** Scan response data length */
    unsigned int rspDataLen;
} StartAdvRawData;

/**
 * @brief Defines the BLE scan result data.
 *
 * @since 6
 */
typedef struct {
    /** Advertisement type, as enumerated in {@link BleScanResultEvtType} */
    unsigned char eventType;
    /** Data status, as enumerated in {@link BleScanResultDataStatus} */
    unsigned char dataStatus;
    /** Address type, as enumerated in {@link BleScanResultAddrType}. Value <b>0xFE</b> is unavailable. */
    unsigned char addrType;
    /** Address */
    BdAddr addr;
    /**
     * PHY type of a primary advertisement, as enumerated in {@link BleScanResultPhyType}.
     * The value can only be <b>0x01</b> or <b>0x03</b>.
     */
    unsigned char primaryPhy;
    /**
     * PHY type of a secondary advertisement.
     * The value can be any of the enumerations of {@link BleScanResultPhyType}.
     */
    unsigned char secondaryPhy;
    /**
     * Advertising SID in the ADI field of the PDU or of the original scannable advertisement (for scan responses).
     * Value <b>0xFF</b> indicates that no ADI field is available.
     */
    unsigned char advSid;
    /**
     * Transmit power. The value ranges from -127 to +20 dBm.
     * Value <b>0x7F</b> indicates that this field is invalid.
     */
    char txPower;
    /** RSSI. The value ranges from -127 to +20 dBm. Value <b>0x7F</b> indicates that this field is invalid. */
    char rssi;
    /**
     * Periodic advertising interval.
     * It is calculated as follows: [N * 1.25 ms], where N indicates the value of this field.
     * Value <b>0x00</b> indicates that this field is invalid.
     */
    unsigned short periodicAdvInterval;
    /**
     * Address type for directed advertising, as enumerated in {@link BleScanResultAddrType}.
     * Value <b>0xFF</b> is unavailable.
     */
    unsigned char directAddrType;
    /** Address for directed advertising */
    BdAddr directAddr;
    /** Advertising data length */
    unsigned char advLen;
    /** Advertising data */
    unsigned char *advData;
} BtScanResultData;

/**
 * @brief Called when advertising is enabled. For details, see {@link BleStartAdv}.
 *
 * @since 6
 */
typedef void (*AdvEnableCallback)(int advId, int status);

/**
 * @brief Called when advertising is disabled. For details, see {@link BleStopAdv}.
 *
 * @since 6
 */
typedef void (*AdvDisableCallback)(int advId, int status);

/**
 * @brief Called when advertising data is set. For details, see {@link BleSetAdvData}.
 *
 * @since 6
 */
typedef void (*AdvDataCallback)(int advId, int status);

/**
 * @brief Called when advertising parameters are updated. For details, see {@link BleUpdateAdv}
 *
 * @since 6
 */
typedef void (*AdvUpdateCallback)(int advId, int status);

/**
 * @brief Called when a secure access request is received.
 *
 * {@link BleGattSecurityRsp} is used to grant the secure access permission.
 *
 * @since 6
 */
typedef void (*SecurityRespondCallback)(const BdAddr *bdAddr);

/**
 * @brief Called when the scan result is received.
 *
 * @since 6
 */
typedef void (*ScanResultCallback)(BtScanResultData *scanResultdata);

/**
 * @brief Called when scan parameters are set.
 *
 * @since 6
 */
typedef void (*ScanParameterSetCompletedCallback)(int clientId, int status);

/**
 * @brief Defines GATT callbacks.
 *
 * @since 6
 */
typedef struct {
    /** Called when advertising is enabled. */
    AdvEnableCallback advEnableCb;
    /** Called when advertising is disabled. */
    AdvDisableCallback advDisableCb;
    /** Called when advertising data is set. */
    AdvDataCallback advDataCb;
    /** Called when advertising parameters are updated. */
    AdvUpdateCallback advUpdateCb;
    /** Called when a secure access request is received. */
    SecurityRespondCallback securityRespondCb;
    /** Called when the scan result is received. */
    ScanResultCallback scanResultCb;
    /** Called when scan parameters are set. */
    ScanParameterSetCompletedCallback scanParamSetCb;
} BtGattCallbacks;

/**
 * @brief Initializes the Bluetooth protocol stack.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth protocol stack is initialized;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int InitBtStack(void);

/**
 * @brief Enables the Bluetooth protocol stack.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth protocol stack is enabled;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int EnableBtStack(void);

/**
 * @brief Disables the Bluetooth protocol stack.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth protocol stack is disabled;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int DisableBtStack(void);

/**
 * @brief Sets the Bluetooth device name.
 *
 * @param name Indicates the pointer to the name to set.
 * @param len Indicates the length of the name to set.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the Bluetooth device name is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int SetDeviceName(const char *name, unsigned int len);

/**
 * @brief Sets advertising data.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @param data Indicates the pointer to the advertising data. For details, see {@link StartAdvRawData}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising data is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetAdvData(int advId, const StartAdvRawData data);

/**
 * @brief Starts advertising.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @param param Indicates the pointer to the advertising parameters. For details, see {@link BleAdvParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartAdv(int advId, const BleAdvParams *param);

/**
 * @brief Stops advertising.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising is stopped;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStopAdv(int advId);

/**
 * @brief Updates advertising parameters.
 *
 * @param advId Indicates the advertisement ID, which is allocated by the upper layer of the advertiser.
 * @param param Indicates the pointer to the advertising parameters. For details, see {@link BleAdvParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if advertising parameters are updated;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleUpdateAdv(int advId, const BleAdvParams *param);

/**
 * @brief Sets the secure I/O capability mode.
 *
 * @param mode Indicates the capability mode to set. For details, see {@link BleIoCapMode}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the capability mode is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetSecurityIoCap(BleIoCapMode mode);

/**
 * @brief Sets the authentication mode for secure connection requests.
 *
 * @param mode Indicates the authentication mode to set. For details, see {@link BleAuthReqMode}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the authentication mode is set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetSecurityAuthReq(BleAuthReqMode mode);

/**
 * @brief Responds to a secure connection request.
 *
 * @param bdAddr Indicates the address of the device that sends the request.
 * @param accept Specifies whether to accept the request. The value <b>true</b> means to accept the request,
 * and <b>false</b> means to reject the request.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the request is responded to;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattSecurityRsp(BdAddr bdAddr, bool accept);

/**
 * @brief Obtains the device MAC address.
 *
 * @param mac Indicates the pointer to the device MAC address.
 * @param len Indicates the length of the device MAC address.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the device MAC address is obtained;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int ReadBtMacAddr(unsigned char *mac, unsigned int len);

/**
 * @brief Sets scan parameters.
 *
 * @param clientId Indicates the client ID, which is obtained during client registration.
 * @param param Indicates the pointer to the scan parameters. For details, see {@link BleScanParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan parameters are set;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleSetScanParameters(int clientId, BleScanParams *param);

/**
 * @brief Starts a scan.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartScan(void);

/**
 * @brief Stops a scan.
 *
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan is stopped;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStopScan(void);

/**
 * @brief Registers GATT callbacks.
 *
 * @param func Indicates the pointer to the callbacks to register. For details, see {@link BtGattCallbacks}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the GATT callbacks are registered;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleGattRegisterCallbacks(BtGattCallbacks *func);

/**
 * @brief Sets advertising data and parameters and starts advertising.
 *
 * This function is available for system applications only. \n
 *
 * @param advId Indicates the pointer to the advertisement ID.
 * @param rawData Indicates the advertising data. For details, see {@link StartAdvRawData}.
 * @param advParam Indicates the advertising parameters. For details, see {@link BleAdvParams}.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the operation is successful;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartAdvEx(int *advId, const StartAdvRawData rawData, BleAdvParams advParam);

/**
 * @brief Starts a scan with BleScanConfigs.
 * If don't need ble scan filter, set BleScanNativeFilter to NULL or filterSize to zero.
 * If one of the ble scan filtering rules is not required, set it to NULL.
 * For example, set the address to NULL when you don't need it.
 * Don't support only using manufactureId as filter conditions, need to use it with manufactureData.
 * The manufactureId need to be set a related number when you need a filtering condition of manufactureData.
 *
 * @param configs Indicates the pointer to the scan filter. For details, see {@link BleScanConfigs}.
 * @param filter Indicates the pointer to the scan filter. For details, see {@link BleScanNativeFilter}.
 * @param filterSize Indicates the number of the scan filter.
 * @return Returns {@link OHOS_BT_STATUS_SUCCESS} if the scan is started;
 * returns an error code defined in {@link BtStatus} otherwise.
 * @since 6
 */
int BleStartScanEx(BleScanConfigs *configs, BleScanNativeFilter *filter, unsigned int filterSize);

#ifdef __cplusplus
}
#endif
#endif
/** @} */