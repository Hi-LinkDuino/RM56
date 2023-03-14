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

#ifndef GAP_INTERNAL_H
#define GAP_INTERNAL_H

#include "gap_def.h"

#include "btm.h"
#include "hci/hci.h"
#include "hci/hci_error.h"
#include "l2cap_le_if.h"
#include "smp/smp.h"

#ifdef __cplusplus
extern "C" {
#endif

void GapChangeHCIAddr(BtAddr *addr, const HciBdAddr *hciAddr, uint8_t addrType);
bool GapIsEmptyAddr(const uint8_t *addr);
bool GapAddrCompare(const BtAddr *addr1, const BtAddr *addr2);
bool GapCompareChannelID(GAP_SecMultiplexingProtocol protocolID, GapSecChannel channelID1, GapSecChannel channelID2);

void GapRegisterHciEventCallbacks(void);
void GapDeregisterHciEventCallbacks(void);
void GapRegisterBtmAclCallbacks(void);
void GapDeregisterBtmAclCallbacks(void);
void GapIsRemoteDeviceSupportHostSecureSimplePairingAsync(const BtAddr *addr);

void GapRegisterSmCallbacks(void);
void GapDeregisterSmCallbacks(void);

void GapRegisterL2capCallbacks(void);
void GapDeregisterL2capCallbacks(void);
void GapLeConnectionParameterUpdateReq(uint16_t aclHandle, const L2capLeConnectionParameter *param);
void GapLeConnectionParameterUpdateRsp(uint16_t aclHandle, uint8_t id, uint16_t result);

#ifdef GAP_BREDR_SUPPORT

bool GapIsBredrEnable(void);
ScanModeBlock *GapGetScanModeBlock(void);
InquiryBlock *GapGetInquiryBlock(void);
RemoteNameBlock *GapGetRemoteNameBlock(void);
GAP_SecurityMode *GapGetSecurityMode(void);
ProfileSecurityBlock *GapGetProfileSecurityBlock(void);
ConnectionInfoBlock *GapGetConnectionInfoBlock(void);
EncryptionBlock *GapGetEncryptionBlock(void);
bool GapIsBondMode(void);
bool GapFindConnectionDeviceByAddr(void *nodeData, void *param);
bool GapFindConnectionDeviceByHandle(void *nodeData, void *param);
bool GapFindCmpListData(void *nodeData, void *param);
void GapStopUseAclConnection(void *dev);
void GapStartUseAclConnection(DeviceInfo *device, uint64_t timeMs);
int GapReadNewLocalOOBData(void);
bool GapIsKeyMissingRetry(void);

void GapAclConnectionComplete(const BtmAclConnectCompleteParam *param, void *context);
void GapAclDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);
void GapWriteScanEnableComplete(const HciWriteScanEnableReturnParam *param);
void GapWriteInquiryScanActivityComplete(const HciWriteInquiryScanActivityReturnParam *param);
void GapWriteInquiryScanTypeComplete(const HciWriteInquiryScanTypeReturnParam *param);
void GapWriteCurrentIACLAPComplete(const HciWriteCurrentIacLapReturnParam *param);
void GapWritePageScanActivityComplete(const HciWritePageScanActivityReturnParam *param);
void GapWritePageScanTypeComplete(const HciWritePageScanTypeReturnParam *param);
void GapWriteClassOfDeviceComplete(const HciWriteClassofDeviceReturnParam *param);
void GapSetExtendedInquiryResponseComplete(const HciWriteExtendedInquiryResponseReturnParam *param);

void GapInquiryCancelComplete(const HciInquiryCancelReturnParam *param);
void GapGetRemoteNameCancelComplete(const HciRemoteNameRequestCancelReturnParam *param);
void GapOnInquiryComplete(const HciInquiryCompleteEventParam *eventParam);
void GapOnInquiryResult(const HciInquiryResultEventParam *eventParam);
void GapOnInquiryResultRssi(const HciInquiryResultWithRssiEventParam *eventParam);
void GapOnEntendedInquiryResult(const HciExtendedInquiryResultEventParam *eventParam);
void GapOnGetRemoteNameComplete(const HciRemoteNameRequestCompleteEventParam *eventParam);

