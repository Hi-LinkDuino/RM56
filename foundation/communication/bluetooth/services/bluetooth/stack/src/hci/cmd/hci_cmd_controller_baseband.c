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
// 7.3.1 Set Event Mask Command
int HCI_SetEventMask(const HciSetEventMaskParam *param)
{
    HciCmd *cmd = HciAllocCmd(HCI_SET_EVENT_MASK, (void *)param, sizeof(HciSetEventMaskParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.2 Reset Command
int HCI_Reset()
{
    HciCmd *cmd = HciAllocCmd(HCI_RESET, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.4 Flush Command
int HCI_Flush(const HciFlushParam *param)
{
    HciCmd *cmd = HciAllocCmd(HCI_FLUSH, (void *)param, sizeof(HciFlushParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.11 Write Local Name Command
int HCI_WriteLocalName(const HciWriteLocalNameParam *param)
{
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_LOCAL_NAME, (void *)param, sizeof(HciWriteLocalNameParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.18 Write Scan Enable Command
int HCI_WriteScanEnable(const HciWriteScanEnableParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    HciCmd *cmd = HciAllocCmd(HCI_WRITE_SCAN_ENABLE, (void *)param, sizeof(HciWriteScanEnableParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.20 Write Page Scan Activity Command
int HCI_WritePageScanActivity(const HciWritePageScanActivityParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_PAGE_SCAN_ACTIVITY, (void *)param, sizeof(HciWritePageScanActivityParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.22 Write Inquiry Scan Activity Command
int HCI_WriteInquiryScanActivity(const HciWriteInquiryScanActivityParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_INQUIRY_SCAN_ACTIVITY, (void *)param, sizeof(HciWriteInquiryScanActivityParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.26 Write Class of Device Command
int HCI_WriteClassofDevice(const HciWriteClassofDeviceParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_CLASS_OF_DEVICE, (void *)param, sizeof(HciWriteClassofDeviceParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.28 Write Voice Setting Command
int HCI_WriteVoiceSetting(const HciWriteVoiceSettingParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_VOICE_SETTING, (void *)param, sizeof(HciWriteVoiceSettingParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.39 Host Buffer Size Command
int HCI_HostBufferSize(const HciHostBufferSizeCmdParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_HOST_BURRER_SIZE, (void *)param, sizeof(HciHostBufferSizeCmdParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.45 Write Current IAC LAP Command
int HCI_WriteCurrentIacLap(const HciWriteCurrentIacLapParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }

    int result;

    const size_t length = 1 + param->numCurrentIAC * IACLAP_SIZE;
    uint8_t *data = MEM_MALLOC.alloc(length);
    if (data != NULL) {
        int offset = 0;
        data[offset] = param->numCurrentIAC;
        offset += 1;

        for (size_t i = 0; i < param->numCurrentIAC; i++) {
            (void)memcpy_s(data + offset, length - offset, &param->iacLAP[i], IACLAP_SIZE);
            offset += IACLAP_SIZE;
        }

        HciCmd *cmd = HciAllocCmd(HCI_WRITE_CURRENT_IAC_LAP, (void *)data, length);

        result = HciSendCmd(cmd);

        MEM_MALLOC.free(data);
    } else {
        result = BT_NO_MEMORY;
    }

    return result;
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.48 Write Inquiry Scan Type Command
int HCI_WriteInquiryScanType(const HciWriteInquiryScanTypeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_INQUIRY_SCAN_TYPE, (void *)param, sizeof(HciWriteInquiryScanTypeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.50 Write Inquiry Mode Command
int HCI_WriteInquiryMode(const HciWriteInquiryModeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_INQUIRY_MODE, (void *)param, sizeof(HciWriteInquiryModeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.52 Write Page Scan Type Command
int HCI_WritePageScanType(const HciWritePageScanTypeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_PAGE_SCAN_TYPE, (void *)param, sizeof(HciWritePageScanTypeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.56 Write Extended Inquiry Response Command
int HCI_WriteExtendedInquiryResponse(const HciWriteExtendedInquiryResponseParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd =
        HciAllocCmd(HCI_WRITE_EXTENDED_INQUIRY_RESPONSE, (void *)param, sizeof(HciWriteExtendedInquiryResponseParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.59 Write Simple Pairing Mode Command
int HCI_WriteSimplePairingMode(const HciWriteSimplePairingModeParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_SIMPLE_PAIRING_MODE, (void *)param, sizeof(HciWriteSimplePairingModeParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.60 Read Local OOB Data Command
int HCI_ReadLocalOOBData()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_LOCAL_OOB_DATA, NULL, 0);
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.79 Write LE Host Support Command
int HCI_WriteLeHostSupport(const HciWriteLeHostSupportParam *param)
{
    if (param == NULL) {
        return BT_BAD_PARAM;
    }
    HciCmd *cmd = HciAllocCmd(HCI_WRITE_LE_HOST_SUPPORT, (void *)param, sizeof(HciWriteLeHostSupportParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.92 Write Secure Connections Host Support Command
int HCI_WriteSecureConnectionsHostSupport(const HciWriteSecureConnectionsHostSupportParam *param)
{
    HciCmd *cmd = HciAllocCmd(
        HCI_WRITE_SECURE_CONNECTIONS_HOST_SUPPORT, (void *)param, sizeof(HciWriteSecureConnectionsHostSupportParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.94 Write Authenticated Payload Timeout Command
int HCI_WriteAuthenticatedPayloadTimeout(const HciWriteAuthenticatedPayloadTimeoutParam *param)
{
    HciCmd *cmd = HciAllocCmd(
        HCI_WRITE_AUTHENTICATED_PAYLOAD_TIMEOUT, (void *)param, sizeof(HciWriteAuthenticatedPayloadTimeoutParam));
    return HciSendCmd(cmd);
}

// BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
// 7.3.95 Read Local OOB Extended Data Command
int HCI_ReadLocalOOBExtendedData()
{
    HciCmd *cmd = HciAllocCmd(HCI_READ_LOCAL_OOB_EXTENDED_DATA, NULL, 0);
    return HciSendCmd(cmd);
}
