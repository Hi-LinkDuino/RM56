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

#include "hci/hci.h"

#include <securec.h>

#include "btstack.h"
#include "platform/include/allocator.h"

#include "hci_cmd.h"

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.1 Inquiry Command
int HCI_Inquiry(const HciInquiryeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_INQUIRY, (void *)param, sizeof(HciInquiryeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.2 Inquiry Cancel Command
int HCI_InquiryCancel()
{
    HciCmd *cmd = HciAllocCmd(HCI_INQUIRY_CANCEL, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.5 Create Connection Command
int HCI_CreateConnection(const HciCreateConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_CREATE_CONNECTION, (void *)param, sizeof(HciCreateConnectionParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.6 Disconnect Command
int HCI_Disconnect(const HciDisconnectParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_DISCONNECT, (void *)param, sizeof(HciDisconnectParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.7 Create Connection Cancel Command
int HCI_CreateConnectionCancel(const HciCreateConnectionCancelParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_CREATE_CONNECTION_CANCEL, (void *)param, sizeof(HciCreateConnectionCancelParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.8 Accept Connection Request Command
int HCI_AcceptConnectionRequest(const HciAcceptConnectionReqestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_ACCEPT_CONNECTION_REQUEST, (void *)param, sizeof(HciAcceptConnectionReqestParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.9 Reject Connection Request Command
int HCI_RejectConnectionRequest(const HciRejectConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_REJECT_CONNECTION_REQUEST, (void *)param, sizeof(HciRejectConnectionRequestParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.10 Link Key Request Reply Command
int HCI_LinkKeyRequestReply(const HciLinkKeyRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LINK_KEY_REQUEST_REPLY, (void *)param, sizeof(HciLinkKeyRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.11 Link Key Request Negative Reply Command
int HCI_LinkKeyRequestNegativeReply(const HciLinkKeyRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_LINK_KEY_REQUEST_NEGATIVE_REPLY, (void *)param, sizeof(HciLinkKeyRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.12 PIN Code Request Reply Command
int HCI_PINCodeRequestReply(const HciPinCodeRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_PIN_CODE_REQUEST_REPLY, (void *)param, sizeof(HciPinCodeRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.13 PIN Code Request Negative Reply Command
int HCI_PINCodeRequestNegativeReply(const HciPinCodeRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_PIN_CODE_REQUEST_NEGATIVE_REPLY, (void *)param, sizeof(HciPinCodeRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.14 Change Connection Packet Type Command
int HCI_ChangeConnectionPacketType(const HciChangeConnectionPacketTypeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_CHANGE_CONNECTION_PACKET_TYPE, (void *)param, sizeof(HciChangeConnectionPacketTypeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.15 Authentication Requested Command
int HCI_AuthenticationRequested(const HciAuthenticationRequestedParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_AUTHENTICATION_REQUESTED, (void *)param, sizeof(HciAuthenticationRequestedParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.16 Set Connection Encryption Command
int HCI_SetConnectionEncryption(const HciSetConnectionEncryptionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_SET_CONNECTION_ENCRYPTION, (void *)param, sizeof(HciSetConnectionEncryptionParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.19 Remote Name Request Command
int HCI_RemoteNameRequest(const HciRemoteNameRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_REMOTE_NAME_REQUEST, (void *)param, sizeof(HciRemoteNameRequestParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.20 Remote Name Request Cancel Command
int HCI_RemoteNameRequestCancel(const HciRemoteNameRequestCancelParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_REMOTE_NAME_REQUEST_CANCEL, (void *)param, sizeof(HciRemoteNameRequestCancelParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.21 Read Remote Supported Features Command
int HCI_ReadRemoteSupportedFeatures(const HciReadRemoteSupportedFeaturesParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd =
        HciAllocCmd(HCI_READ_REMOTE_SUPPORTED_FEATURES, (void *)param, sizeof(HciReadRemoteSupportedFeaturesParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.22 Read Remote Extended Features Command
int HCI_ReadRemoteExtendedFeatures(const HciReadRemoteExtendedFeaturesParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd =
        HciAllocCmd(HCI_READ_REMOTE_EXTENDED_FEATURES, (void *)param, sizeof(HciReadRemoteExtendedFeaturesParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.23 Read Remote Version Information Command
int HCI_ReadRemoteVersionInformation(const HciReadRemoteVersionInformationParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd =
        HciAllocCmd(HCI_READ_REMOTE_VERSION_INFORMATION, (void *)param, sizeof(HciReadRemoteVersionInformationParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.26 Setup Synchronous Connection Command
int HCI_SetupSynchronousConnection(const HciSetupSynchronousConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd =
        HciAllocCmd(HCI_SETUP_SYNCHRONOUS_CONNECTION, (void *)param, sizeof(HciSetupSynchronousConnectionParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.27 Accept Synchronous Connection Request Command
int HCI_AcceptSynchronousConnectionRequest(const HciAcceptSynchronousConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(
        HCI_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST, (void *)param, sizeof(HciAcceptSynchronousConnectionRequestParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.28 Reject Synchronous Connection Request Command
int HCI_RejectSynchronousConnectionRequest(const HciRejectSynchronousConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(
        HCI_REJECT_SYNCHRONOUS_CONNECTION_REQUEST, (void *)param, sizeof(HciRejectSynchronousConnectionRequestParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.29 IO Capability Request Reply Command
int HCI_IOCapabilityRequestReply(const HciIOCapabilityRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_IO_CAPABILITY_REQUEST_REPLY, (void *)param, sizeof(HciIOCapabilityRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.30 User Confirmation Request Reply Command
int HCI_UserConfirmationRequestReply(const HciUserConfirmationRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_USER_CONFIRMATION_REQUEST_REPLY, (void *)param, sizeof(HciUserConfirmationRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.31 User Confirmation Request Negative Reply Command
int HCI_UserConfirmationRequestNegativeReply(const HciUserConfirmationRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_USER_CONFIRMATION_REQUEST_NEGATIVE_REPLY,
        (void *)param,
        sizeof(HciUserConfirmationRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.32 User Passkey Request Reply Command
int HCI_UserPasskeyRequestReply(const HciUserPasskeyRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_USER_PASSKEY_REQUEST_REPLY, (void *)param, sizeof(HciUserPasskeyRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.33 User Passkey Request Negative Reply Command
int HCI_UserPasskeyRequestNegativeReply(const HciUserPasskeyRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_USER_PASSKEY_REQUEST_NEGATIVE_REPLY, (void *)param, sizeof(HciUserPasskeyRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.34 Remote OOB Data Request Reply Command
int HCI_RemoteOOBDataRequestReply(const HciRemoteOobDataRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_REMOTE_OOB_DATA_REQUEST_REPLY, (void *)param, sizeof(HciRemoteOobDataRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.35 Remote OOB Data Request Negative Reply Command
int HCI_RemoteOOBDataRequestNegativeReply(const HciRemoteOobDataRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_REMOTE_OOB_DATA_REQUEST_NEGATIVE_REPLY, (void *)param, sizeof(HciRemoteOobDataRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.36 IO Capability Request Negative Reply Command
int HCI_IOCapabilityRequestNegativeReply(const HciIoCapabilityRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_IO_CAPABILITY_REQUEST_NEGATIVE_REPLY, (void *)param, sizeof(HciIoCapabilityRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.45 Enhanced Setup Synchronous Connection Command
int HCI_EnhancedSetupSynchronousConnection(const HciEnhancedSetupSynchronousConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(
        HCI_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION, (void *)param, sizeof(HciEnhancedSetupSynchronousConnectionParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.46 Enhanced Accept Synchronous Connection Request Command
int HCI_EnhancedAcceptSynchronousConnectionRequest(const HciEnhancedAcceptSynchronousConnectionRequestParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION_REQUEST,
        (void *)param,
        sizeof(HciEnhancedAcceptSynchronousConnectionRequestParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.1.53 Remote OOB Extended Data Request Reply Command
int HCI_RemoteOOBExtendedDataRequestReply(const HciRemoteOobExtendedDataRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(
        HCI_REMOTE_OOB_EXTENDED_DATA_REQUEST_REPLY, (void *)param, sizeof(HciRemoteOobExtendedDataRequestReplyParam));
    return HciSendCmd(cmd);
}