void GapAuthenticationClearInfo(RequestSecInfo *reqInfo);
void GapAuthenticationRetry(DeviceInfo *deviceInfo, RequestSecInfo *reqInfo, uint8_t hciStatus);
void GapRequestSecurityProcess(void);
void GapUpdateSecurityRequest(DeviceInfo *devInfo, enum DeviceSecurityEvent event, uint8_t hciStatus);
void GapRemoteDeviceSupportHostSecureSimplePairingCallback(const BtAddr *addr, bool support);
void GapDoAuthenticationCallback(const void *req);
void GapDoSecurityCallback(void *req);

void GapLinkKeyRequestReplyComplete(const HciLinkKeyRequestReplyReturnParam *param);
void GapLinkKeyRequestNegativeReplyComplete(const HciLinkKeyRequestNegativeReplyReturnParam *param);
void GapPINCodeRequestReplyComplete(const HciPinCodeRequestReplyReturnParam *param);
void GapPINCodeRequestNegativeReplyComplete(const HciPinCodeRequestNegativeReplyReturnParam *param);
void GapIOCapabilityRequestReplyComplete(const HciIOCapabilityRequestReplyReturnParam *param);
void GapIOCapabilityRequestNegativeReplyComplete(const HciIoCapabilityRequestNegativeReplyReturnParam *param);
void GapOnIOCapabilityResponseEvent(const HciIoCapabilityResponseEventParam *eventParam);
void GapUserConfirmationRequestReplyComplete(const HciUserConfirmationRequestReplyReturnParam *param);
void GapUserConfirmationRequestNegativeReplyComplete(const HciUserConfirmationRequestNegativeReplyReturnParam *param);
void GapUserPasskeyRequestReplyComplete(const HciUserPasskeyRequestReplyReturnParam *param);
void GapUserPasskeyRequestNegativeReplyComplete(const HciUserPasskeyRequestNegativeReplyReturnParam *param);
void GapRemoteOOBDataRequestReplyComplete(const HciRemoteOobDataRequestReplyReturnParam *param);
void GapRemoteOOBExtendedDataRequestReplyComplete(const HciRemoteOobExtendedDataRequestReplyReturnParam *param);
void GapRemoteOOBDataRequestNegativeReplyComplete(const HciRemoteOobDataRequestNegativeReplyReturnParam *param);
void GapReadLocalOobDataComplete(const HciReadLocalOOBDataReturnParam *param);
void GapOnSimplePairingComplete(const HciSimplePairingCompleteEventParam *eventParam);
void GapReadLocalOobExtendedDataComplete(const HciReadLocalOobExtendedDataReturnParam *param);
void GapReadLocalOobDataComplete(const HciReadLocalOOBDataReturnParam *param);
void GapOnAuthenticationComplete(const HciAuthenticationCompleteEventParam *eventParam);
void GapOnEncryptionChangeEvent(const HciEncryptionChangeEventParam *eventParam);
void GapOnPINCodeRequestEvent(const HciPinCodeRequestEventParam *eventParam);
void GapOnLinkKeyRequestEvent(const HciLinkKeyRequestEventParam *eventParam);
void GapOnLinkKeyNotificationEvent(const HciLinkKeyNotificationEventParam *eventParam);
void GapOnEncryptionKeyRefreshComplete(const HciEncryptionKeyRefreshCompleteEventParam *eventParam);
void GapOnIOCapabilityRequestEvent(const HciIoCapabilityRequestEventParam *eventParam);
void GapOnUserConfirmationRequestEvent(const HciUserConfirmationRequestEventParam *eventParam);
void GapOnUserPasskeyRequestEvent(const HciUserPasskeyRequestEventParam *eventParam);
void GapOnRemoteOOBDataRequestEvent(const HciRemoteOobDataRequestEventParam *eventParam);
void GapRemoteOOBDataRequestReplyComplete(const HciRemoteOobDataRequestReplyReturnParam *param);
void GapRemoteOOBExtendedDataRequestReplyComplete(const HciRemoteOobExtendedDataRequestReplyReturnParam *param);
void GapOnUserPasskeyNotificationEvent(const HciUserPasskeyNotificationEventParam *eventParam);
void GapOnUserPasskeyRequestEvent(const HciUserPasskeyRequestEventParam *eventParam);
#endif

