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

#ifndef GAP_H
#define GAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "btstack.h"
#include "gap_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Get local bluetooth address from HCI.
 * @param[out]  addr                Bluetooth address of bluetooth chip.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_GetLocalAddr(BtAddr *addr);

/**
 * @brief       Set local bluetooth device name. (Used for BR/EDR)
 * @param[in]   name                Bluetooth device name.
 * @param[in]   length              Length of the device name. (248 or less)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_SetLocalName(const char *name, int length);

/**
 * @brief       Set local bluetooth device class.
 * @param[in]   cod                 Bluetooth device class.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_SetClassOfDevice(uint32_t cod);

/**
 * @brief       Set local bluetooth device class.
 * @param[in]   cod                 Bluetooth device class.
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_SetExtendedInquiryResponse(const uint8_t eir[GAP_EIR_SIZE_MAX]);

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
int GAP_SetScanMode(const GapDiscoverModeInfo *discoverInfo, const GapConnectableModeInfo *connectableInfo,
    GapSetScanModeResultCallback callback, void *context);

/**
 * @brief       Set boneable mode of bluetooth BR/EDR physical transport.
 * @param[in]   bondableMode        boneable mode
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 * @see         BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part C
 *              4.3 BONDABLE MODES
 */
int GAP_SetBondableMode(uint8_t bondableMode);

/**
 * @brief       Service register security requirements to GAP
 * @param[in]   addr                outgoing attributes to remote device
 * @param[in]   serviceInfo         security requirements information
 * @param[in]   securityMode        Security attributes
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo, uint16_t securityMode);

/**
 * @brief       Service deregister security requirements to GAP
 * @param[in]   addr                outgoing attributes to remote device
 * @param[in]   serviceInfo         security requirements information
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterServiceSecurity(const BtAddr *addr, const GapServiceSecurityInfo *serviceInfo);

/**
 * @brief       Service request security requirements to GAP
 * @param[in]   addr                target device address
 * @param[in]   param               structure of security requirements information and result callback function
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RequestSecurity(const BtAddr *addr, const GapRequestSecurityParam *param);

/**
 * @brief       Register service security verify callback
 * @param[in]   callback            security verify callback
 * @param[in]   context             security verify callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterSecurityCallback(const GapSecurityCallback *callback, void *context);

/**
 * @brief       Deregister service security verify callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterSecurityCallback(void);

/**
 * @brief       Set security modes of BR/EDR physical transport
 * @param[in]   mode                security modes
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_SetSecurityMode(GAP_SecurityMode mode);

/**
 * @brief       Service authorization verify response
 * @param[in]   addr                target device address
 * @param[in]   service             service identity
 * @param[in]   accept              accept or reject
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_AuthorizeRes(const BtAddr *addr, GAP_Service service, uint8_t accept);

/**
 * @brief       Register authentication callback
 * @param[in]   callback            authentication callback structure
 * @param[in]   context             authentication verify callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterAuthenticationCallback(const GapAuthenticationCallback *callback, void *context);

/**
 * @brief       Deregister authentication callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterAuthenticationCallback(void);

/**
 * @brief       Get current pair originator
 * @param[in]   addr                pairing device address
 * @param[out]  isLocal             is local initiate
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_PairIsFromLocal(const BtAddr *addr, bool *isLocal);

/**
 * @brief       authenticate the remote device associated.
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_AuthenticationReq(const BtAddr *addr);

/**
 * @brief       cancel authenticate the remote device associated.
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_CancelAuthenticationReq(const BtAddr *addr);

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
int GAP_IOCapabilityRsp(
    const BtAddr *addr, uint8_t accept, uint8_t ioCapability, uint8_t oobDataPresent, uint8_t authReq);

/**
 * @brief       Respond user confirmation request. Reply callback GapAuthenticationCallback::userConfirmReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_UserConfirmRsp(const BtAddr *addr, uint8_t accept);

/**
 * @brief       Respond user passkey request. Reply callback GapAuthenticationCallback::userPasskeyReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   number              user input number (000000 - 999999)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_UserPasskeyRsp(const BtAddr *addr, uint8_t accept, uint32_t number);

/**
 * @brief       Respond remote OOB data request. Reply callback GapAuthenticationCallback::remoteOobReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   data                OOB data
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RemoteOobRsp(const BtAddr *addr, uint8_t accept, const GapOOBData *data);

/**
 * @brief       Respond PIN code request. Reply callback GapAuthenticationCallback::pinCodeReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   pinCode             PIN code data
 * @param[in]   pinCodeLength       PIN code data length
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_PinCodeRsp(const BtAddr *addr, uint8_t accept, const uint8_t *pinCode, uint8_t pinCodeLength);

/**
 * @brief       Respond link key request. Reply callback GapAuthenticationCallback::linkKeyReq
 * @param[in]   addr                target device address
 * @param[in]   accept              accept or reject
 * @param[in]   linkKey             link key
 * @param[in]   keyType             link key type
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_LinkKeyRsp(const BtAddr *addr, uint8_t accept, const uint8_t linkKey[GAP_LINKKEY_SIZE], uint8_t keyType);

/**
 * @brief       Get local OOB data to paired.
 * @param[out]  oobData192
 * @param[out]  oobData256
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_GetLocalExtendedOOBData(GapOOBData *oobData192, GapOOBData *oobData256);

/**
 * @brief       Register device discover callback
 * @param[in]   callback            device discover callback structure
 * @param[in]   context             device discover callback context parameter
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_RegisterDiscoveryCallback(const GapDiscoveryCallback *callback, void *context);

/**
 * @brief       Deregister device discover callback
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_DeregisterDiscoveryCallback(void);

/**
 * @brief       discover other nearby BR/EDR Controllers
 * @param[in]   mode                Inquiry mode
 * @param[in]   inquiryLength       Maximum inquiry time.(n * 1.28s)
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_Inquiry(uint8_t mode, uint8_t inquiryLength);

/**
 * @brief       Cancel discover other nearby BR/EDR Controllers
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_InquiryCancel(void);

/**
 * @brief       Get remote device name
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_GetRemoteName(const BtAddr *addr);

/**
 * @brief       Cancel get remote device name
 * @param[in]   addr                target device address
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_GetRemoteNameCancel(const BtAddr *addr);

/**
 * @brief       Set configuration of retry pairing when remote device delete linkkey.
 * @param[in]   retry               do retry
 * @return      @c BT_NO_ERROR      : The function is executed successfully.
 *              @c otherwise        : The function is not executed successfully.
 */
int GAP_SetKeyMissingRetry(bool retry);
#ifdef __cplusplus
}
#endif

#endif /* GAP_H */
