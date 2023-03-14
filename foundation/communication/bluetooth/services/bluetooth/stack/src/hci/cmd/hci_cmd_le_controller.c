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

#define BITS_IN_BYTE 8

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.1 LE Set Event Mask Command
int HCI_LeSetEventMask(const HciLeSetEventMaskParam *param)
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_EVENT_MASK, (void *)param, sizeof(HciLeSetEventMaskParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.2 LE Read Buffer Size Command
int HCI_LeReadBufferSize()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_BUFFER_SIZE, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.3 LE Read Local Supported Features Command
int HCI_LeReadLocalSupportedFeatures()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_LOCAL_SUPPORTED_FEATURES, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.4 LE Set Random Address Command
int HCI_LeSetRandomAddress(const HciLeSetRandomAddressParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_RANDOM_ADDRESS, (void *)param, sizeof(HciLeSetRandomAddressParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.5 LE Set Advertising Parameters Command
int HCI_LeSetAdvertisingParameters(const HciLeSetAdvertisingParametersParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_LE_SET_ADVERTISING_PARAMETERS, (void *)param, sizeof(HciLeSetAdvertisingParametersParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.6 LE Read Advertising Channel Tx Power Command
int HCI_LeReadAdvertisingChannelTxPower()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_ADVERTISING_CHANNEL_TX_POWER, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.7 LE Set Advertising Data Command
int HCI_LeSetAdvertisingData(const HciLeSetAdvertisingDataParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_ADVERTISING_DATA, (void *)param, sizeof(HciLeSetAdvertisingDataParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.8 LE Set Scan Response Data Command
int HCI_LeSetScanResponseData(const HciLeSetScanResponseDataParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_SCAN_RESPONSE_DATA, (void *)param, sizeof(HciLeSetScanResponseDataParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.9 LE Set Advertising Enable Command
int HCI_LeSetAdvertisingEnable(const HciLeSetAdvertisingEnableParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_ADVERTISING_ENABLE, (void *)param, sizeof(HciLeSetAdvertisingEnableParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.10 LE Set Scan Parameters Command
int HCI_LeSetScanParameters(const HciLeSetScanParametersParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_SCAN_PARAMETERS, (void *)param, sizeof(HciLeSetScanParametersParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.11 LE Set Scan Enable Command
int HCI_LeSetScanEnable(const HciLeSetScanEnableParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_SCAN_ENABLE, (void *)param, sizeof(HciLeSetScanEnableParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.12 LE Create Connection Command
int HCI_LeCreateConnection(const HciLeCreateConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_CREATE_CONNECTION, (void *)param, sizeof(HciLeCreateConnectionParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.13 LE Create Connection Cancel Command
int HCI_LeCreateConnectionCancel()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_CREATE_CONNECTION_CANCEL, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.14 LE Read WL Size Command
int HCI_LeReadWhiteListSize()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_WHITE_LIST_SIZE, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.15 LE Clear WL Command
int HCI_LeClearWhiteList()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_CLEAR_WHITE_LIST, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.16 LE Add Device To WL Command
int HCI_LeAddDeviceToWhiteList(const HciLeAddDeviceToWhiteListParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_ADD_DEVICE_TO_WHITE_LIST, (void *)param, sizeof(HciLeAddDeviceToWhiteListParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.17 LE Remove Device From WL Command
int HCI_LeRemoveDeviceFromWhiteList(const HciLeRemoveDeviceFromWhiteListParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_LE_REMOVE_DEVICE_FROM_WHITE_LIST, (void *)param, sizeof(HciLeRemoveDeviceFromWhiteListParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.18 LE Connection Update Command
int HCI_LeConnectionUpdate(const HciLeConnectionUpdateParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_CONNECTION_UPDATE, (void *)param, sizeof(HciLeConnectionUpdateParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.19 LE Set Host Channel Classification Command
int HCI_LeSetHostChannelClassification(const HciLeSetHostChannelClassificationParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION, (void *)param, sizeof(HciLeSetHostChannelClassificationParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.20 LE Read Channel Map Command
int HCI_LeReadChannelMap(const HciLeReadChannelMapParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_CHANNEL_MAP, (void *)param, sizeof(HciLeReadChannelMapParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.21 LE Read Remote Features Command
int HCI_LeReadRemoteFeatures(const HciLeReadRemoteFeaturesParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_REMOTE_FEATURES, (void *)param, sizeof(HciLeReadRemoteFeaturesParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.22 LE Encrypt Command
int HCI_LeEncrypt(const HciLeEncryptParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_ENCRYPT, (void *)param, sizeof(HciLeEncryptParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.23 LE Rand Command
int HCI_LeRand()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_RAND, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.24 LE Start Encryption Command
int HCI_LeStartEncryption(const HciLeStartEncryptionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_START_ENCRYPTION, (void *)param, sizeof(HciLeStartEncryptionParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.25 LE Long Term Key Request Reply Command
int HCI_LeLongTermKeyRequestReply(const HciLeLongTermKeyRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_LE_LONG_TERM_KEY_REQUEST_REPLY, (void *)param, sizeof(HciLeLongTermKeyRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.26 LE Long Term Key Request Negative Reply Command
int HCI_LeLongTermKeyRequestNegativeReply(const HciLeLongTermKeyRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_LE_LONG_TERM_KEY_REQUEST_NEGATIVE_REPLY, (void *)param, sizeof(HciLeLongTermKeyRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.31 LE Remote Connection Parameter Request Reply Command
int HCI_LeRemoteConnectionParameterRequestReply(const HciLeRemoteConnectionParameterRequestReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_REPLY,
        (void *)param,
        sizeof(HciLeRemoteConnectionParameterRequestReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.32 LE Remote Connection Parameter Request Negative Reply Command
int HCI_LeRemoteConnectionParameterRequestNegativeReply(
    const HciLeRemoteConnectionParameterRequestNegativeReplyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_NEGATIVE_REPLY,
        (void *)param,
        sizeof(HciLeRemoteConnectionParameterRequestNegativeReplyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.36 LE Read Local P-256 Public Key Command
int HCI_LeReadLocalP256PublicKey()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_LOCAL_P256_PUBLIC_KEY, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.37 LE Generate DHKey Command
int HCI_LeGenerateDHKey(const HciLeGenerateDHKeyParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_GENERATE_DHKEY, (void *)param, sizeof(HciLeGenerateDHKeyParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.38 LE Add Device To Resolving List Command
int HCI_LeAddDeviceToResolvingList(const HciLeAddDeviceToResolvingListParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST, (void *)param, sizeof(HciLeAddDeviceToResolvingListParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.39 LE Remove Device From Resolving List Command
int HCI_LeRemoveDeviceFromResolvingList(const HciLeRemoveDeviceFromResolvingListParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST, (void *)param, sizeof(HciLeRemoveDeviceFromResolvingListParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.40 LE Clear Resolving List Command
int HCI_LeClearResolvingList()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_CLEAR_RESOLVING_LIST, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.41 LE Read Resolving List Size Command
int HCI_LeReadResolvingListSize()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_RESOLVING_LIST_SIZE, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.44 LE Set Address Resolution Enable Command
int HCI_LeSetAddressResolutionEnable(const HciLeSetAddressResolutionEnableParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd =
        HciAllocCmd(HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE, (void *)param, sizeof(HciLeSetAddressResolutionEnableParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.52 LE Set Advertising Set Random Address Command
int HCI_LeSetAdvertisingSetRandomAddress(const HciLeSetAdvertisingSetRandomAddressParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_LE_SET_ADVERTISING_SET_RANDOM_ADDRESS, (void *)param, sizeof(HciLeSetAdvertisingSetRandomAddressParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.53 LE Set Extended Advertising Parameters Command
int HCI_LeSetExtendedAdvertisingParameters(const HciLeSetExtendedAdvertisingParametersParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(
        HCI_LE_SET_EXTENDED_ADVERTISING_PARAMETERS, (void *)param, sizeof(HciLeSetExtendedAdvertisingParametersParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.54 LE Set Extended Advertising Data Command
int HCI_LeSetExtendedAdvertisingData(const HciLeSetExtendedAdvertisingDataParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    const size_t length =
        sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + param->advertisingDataLength;
    uint8_t *buf = MEM_MALLOC.alloc(length);
    if (buf == NULL) {
        return BT_NO_MEMORY;
    }

    uint16_t index = 0;
    buf[index] = param->advertisingHandle;
    index += sizeof(uint8_t);
    buf[index] = param->operation;
    index += sizeof(uint8_t);
    buf[index] = param->fragmentPreference;
    index += sizeof(uint8_t);
    buf[index] = param->advertisingDataLength;
    index += sizeof(uint8_t);

    if (param->advertisingDataLength > 0) {
        (void)memcpy_s(buf + index, length - index, param->advertisingData, param->advertisingDataLength);
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_EXTENDED_ADVERTISING_DATA, (void *)buf, length);

    int result = HciSendCmd(cmd);
    MEM_MALLOC.free(buf);
    return result;
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.55 LE Set Extended Scan Response Data Command
int HCI_LeSetExtendedScanResponseData(const HciLeSetExtendedScanResponseDataParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    const size_t length =
        sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + param->scanResponseDataLength;
    uint8_t *buf = MEM_MALLOC.alloc(length);
    if (buf == NULL) {
        return BT_NO_MEMORY;
    }

    uint16_t index = 0;
    buf[index] = param->advertisingHandle;
    index += sizeof(uint8_t);
    buf[index] = param->operation;
    index += sizeof(uint8_t);
    buf[index] = param->fragmentPreference;
    index += sizeof(uint8_t);
    buf[index] = param->scanResponseDataLength;
    index += sizeof(uint8_t);
    if (param->scanResponseDataLength > 0) {
        (void)memcpy_s(buf + index, length - index, param->scanResponseData, param->scanResponseDataLength);
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_EXTENDED_SCAN_RESPONSE_DATA, (void *)buf, length);
    int result = HciSendCmd(cmd);

    MEM_MALLOC.free(buf);
    return result;
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.56 LE Set Extended Advertising Enable Command
int HCI_LeSetExtendedAdvertisingEnable(const HciLeSetExtendedAdvertisingEnableParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    const size_t length =
        sizeof(uint8_t) + sizeof(uint8_t) + sizeof(HciLeExtendedAdvertisingParamSet) * param->numberofSets;
    uint8_t *buf = MEM_MALLOC.alloc(length);
    if (buf == NULL) {
        return BT_NO_MEMORY;
    }

    uint16_t index = 0;
    buf[index] = param->enable;
    index += sizeof(uint8_t);
    buf[index] = param->numberofSets;
    index += sizeof(uint8_t);
    for (uint8_t i = 0; i < param->numberofSets; i++) {
        (void)memcpy_s(buf + index, length - index, param->sets + i, sizeof(HciLeExtendedAdvertisingParamSet));
        index += sizeof(HciLeExtendedAdvertisingParamSet);
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_EXTENDED_ADVERTISING_ENABLE, (void *)buf, length);

    int result = HciSendCmd(cmd);

    MEM_MALLOC.free(buf);

    return result;
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.57 LE Read Maximum Advertising Data Length Command
int HCI_LeReadMaximumAdvertisingDataLength()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_MAXIMUM_ADVERTISING_DATA_LENGTH, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.58 LE Read Number of Supported Advertising Sets Command
int HCI_LeReadNumberofSupportedAdvertisingSets()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_READ_NUMBER_OF_SUPPORTED_ADVERTISING_SETS, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.59 LE Remove Advertising Set Command
int HCI_LeRemoveAdvertisingSet(const HciLeRemoveAdvertisingSetParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_REMOVE_ADVERTISING_SET, (void *)param, sizeof(HciLeRemoveAdvertisingSetParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.60 LE Clear Advertising Sets Command
int HCI_LeClearAdvertisingSets()
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_CLEAR_ADVERTISING_SETS, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.64 LE Set Extended Scan Parameters Command
int HCI_LeSetExtendedScanParameters(const HciLeSetExtendedScanParametersParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    uint8_t numberOfSets = 0;
    for (uint8_t i = 0; i < BITS_IN_BYTE; i++) {
        if ((param->ScanningPHYs >> i) & 0x01) {
            numberOfSets++;
        }
    }
    const size_t length =
        sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(HciLeExtendedScanParametersSet) * numberOfSets;
    uint8_t *buf = MEM_MALLOC.alloc(length);
    if (buf == NULL) {
        return BT_NO_MEMORY;
    }

    uint16_t index = 0;
    buf[index] = param->ownAddressType;
    index += sizeof(uint8_t);
    buf[index] = param->ScanningFilterPolicy;
    index += sizeof(uint8_t);
    buf[index] = param->ScanningPHYs;
    index += sizeof(uint8_t);

    for (uint8_t i = 0; i < numberOfSets; i++) {
        (void)memcpy_s(buf + index, length - index, param->sets + i, sizeof(HciLeExtendedScanParametersSet));
        index += sizeof(HciLeExtendedScanParametersSet);
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_EXTENDED_SCAN_PARAMETERS, (void *)buf, length);
    int result = HciSendCmd(cmd);

    MEM_MALLOC.free(buf);

    return result;
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.65 LE Set Extended Scan Enable Command
int HCI_LeSetExtendedScanEnable(const HciLeSetExtendedScanEnableParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_EXTENDED_SCAN_ENABLE, (void *)param, sizeof(HciLeSetExtendedScanEnableParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.66 LE Extended Create Connection Command
int HCI_LeExtenedCreateConnection(const HciLeExtendedCreateConnectionParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    uint8_t countOfSets = 0;
    if (param->initiatingPhys & LE_1M_PHY) {
        countOfSets++;
    }
    if (param->initiatingPhys & LE_2M_PHY) {
        countOfSets++;
    }
    if (param->initiatingPhys & LE_CODED_PHY) {
        countOfSets++;
    }

    const size_t length = sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint8_t) + sizeof(HciBdAddr) + sizeof(uint8_t) +
                          sizeof(HciLeConnectionParamSet) * countOfSets;
    uint8_t *buf = MEM_MALLOC.alloc(length);
    if (buf == NULL) {
        return BT_NO_MEMORY;
    }

    uint16_t index = 0;

    buf[index] = param->initiatingFilterPolicy;
    index += sizeof(uint8_t);

    buf[index] = param->ownAddressType;
    index += sizeof(uint8_t);

    buf[index] = param->peerAddressType;
    index += sizeof(uint8_t);

    (void)memcpy_s(buf + index, length - index, param->peerAddress.raw, sizeof(HciBdAddr));
    index += sizeof(HciBdAddr);

    buf[index] = param->initiatingPhys;
    index += sizeof(uint8_t);

    for (uint8_t i = 0; i < countOfSets; i++) {
        (void)memcpy_s(buf + index, length - index, param->sets + i, sizeof(HciLeConnectionParamSet));
        index += sizeof(HciLeConnectionParamSet);
    }

    HciCmd *cmd = HciAllocCmd(HCI_LE_EXTENDED_CREATE_CONNECTION, (void *)buf, length);
    int result = HciSendCmd(cmd);

    MEM_MALLOC.free(buf);

    return result;
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.8.77 LE Set Privacy Mode Command
int HCI_LeSetPrivacyMode(const HciLeSetPrivacyModeParam *param)
{
    HciCmd *cmd = HciAllocCmd(HCI_LE_SET_PRIVACY_MODE, (void *)param, sizeof(HciLeSetPrivacyModeParam));
    return HciSendCmd(cmd);
}