#ifdef GAP_LE_SUPPORT

bool GapIsLeEnable(void);
bool GapLeRolesCheck(uint8_t role);
void GapFreeLeDeviceInfo(void *data);
void GapFreeLeSignatureRequest(void *data);
void GapFreeReportRPAResolveInfo(void *data);
LeBondBlock *GapGetLeBondBlock(void);
LeConnectionInfoBlock *GapGetLeConnectionInfoBlock(void);
LeSignatureBlock *GapGetLeSignatureBlock(void);
LeRandomAddressBlock *GapGetLeRandomAddressBlock(void);
LeExAdvBlock *GapGetLeExAdvBlock(void);
bool GapIsLeBondableMode(void);
LeLocalInfo *GapGetLeLocalInfo(void);
bool GapAddrIsResolvablePrivateAddress(const BtAddr *addr);
bool GapAddrIsStaticAddress(const BtAddr *addr);
bool GapAddrIsPublicAddress(const BtAddr *addr);
bool GapAddrIsIdentityAddress(const BtAddr *addr);
bool GapFindLeConnectionDeviceByAddr(void *nodeData, void *param);
bool GapFindLeConnectionDeviceByHandle(void *nodeData, void *param);

void GapLeReadMaximumAdvertisingDataLengthComplete(const HciLeReadMaximumAdvertisingDataLengthReturnParam *param);
void GapLeReadNumberofSupportedAdvertisingSetsComplete(
    const HciLeReadNumberofSupportedAdvertisingSetsReturnParam *param);
void GapLeSetAdvertisingSetRandomAddressComplete(const HciLeSetAdvertisingSetRandomAddressReturnParam *param);
void GapLeSetExtendedAdvertisingParametersComplete(const HciLeSetExtendedAdvertisingParametersReturnParam *param);
void GapLeSetExtendedAdvertisingDataComplete(const HciLeSetExtendedAdvertisingDataReturnParam *param);
void GapLeSetExtendedScanResponseDataComplete(const HciLeSetExtendedScanResponseDataReturnParam *param);
void GapLeSetExtendedAdvertisingEnableComplete(const HciLeSetExtendedAdvertisingEnableReturnParam *param);
void GapOnLeScanRequestReceivedEvent(const HciLeScanRequestReceivedEventParam *eventParam);
void GapLeRemoveAdvertisingSetComplete(const HciLeRemoveAdvertisingSetReturnParam *param);
void GapLeClearAdvertisingSetsComplete(const HciLeClearAdvertisingSetsReturnParam *param);
void GapOnLeAdvertisingSetTerminated(const HciLeAdvertisingSetTerminatedEventParam *eventParam);
void GapLeAdvSetParamComplete(const HciLeSetAdvertisingParametersReturnParam *param);
void GapLeAdvReadTxPowerComplete(const HciLeReadAdvertisingChannelTxPowerReturnParam *param);
void GapLeAdvSetDataComplete(const HciLeSetAdvertisingDataReturnParam *param);
void GapLeAdvSetScanRspDataComplete(const HciLeSetScanResponseDataReturnParam *param);
void GapLeAdvSetEnableComplete(const HciLeSetAdvertisingEnableReturnParam *param);

