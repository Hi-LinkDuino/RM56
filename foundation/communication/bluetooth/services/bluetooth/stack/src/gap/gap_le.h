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

#ifndef GAP_LE_H
#define GAP_LE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "btstack.h"
#include "gap_le_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Set BLE Roles
 * @param[in]   role                BLE Roles
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeSetRole(uint8_t role);

/**
 * @brief       Get local bluetooth address from HCI.
 * @param[out]  addr                Bluetooth address of bluetooth chip.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_GetLocalAddr(BtAddr *addr);

/**
 * @brief       Set Static Identity Address if controller does not have a Public Device Address
 * @param[in/out]  addr             Static Identity Address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeSetStaticIdentityAddr(uint8_t addr[BT_ADDRESS_SIZE]);

/**
 * @brief       Generate a local resolvable private address
 * @param[in]   callback            Generate RPA result function
 * @param[in]   context             Generate RPA result function context
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeGenResPriAddrAsync(GenResPriAddrResult callback, void *context);

/**
 * @brief       Read the maximum length of entended advertising data supported by the Controller
 * @param[out]  len                length of entended advertising data or scan response data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvGetMaxDataLen(uint16_t *len);

/**
 * @brief       Read the maximum number of advertising sets supported by the advertising Controller at the same time.
 * @param[out]  num                maximum number of advertising sets
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvGetMaxHandleNum(uint8_t *num);

/**
 * @brief       Register Extended advertising callback function
 * @param[in]   callback            Extended advertising callback structure
 * @param[in]   context             Extended advertising result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterExAdvCallback(const GapExAdvCallback *callback, void *context);

/**
 * @brief       Degegister Extended advertising callback function
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterExAdvCallback(void);

/**
 * @brief       Set the random device address used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   addr                random device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvSetRandAddr(uint8_t advHandle, const uint8_t addr[BT_ADDRESS_SIZE]);

/**
 * @brief       Set the advertising parameter used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   properties          entended Advertising property
 * @param[in]   txPower            advertising TX power
 * @param[in]   advExParam          advertising parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvSetParam(uint8_t advHandle, uint8_t properties, int8_t txPower, GapLeExAdvParam advExParam);

/**
 * @brief       Set the advertising data used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   operation           entended Advertising operation
 * @param[in]   fragmentPreference  fragment preference
 * @param[in]   advDataLength       advertising data length
 * @param[in]   advData             advertising data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvSetData(
    uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference, uint8_t advDataLength, const uint8_t *advData);

/**
 * @brief       Set the scan response data used by advertising.
 * @param[in]   advHandle           used to identify an advertising set (0x00-0xEF)
 * @param[in]   operation           entended Advertising operation
 * @param[in]   fragmentPreference  fragment preference
 * @param[in]   scanResponseDataLen scan response data length
 * @param[in]   scanResponseData    scan response data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvSetScanRspData(uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference,
    uint8_t scanResponseDataLen, const uint8_t *scanResponseData);

/**
 * @brief       Set the advertising enable used by advertising.
 * @param[in]   enable              advertising enable (0x00 is disable, 0x01 is enable)
 * @param[in]   numberOfSet         number of parameter (When disabled, 0 is to disable all)
 * @param[in]   advSet              advertising enable parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvSetEnable(uint8_t enable, uint8_t numberOfSet, const GapExAdvSet *advSet);

/**
 * @brief       Remove all advertising set of advertising.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExAdvClearHandle(void);

/**
 * @brief       Register legacy advertising result callback
 * @param[in]   callback            legacy advertising callback
 * @param[in]   context             legacy advertising callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterAdvCallback(const GapAdvCallback *callback, void *context);

/**
 * @brief       Deregister legacy advertising result callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterAdvCallback(void);

/**
 * @brief       Set legacy advertising parameter
 * @param[in]   advType             legacy advertising type
 * @param[in]   advParam            legacy advertising parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeAdvSetParam(uint8_t advType, GapLeAdvParam advParam);

/**
 * @brief       Read legacy advertising TX power
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeAdvReadTxPower(void);

/**
 * @brief       Set legacy advertising data
 * @param[in]   advDataLength       legacy advertising data length
 * @param[in]   advData             legacy advertising data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeAdvSetData(uint8_t advDataLength, const uint8_t *advData);

/**
 * @brief       Set legacy advertising scan response data
 * @param[in]   advDataLength       legacy advertising scan response data length
 * @param[in]   advData             legacy advertising scan response data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeAdvSetScanRspData(uint8_t advDataLength, const uint8_t *advData);

/**
 * @brief       Set legacy advertising enable
 * @param[in]   enable              advertising enable (0x00 is disable, 0x01 is enable)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeAdvSetEnable(uint8_t enable);

/**
 * @brief       Register scan result callback
 * @param[in]   callback            scan result callback
 * @param[in]   context             scan result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterScanCallback(const GapScanCallback *callback, void *context);

/**
 * @brief       Deregister scan result callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterScanCallback(void);

/**
 * @brief       Set scan parameter
 * @param[in]   param               scan parameter
 * @param[in]   scanFilterPolity    scan filter policy
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeScanSetParam(GapLeScanParam param, uint8_t scanFilterPolity);

/**
 * @brief       Set scan enable
 * @param[in]   scanEnable          scan enable (0x00 is disable, 0x01 is enable)
 * @param[in]   filterDuplicates    filter duplicates (0x00 is disable, 0x01 is enable)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates);

/**
 * @brief       Register extended scan result callback
 * @param[in]   callback            extended scan result callback
 * @param[in]   context             extended scan result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterExScanCallback(const GapExScanCallback *callback, void *context);

/**
 * @brief       Deregister extended scan result callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterExScanCallback(void);

/**
 * @brief       Set extended scan parameter
 * @param[in]   scanFilterPolity    scan filter policy
 * @param[in]   scanPhys            scan PHYs
 * @param[in]   param               scan parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExScanSetParam(uint8_t scanFilterPolity, uint8_t scanPhys, const GapLeScanParam param[]);

/**
 * @brief       Set extended scan parameter
 * @param[in]   scanEnable          scan enable (0x00 is disable, 0x01 is enable)
 * @param[in]   filterDuplicates    filter duplicates (0x00 is disable, 0x01 is enable)
 * @param[in]   duration            (n * 10ms, 0 is scan continuously)
 * @param[in]   period              (n * 1.28s, 0 is periodic scanning disabled)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeExScanSetEnable(uint8_t scanEnable, uint8_t filterDuplicates, uint16_t duration, uint16_t period);

/**
 * @brief       Register link layer control callback
 * @param[in]   callback            link layer control callback
 * @param[in]   context             link layer control callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterLeConnCallback(const GapLeConnCallback *callback, void *context);

/**
 * @brief       Deregister link layer control callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterLeConnCallback(void);

/**
 * @brief       Send Connection parameter update request
 * @param[in]   addr                target device address
 * @param[in]   connParam           connection parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeConnParamUpdate(const BtAddr *addr, const GapLeConnectionParameter *connParam);

/**
 * @brief       Respond Connection parameter update request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   connParam           connection parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeConnectionParameterRsp(const BtAddr *addr, uint8_t accept, const GapLeConnectionParameter *connParam);

/**
 * @brief       specify a channel classification for data channels
 * @param[in]   channelMap          contains 37 bits (0 is bad, 1 is unknown)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeSetHostChannelClassification(uint64_t channelMap);

/**
 * @brief       Register BLE security callback
 * @param[in]   callback            BLE security callback
 * @param[in]   context             BLE security callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterLeSecurityCallback(const GapLeSecurityCallback *callback, void *context);

/**
 * @brief       Deregister BLE security callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterLeSecurityCallback(void);

/**
 * @brief       Respond remote encryption key of target device
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   encKey              remote encryption key
 * @param[in]   keyType             remote encryption key type
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeRemoteEncryptionKeyRsp(const BtAddr *addr, uint8_t accept, LeEncKey encKey, uint8_t keyType);

/**
 * @brief       Respond local encryption key of target device
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   encKey              local encryption key
 * @param[in]   keyType             local encryption key type
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeLocalEncryptionKeyRsp(const BtAddr *addr, uint8_t accept, LeEncKey encKey, uint8_t keyType);

/**
 * @brief       Respond signing key of target device
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   info                signing key info
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RequestSigningAlgorithmInfoRsp(const BtAddr *addr, uint8_t accept, GapSigningAlgorithmInfo info);

/**
 * @brief       Set BLE bondable mode
 * @param[in]   mode                boneable mode
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeSetBondMode(uint8_t mode);

/**
 * @brief       Set BLE security mode
 * @param[in]   mode1Level          level of LE security mode 1
 * @param[in]   mode2Level          level of LE security mode 2
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeSetSecurityMode(GAP_LeSecMode1Level mode1Level, GAP_LeSecMode2Level mode2Level);

/**
 * @brief       Get security status of le connection
 * @param[in]   addr                target device address
 * @param[out]  status              security status of le connection
 * @param[out]  encKeySize          encryption Key Size
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeGetSecurityStatus(const BtAddr *addr, GAP_LeSecurityStatus *status, uint8_t *encKeySize);

/**
 * @brief       Request security of le connection
 * @param[in]   addr                target device address
 * @param[in]   status              security status of le connection
 * @param[in]   callback            result callback
 * @param[in]   context             result callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeRequestSecurity(
    const BtAddr *addr, GAP_LeSecurityStatus status, GapLeRequestSecurityResult callback, void *context);

/**
 * @brief       BLE bonding procedure
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LePair(const BtAddr *addr);

/**
 * @brief       Cancel BLE bonding procedure
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeCancelPair(const BtAddr *addr);

/**
 * @brief       Set minimum of Long Term Key size
 * @param[in]   minSize             minimum of Long Term Key size
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeSetMinEncKeySize(uint8_t minSize);

/**
 * @brief       Register BLE pair callback
 * @param[in]   callback            BLE pair callback
 * @param[in]   context             BLE pair callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterLePairCallback(const GapLePairCallback *callback, void *context);

/**
 * @brief       Deregister BLE pair callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterLePairCallback(void);

/**
 * @brief       Respond BLE pair feature request
 * @param[in]   addr                target device address
 * @param[in]   localFeature        local BLE pair feature
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LePairFeatureRsp(const BtAddr *addr, GapLePairFeature localFeature);

/**
 * @brief       Respond BLE pair passkey request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @param[in]   number              passkey number (000000-999999)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LePairPassKeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number);

/**
 * @brief       Respond BLE legacy OOB data request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @param[in]   oobData             legacy OOB data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LePairOobRsp(const BtAddr *addr, uint8_t accept, uint8_t oobData[GAP_OOB_DATA_SIZE]);

/**
 * @brief       Respond BLE Secure connection OOB data request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @param[in]   oobDataC            Secure connection OOB confirm data
 * @param[in]   oobDataR            Secure connection OOB random data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LePairScOobRsp(const BtAddr *addr, uint8_t accept, const uint8_t oobDataC[GAP_OOB_DATA_CONFIRM_SIZE],
    const uint8_t oobDataR[GAP_OOB_DATA_RANDOM_SIZE]);

/**
 * @brief       Respond user confirmation request
 * @param[in]   addr                target device address
 * @param[in]   accept              accept orr reject
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LePairScUserConfirmRsp(const BtAddr *addr, uint8_t accept);

/**
 * @brief       generation a data signature
 * @param[in]   addr                target device address
 * @param[in]   dataInfo            data PDU
 * @param[in]   callback            result callback function
 * @param[in]   context             result callback function context
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LeDataSignatureGeneration(
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
int GAP_LeDataSignatureConfirmation(const BtAddr *addr, GapSignatureData dataInfo,
    const uint8_t signature[GAP_SIGNATURE_SIZE], GAPSignatureConfirmationResult callback, void *context);

#ifdef __cplusplus
}
#endif

#endif /* GAP_LE_H */