void GapLeScanSetParamComplete(const HciLeSetExtendedScanParametersReturnParam *param);
void GapOnLeAdvertisingReportEvent(const HciLeAdvertisingReportEventParam *eventParam);
void GapLeScanSetEnableComplete(const HciLeSetScanEnableReturnParam *param);
void GapOnLeExtendedAdvertisingReportEvent(const HciLeExtendedAdvertisingReportEventParam *eventParam);
void GapOnLeDirectedAdvertisingReport(const HciLeDirectedAdvertisingReportEventParam *eventParam);
void GapOnLeScanTimeoutEvent(const void *eventParam);
void GapLeSetExtendedScanParametersComplete(const HciLeSetExtendedScanParametersReturnParam *param);
void GapLeSetExtendedScanEnableComplete(const HciLeSetExtendedScanEnableReturnParam *param);
void GapGenerateRPAResult(uint8_t status, const uint8_t *addr);
void GapResolveRPAResult(uint8_t status, bool result, const uint8_t *addr, const uint8_t *irk);

int GapLeRequestSecurityProcess(LeDeviceInfo *deviceInfo);
void GapLeDoPair(const void *addr);
void GapLeAuthenticationRequest(uint16_t handle, uint8_t pairMethod, const uint8_t *displayValue);
void GapLePairResult(uint16_t handle, uint8_t status, const SMP_PairResult *result);
void GapLeRemotePairRequest(uint16_t handle, const SMP_PairParam *param);
void GapLeRemotePairResponse(uint16_t handle, const SMP_PairParam *param);
void GapLeRemoteSecurityRequest(uint16_t handle, uint8_t authReq);
void GapLeLongTermKeyRequest(uint16_t handle, const uint8_t *random, uint16_t ediv);
void GapLeEncryptionComplete(uint16_t handle, uint8_t status);
void GapLeGenerateSignatureResult(uint8_t status, const uint8_t *sign);
int GapRequestSigningAlgorithmInfo(const BtAddr *addr);
void GapClearPairingStatus(const BtAddr *addr);
void GapDoPairResultCallback(const BtAddr *addr, uint8_t status);

void GapWriteAuthenticatedPayloadTimeoutComplete(const HciWriteAuthenticatedPayloadTimeoutReturnParam *param);
void GapOnAuthenticatedPayloadTimeoutExpiredEvent(const HciAuthenticatedPayloadTimeoutExpiredEventParam *eventParam);
void GapLeSetHostChannelClassificationComplete(const HciLeSetHostChannelClassificationReturnParam *param);
void GapLeReadChannelMapComplete(const HciLeReadChannelMapReturnParam *param);
void GapLeRemoteConnectionParameterRequestReplyComplete(
    const HciLeRemoteConnectionParameterRequestReplyReturnParam *param);
void GapLeRemoteConnectionParameterRequestNegativeReplyComplete(
    const HciLeRemoteConnectionParameterRequestNegativeReplyReturnParam *param);
void GapOnLeConnectionUpdateCompleteEvent(const HciLeConnectionUpdateCompleteEventParam *eventParam);
void GapOnLeRemoteConnectionParameterRequestEvent(const HciLeRemoteConnectionParameterRequestEventParam *eventParam);
bool GapLeDeviceNeedBond(const LeDeviceInfo *deviceInfo);
void GapLeConnectionComplete(
    uint8_t status, uint16_t connectionHandle, const BtAddr *addr, uint8_t role, void *context);
void GapLeDisconnectionComplete(uint8_t status, uint16_t connectionHandle, uint8_t reason, void *context);

void GapReceiveL2capParameterUpdateReq(
    uint16_t aclHandle, uint8_t id, const L2capLeConnectionParameter *param, void *ctx);
void GapReceiveL2capParameterUpdateRsp(uint16_t aclHandle, uint16_t result, void *ctx);

#endif

#ifdef __cplusplus
}
#endif

#endif