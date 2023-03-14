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

#include "btm_controller.h"

#include <securec.h>

#include "btstack.h"
#include "hci/hci.h"
#include "hci/hci_def.h"
#include "hci/hci_error.h"
#include "log.h"
#include "platform/include/allocator.h"
#include "platform/include/event.h"

#include "btm.h"

#define WAIT_CMD_TIMEOUT 15000

#define EXTENDED_FEATURES_PAGE_2 2

#define CHECK_RESULT_BREAK(x) \
    if ((x) != BT_NO_ERROR) { \
        break;                \
    }

// Setup Controller
static HciEventCallbacks g_hciEventCallbacks;

static Event *g_waitSetupController = NULL;

static HciResetReturnParam g_hciResetResult;
static HciReadBufferSizeReturnParam g_readBufferSizeResult;
static HciHostBufferSizeReturnParam g_hostBufferSizeResult;
static HciReadLocalVersionInformationReturnParam g_readLocalVersionResult;
static HciReadBdAddrReturnParam g_readBdAddrResult;
static HciReadLocalSupportedCommandsReturnParam g_readLocalSupportedCommandsResult;
static HciReadLocalSupportedFeaturesReturnParam g_readLocalSupportedFeaturesResult;
static HciSetEventMaskReturnParam g_setEventMaskResult;
static uint8_t g_readLocalSupportedCodecsResult;
static BtmLocalSupportedCodecs g_localSupportedCodecs;
static HciReadLocalExtendedFeaturesReturnParam g_readLocalExtendedFeaturesResult[MAX_EXTENED_FEATURES_PAGE_COUNT];
static HciWriteLeHostSupportReturnParam g_writeLeHostSupportedResult;

static HciLeReadBufferSizeReturnParam g_leReadBufferSizeResult;
static HciLeReadLocalSupportedFeaturesReturnParam g_leReadLocalSupportedFeaturesResult;
static HciLeSetEventMaskReturnParam g_leSetEventMaskResult;
static HciLeReadWhiteListSizeReturnParam g_leReadWhiteListSizeResult;
static HciLeReadResolvingListSizeReturnParam g_leReadResolvingListSizeResult;

static void BtmControllerOnResetComplete(const HciResetReturnParam *returnParam)
{
    g_hciResetResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnReadBufferSizeComplete(const HciReadBufferSizeReturnParam *returnParam)
{
    g_readBufferSizeResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnHostBufferSizeComplete(const HciHostBufferSizeReturnParam *returnParam)
{
    g_hostBufferSizeResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnReadLocalVersionInformationComplete(
    const HciReadLocalVersionInformationReturnParam *returnParam)
{
    g_readLocalVersionResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnReadBdAddrComplete(const HciReadBdAddrReturnParam *returnParam)
{
    g_readBdAddrResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnReadLocalSupportedCommandsComplete(
    const HciReadLocalSupportedCommandsReturnParam *returnParam)
{
    g_readLocalSupportedCommandsResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnReadLocalSupportedFeaturesComlete(
    const HciReadLocalSupportedFeaturesReturnParam *returnParam)
{
    g_readLocalSupportedFeaturesResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnReadLocalExtendedFeaturesComplete(const HciReadLocalExtendedFeaturesReturnParam *returnParam)
{
    if (returnParam->pageNumber < MAX_EXTENED_FEATURES_PAGE_COUNT) {
        g_readLocalExtendedFeaturesResult[returnParam->pageNumber] = *returnParam;
    }
    EventSet(g_waitSetupController);
}

static void BtmControllerOnSetEventMaskComplete(const HciSetEventMaskReturnParam *returnParam)
{
    g_setEventMaskResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerCopySupportedCodecs(const HciReadLocalSupportedCodecsReturnParam *returnParam)
{
    g_localSupportedCodecs.numberOfSupportedCodecs = returnParam->numberOfSupportedCodecs;
    if (g_localSupportedCodecs.numberOfSupportedCodecs) {
        g_localSupportedCodecs.supportedCodecs =
            MEM_MALLOC.alloc(sizeof(uint8_t) * g_localSupportedCodecs.numberOfSupportedCodecs);
        if (g_localSupportedCodecs.supportedCodecs) {
            for (uint8_t i = 0; i < g_localSupportedCodecs.numberOfSupportedCodecs; i++) {
                g_localSupportedCodecs.supportedCodecs[i] = returnParam->supportedCodecs[i];
            }
        }
    }
}

static void BtmControllerCopyVendorSpecificCodecs(const HciReadLocalSupportedCodecsReturnParam *returnParam)
{
    g_localSupportedCodecs.numberOfSupportedVendorSpecificCodecs = returnParam->numberOfSupportedVendorSpecificCodecs;
    if (g_localSupportedCodecs.numberOfSupportedVendorSpecificCodecs) {
        g_localSupportedCodecs.vendorSpecificCodecs = MEM_MALLOC.alloc(
            sizeof(BtmVendorSpecificCodec) * g_localSupportedCodecs.numberOfSupportedVendorSpecificCodecs);
        if (g_localSupportedCodecs.vendorSpecificCodecs) {
            for (uint8_t i = 0; i < g_localSupportedCodecs.numberOfSupportedVendorSpecificCodecs; i++) {
                g_localSupportedCodecs.vendorSpecificCodecs[i].companyID =
                    returnParam->vendorSpecificCodecs[i].companyID;
                g_localSupportedCodecs.vendorSpecificCodecs[i].vendorDefinedCodecID =
                    returnParam->vendorSpecificCodecs[i].vendorDefinedCodecID;
            }
        }
    }
}

static void BtmControllerOnReadLocalSupportedCodecs(const HciReadLocalSupportedCodecsReturnParam *returnParam)
{
    g_readLocalSupportedCodecsResult = returnParam->status;
    if (g_readLocalSupportedCodecsResult == HCI_SUCCESS) {
        BtmControllerCopySupportedCodecs(returnParam);
        BtmControllerCopyVendorSpecificCodecs(returnParam);
    }

    EventSet(g_waitSetupController);
}

static void BtmControllerOnLeReadBufferSizeComplete(const HciLeReadBufferSizeReturnParam *returnParam)
{
    g_leReadBufferSizeResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnLeReadLocalSupportedFeaturesComplete(
    const HciLeReadLocalSupportedFeaturesReturnParam *returnParam)
{
    g_leReadLocalSupportedFeaturesResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnLeSetEventMaskComplete(const HciLeSetEventMaskReturnParam *returnParam)
{
    g_leSetEventMaskResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnLeReadWhiteListSizeComplete(const HciLeReadWhiteListSizeReturnParam *returnParam)
{
    g_leReadWhiteListSizeResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnLeReadResolvingListSizeComplete(const HciLeReadResolvingListSizeReturnParam *returnParam)
{
    g_leReadResolvingListSizeResult = *returnParam;
    EventSet(g_waitSetupController);
}

static void BtmControllerOnWriteLeHostSupportedComplete(const HciWriteLeHostSupportReturnParam *returnParam)
{
    g_writeLeHostSupportedResult = *returnParam;
    EventSet(g_waitSetupController);
}

bool BtmIsControllerSupportedReadLocalSupportedCodecsCommand()
{
    return HCI_SUPPORT_READ_LOCAL_SUPPORTED_CODECS(g_readLocalSupportedCommandsResult.supportedCommands);
}

bool BtmIsControllerSupportedEnhancedSetupSynchronousConnection()
{
    return HCI_SUPPORT_ENHANCED_SETUP_SYNCHRONOUS_CONNECTION(g_readLocalSupportedCommandsResult.supportedCommands);
}

bool BtmIsControllerSupportedEnhancedAcceptSynchronousConnection()
{
    return HCI_SUPPORT_ENHANCED_ACCEPT_SYNCHRONOUS_CONNECTION(g_readLocalSupportedCommandsResult.supportedCommands);
}

static bool BtmIsControllerSupportedLeReadLocalP256PublicKey()
{
    return HCI_SUPPORT_LE_READ_LOCAL_P_256_PUBLIC_KEY(g_readLocalSupportedCommandsResult.supportedCommands);
}

static bool BtmIsControllerSupportedLeGenerateDhKey()
{
    return HCI_SUPPORT_LE_GENERATE_DH_KEY(g_readLocalSupportedCommandsResult.supportedCommands);
}

bool BtmIsControllerSupportedLeSetPrivacyMode()
{
    return HCI_SUPPORT_LE_SET_PRIVACY_MODE(g_readLocalSupportedCommandsResult.supportedCommands);
}

static HciEventCallbacks g_hciEventCallbacks = {
    .resetComplete = BtmControllerOnResetComplete,
    .readBufferSizeComplete = BtmControllerOnReadBufferSizeComplete,
    .hostBufferSizeComplete = BtmControllerOnHostBufferSizeComplete,
    .readLocalVersionInformationComplete = BtmControllerOnReadLocalVersionInformationComplete,
    .readBdAddrComplete = BtmControllerOnReadBdAddrComplete,
    .readLocalSupportedCommandsComplete = BtmControllerOnReadLocalSupportedCommandsComplete,
    .readLocalSupportedFeaturesComplete = BtmControllerOnReadLocalSupportedFeaturesComlete,
    .readLocalExtendedFeaturesComplete = BtmControllerOnReadLocalExtendedFeaturesComplete,
    .setEventMaskComplete = BtmControllerOnSetEventMaskComplete,
    .readLocalSupportedCodecsComplete = BtmControllerOnReadLocalSupportedCodecs,
    .writeLeHostSupportComplete = BtmControllerOnWriteLeHostSupportedComplete,

    .leSetEventMaskComplete = BtmControllerOnLeSetEventMaskComplete,
    .leReadBufferSizeComplete = BtmControllerOnLeReadBufferSizeComplete,
    .leReadLocalSupportedFeaturesComplete = BtmControllerOnLeReadLocalSupportedFeaturesComplete,
    .leReadWhiteListSizeComplete = BtmControllerOnLeReadWhiteListSizeComplete,
    .leReadResolvingListSizeComplete = BtmControllerOnLeReadResolvingListSizeComplete,
};

static uint64_t BtmGetLeEventMask()
{
    uint64_t leEventMask = LE_EVENT_MASK_DEFAULT;

    // BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E
    // Table 3.2: Bluetooth Controller supporting LE requirements

    // C6
    if (BTM_IsControllerSupportConnectionParametersRequestProcedure()) {
        leEventMask |= LE_EVENT_MASK_LE_REMOTE_CONNECTION_PARAMETER_REQUEST_EVENT;
    }
    // C8
    if (BTM_IsControllerSupportLeDataPacketLengthExtension()) {
        leEventMask |= LE_EVENT_MASK_LE_DATA_LENGTH_CHANGE_EVENT;
    }
    // C9
    if (BTM_IsControllerSupportLlPrivacy()) {
        leEventMask |= LE_EVENT_MASK_LE_DIRECTED_ADVERTISING_REPORT_EVENT;
    }
    // C11
    if (BTM_IsControllerSupportLe2MPhy() || BTM_IsControllerSupportLeCodedPhy()) {
        leEventMask |= LE_EVENT_MASK_LE_PHY_UPDATE_COMPLETE_EVENT;
    }
    // C17
    if (BTM_IsControllerSupportLeExtendedAdvertising()) {
        leEventMask |=
            (LE_EVENT_MASK_LE_SCAN_REQUEST_RECEIVED_EVENT | LE_EVENT_MASK_LE_EXTENDED_ADVERTISING_SET_TERMINATED_EVENT);
    }
    // C19
    if (BTM_IsControllerSupportLeExtendedAdvertising()) {
        leEventMask |=
            (LE_EVENT_MASK_LE_EXTENDED_SCAN_TIMEOUT_EVENT | LE_EVENT_MASK_LE_EXTENDED_ADVERTISING_REPORT_EVENT);
    }
    // C20
    if (BTM_IsControllerSupportLePeriodicAdvertising()) {
        leEventMask |= (LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_REPORT_EVENT |
                        LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_ESTABLISHED_EVENT |
                        LE_EVENT_MASK_LE_PERIODIC_ADVERTISING_SYNC_LOST_EVENT);
    }
    // C22
    if (BTM_IsControllerSupportChannelSelectionAlgorithm2()) {
        leEventMask |= LE_EVENT_MASK_LE_CHANNEL_SELECTION_ALGORITHM_EVENT;
    }
    // C23
    if (BTM_IsControllerSupportLlPrivacy() || BTM_IsControllerSupportLeExtendedAdvertising()) {
        leEventMask |= LE_EVENT_MASK_LE_ENHANCED_CONNECTION_COMPLETE_EVENT;
    }
    // Optional
    if (BtmIsControllerSupportedLeReadLocalP256PublicKey()) {
        leEventMask |= LE_EVENT_MASK_LE_READ_LOCAL_P256_PUBLIC_KEY_COMPLETE_EVENT;
    }
    if (BtmIsControllerSupportedLeGenerateDhKey()) {
        leEventMask |= LE_EVENT_MASK_LE_GENERATE_DHKEY_COMPLETE_EVENT;
    }

    return leEventMask;
}

static int BtmHciReset()
{
    int result = HCI_Reset();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_Reset failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_hciResetResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_Reset status: 0x%02x", g_hciResetResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_Reset Timeout");
        result = BT_OPERATION_FAILED;
    }

    return result;
}

static int BtmReadBufferSize()
{
    int result = HCI_ReadBufferSize();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_ReadBufferSize failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        switch (g_readBufferSizeResult.status) {
            case HCI_SUCCESS:
            case HCI_UNKNOWN_HCI_COMMAND:
                result = BT_NO_ERROR;
                break;
            default:
                LOG_ERROR("HCI_ReadBufferSize status: 0x%02x", g_readBufferSizeResult.status);
                result = BT_OPERATION_FAILED;
                break;
        }
    } else {
        LOG_ERROR("HCI_ReadBufferSize Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmHostBufferSize()
{
    HciHostBufferSizeCmdParam hostBufferSizeParam = {
        .hostAclDataPacketLength = L2CAP_MTU_SIZE,
        .hostSynchronousDataPacketLength = SCO_HOST_BUFFER_SIZE,
        .hostTotalNumAclDataPackets = HOST_ACL_DATA_PACKETS,
        .hostTotalNumSynchronousDataPackets = HOST_SCO_DATA_PACKETS,
    };
    int result = HCI_HostBufferSize(&hostBufferSizeParam);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_HostBufferSize failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        switch (g_hostBufferSizeResult.status) {
            case HCI_SUCCESS:
            case HCI_UNKNOWN_HCI_COMMAND:
                result = BT_NO_ERROR;
                break;
            default:
                LOG_ERROR("HCI_HostBufferSize status: 0x%02x", g_hostBufferSizeResult.status);
                result = BT_OPERATION_FAILED;
                break;
        }
    } else {
        LOG_ERROR("HCI_HostBufferSize Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmReadLocalVersionInformation()
{
    int result = HCI_ReadLocalVersionInformation();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_ReadLocalVersionInformation failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_readLocalVersionResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_ReadLocalVersionInformation status: 0x%02x", g_readLocalVersionResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_ReadLocalVersionInformation Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmReadBdAddr()
{
    int result = HCI_ReadBdAddr();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_ReadBdAddr failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_readBdAddrResult.status != HCI_SUCCESS) {
            LOG_INFO("No public address");
            result = BT_NO_ERROR;
        }
    } else {
        LOG_ERROR("HCI_ReadBdAddr Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmReadLocalSupportedCommands()
{
    int result = HCI_ReadLocalSupportedCommands();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_ReadLocalSupportedCommands failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_readLocalSupportedCommandsResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_ReadLocalSupportedCommands status: 0x%02x", g_readLocalSupportedCommandsResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_ReadLocalSupportedCommands Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmReadLocalSupportedFeatures()
{
    int result = HCI_ReadLocalSupportedFeatures();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_ReadLocalSupportedFeatures failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_readLocalSupportedFeaturesResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_ReadLocalSupportedFeatures status: 0x%02x", g_readLocalSupportedFeaturesResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_ReadLocalSupportedFeatures Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmReadLocalExtendedFeatures()
{
    int result;

    HciReadLocalExtendedFeaturesParam readLocalExtendedFeaturesParam = {
        .pageNumber = 0,
    };

    for (uint8_t i = 0; i < MAX_EXTENED_FEATURES_PAGE_COUNT; i++) {
        readLocalExtendedFeaturesParam.pageNumber = i;

        result = HCI_ReadLocalExtendedFeatures(&readLocalExtendedFeaturesParam);
        if (result != BT_NO_ERROR) {
            LOG_ERROR("HCI_ReadLocalExtendedFeatures failed: %{public}d", result);
            break;
        }
        if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) != 0) {
            LOG_ERROR("HCI_ReadLocalExtendedFeatures Timeout");
            result = BT_OPERATION_FAILED;
            break;
        }

        switch (g_readLocalExtendedFeaturesResult[i].status) {
            case HCI_SUCCESS:
            case HCI_UNKNOWN_HCI_COMMAND:
                result = BT_NO_ERROR;
                break;
            default:
                LOG_ERROR("HCI_ReadLocalExtendedFeatures status: 0x%02x", g_readLocalExtendedFeaturesResult[i].status);
                result = BT_OPERATION_FAILED;
                break;
        }

        if (result != BT_NO_ERROR) {
            break;
        }

        if (g_readLocalExtendedFeaturesResult[i].pageNumber >= g_readLocalExtendedFeaturesResult[i].maximunPageNumber) {
            break;
        }
    }

    return result;
}

static int BtmSetEventMask()
{
    HciSetEventMaskParam setEventMaskParam = {
        .eventMask = HCI_EVENT_MASK_CORE_5_0,
    };
    int result = HCI_SetEventMask(&setEventMaskParam);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_SetEventMask failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_setEventMaskResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_SetEventMask status: 0x%02x", g_setEventMaskResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_SetEventMask Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmReadLocalSupportedCodecs()
{
    int result = HCI_ReadLocalSupportedCodecs();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_ReadLocalSupportedCodecs failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_readLocalSupportedCodecsResult != HCI_SUCCESS) {
            LOG_ERROR("HCI_ReadLocalSupportedCodecs status: 0x%02x", g_readLocalSupportedCodecsResult);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_ReadLocalSupportedCodecs Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmWriteLeHostSupport()
{
    HciWriteLeHostSupportParam writeLeHostSupportParam = {
        .leSupportedHost = HCI_LE_SUPPORTED_HOST_ENABLED,
        .simultaneousLeHost = HCI_SIMULTANEOUS_LE_HOST_DISABLED,
    };
    int result = HCI_WriteLeHostSupport(&writeLeHostSupportParam);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_WriteLeHostSupport failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        switch (g_writeLeHostSupportedResult.status) {
            case HCI_SUCCESS:
            case HCI_UNKNOWN_HCI_COMMAND:
                result = BT_NO_ERROR;
                break;
            default:
                LOG_ERROR("HCI_WriteLeHostSupport status: 0x%02x", g_writeLeHostSupportedResult.status);
                result = BT_OPERATION_FAILED;
                break;
        }
    } else {
        LOG_ERROR("HCI_WriteLeHostSupport Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmLeReadBufferSize()
{
    int result = HCI_LeReadBufferSize();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_LeReadBufferSize failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_leReadBufferSizeResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_LeReadBufferSize status: 0x%02x", g_leReadBufferSizeResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_LeReadBufferSize Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmLeReadWhiteListSize()
{
    int result = HCI_LeReadWhiteListSize();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_LeReadWhiteListSize failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_leReadWhiteListSizeResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_LeReadWhiteListSize status: 0x%02x", g_leReadWhiteListSizeResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_LeReadWhiteListSize Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmLeReadLocalSupportedFeatures()
{
    int result = HCI_LeReadLocalSupportedFeatures();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_LeReadLocalSupportedFeatures failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_leReadLocalSupportedFeaturesResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_LeReadLocalSupportedFeatures status: 0x%02x", g_leReadLocalSupportedFeaturesResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_LeReadLocalSupportedFeatures Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmLeReadResolvingListSize()
{
    int result = HCI_LeReadResolvingListSize();
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_LeReadResolvingListSize failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_leReadResolvingListSizeResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_LeReadResolvingListSize status: 0x%02x", g_setEventMaskResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_LeReadResolvingListSize Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmLeSetEventMask()
{
    HciLeSetEventMaskParam lsSetEventMaskParam = {
        .leEventMask = BtmGetLeEventMask(),
    };
    int result = HCI_LeSetEventMask(&lsSetEventMaskParam);
    if (result != BT_NO_ERROR) {
        LOG_ERROR("HCI_LeSetEventMask failed: %{public}d", result);
        return result;
    }
    if (EventWait(g_waitSetupController, WAIT_CMD_TIMEOUT) == 0) {
        if (g_leSetEventMaskResult.status != HCI_SUCCESS) {
            LOG_ERROR("HCI_LeSetEventMask status: 0x%02x", g_leSetEventMaskResult.status);
            result = BT_OPERATION_FAILED;
        }
    } else {
        LOG_ERROR("HCI_LeSetEventMask Timeout");
        result = BT_OPERATION_FAILED;
    }
    return result;
}

static int BtmInitLeFeature()
{
    int result;

    do {
        result = BtmWriteLeHostSupport();
        CHECK_RESULT_BREAK(result);

        result = BtmLeReadBufferSize();
        CHECK_RESULT_BREAK(result);

        HCI_SetLeBufferSize(
            g_leReadBufferSizeResult.hcLeAclDataPacketLength, g_leReadBufferSizeResult.hcTotalNumLeDataPackets);

        result = BtmLeReadWhiteListSize();
        CHECK_RESULT_BREAK(result);

        result = BtmLeReadLocalSupportedFeatures();
        CHECK_RESULT_BREAK(result);

        if (BTM_IsControllerSupportLlPrivacy()) {
            result = BtmLeReadResolvingListSize();
            CHECK_RESULT_BREAK(result);
        }

        result = BtmLeSetEventMask();
    } while (0);

    return result;
}

int BtmInitController()
{
    int result;

    HCI_RegisterEventCallbacks(&g_hciEventCallbacks);

    g_waitSetupController = EventCreate(true);

    do {
        // Reset Command
        result = BtmHciReset();
        CHECK_RESULT_BREAK(result);

        // Read Buffer Size Command
        result = BtmReadBufferSize();
        CHECK_RESULT_BREAK(result);

        HCI_SetBufferSize(
            g_readBufferSizeResult.hcAclDataPacketLength, g_readBufferSizeResult.hcTotalNumAclDataPackets);

        // Host Buffer Size Command
        result = BtmHostBufferSize();
        CHECK_RESULT_BREAK(result);

        // Read Local Version Information Command
        result = BtmReadLocalVersionInformation();
        CHECK_RESULT_BREAK(result);

        // Read BD_ADDR Command
        result = BtmReadBdAddr();
        CHECK_RESULT_BREAK(result);

        // Read Local Supported Commands Command
        result = BtmReadLocalSupportedCommands();
        CHECK_RESULT_BREAK(result);

        // Read Local Supported Features Command
        result = BtmReadLocalSupportedFeatures();
        CHECK_RESULT_BREAK(result);

        // Read Local Extended Features Command
        result = BtmReadLocalExtendedFeatures();
        CHECK_RESULT_BREAK(result);

        // Set Event Mask Command
        result = BtmSetEventMask();
        CHECK_RESULT_BREAK(result);

        if (BtmIsControllerSupportedReadLocalSupportedCodecsCommand()) {
            BtmReadLocalSupportedCodecs();
        }

        if (BTM_IsControllerSupportLe()) {
            result = BtmInitLeFeature();
        }
    } while (0);

    EventDelete(g_waitSetupController);
    g_waitSetupController = NULL;

    HCI_DeregisterEventCallbacks(&g_hciEventCallbacks);

    return result;
}

bool BTM_IsControllerSupportBrEdr()
{
    return HCI_SUPPORT_BREDR(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportLe()
{
    return HCI_SUPPORT_LE(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportRssiInquiryResponse()
{
    return HCI_SUPPORT_RSSI_INQUIRY_RESPONSE(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportEirInquiryResponse()
{
    return HCI_SUPPORT_EIR_INQUIRY_RESPONSE(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportRoleSwitch()
{
    return HCI_SUPPORT_ROLE_SWITCH(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportHoldMode()
{
    return HCI_SUPPORT_HOLD_MODE(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportSniffMode()
{
    return HCI_SUPPORT_SNIFF_MODE(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportEsco()
{
    return HCI_SUPPORT_ESCO_EV3(g_readLocalSupportedFeaturesResult.lmpFeatures.raw) ||
           HCI_SUPPORT_ESCO_EV4(g_readLocalSupportedFeaturesResult.lmpFeatures.raw) ||
           HCI_SUPPORT_ESCO_EV5(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportSecureSimplePairing()
{
    return HCI_SUPPORT_SECURE_SIMPLE_PAIRING(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportSecureConnections()
{
    return HCI_SUPPORT_SECURE_CONNECTIONS(
        g_readLocalExtendedFeaturesResult[EXTENDED_FEATURES_PAGE_2].extendedLMPFeatures);
}

bool BTM_IsControllerSupportNonFlushablePacketBoundaryFlag()
{
    return HCI_SUPPORT_NON_FLUSHABLE_PACKET_BONDARY_FLAG(g_readLocalSupportedFeaturesResult.lmpFeatures.raw);
}

bool BTM_IsControllerSupportLePing()
{
    return HCI_SUPPORT_LE_PING(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportLlPrivacy()
{
    return HCI_SUPPORT_LL_PRIVACY(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportLe2MPhy()
{
    return HCI_SUPPORT_LE_2M_PHY(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportLeCodedPhy()
{
    return HCI_SUPPORT_LE_CODED_PHY(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportLeExtendedAdvertising()
{
    return HCI_SUPPORT_LE_EXTENDED_ADVERTISING(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportLeDataPacketLengthExtension()
{
    return HCI_SUPPORT_LE_DATA_PACKET_LENGTH_EXTENSION(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportChannelSelectionAlgorithm2()
{
    return HCI_SUPPURT_CHANNEL_SELECTION_ALGORITHM_2(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportConnectionParametersRequestProcedure()
{
    return HCI_SUPPORT_CONNECTION_PARAMETERS_REQUEST_PROCEDURE(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BTM_IsControllerSupportLePeriodicAdvertising()
{
    return HCI_SUPPORT_LE_PERIODIC_ADVERTISING(g_leReadLocalSupportedFeaturesResult.leFeatures.raw);
}

bool BtmGetLocalSupportedFeature(HciLmpFeatures *lmpFeature)
{
    *lmpFeature = g_readLocalSupportedFeaturesResult.lmpFeatures;
    return true;
}

int BTM_GetLocalAddr(BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    int result = BT_NO_ERROR;
    if (g_readBdAddrResult.status != HCI_SUCCESS) {
        result = BT_BAD_STATUS;
    } else {
        errno_t err = memcpy_s(addr->addr, BT_ADDRESS_SIZE, g_readBdAddrResult.bdAddr.raw, BT_ADDRESS_SIZE);
        if (err == EOK) {
            addr->type = BT_PUBLIC_DEVICE_ADDRESS;
        } else {
            result = BT_NO_MEMORY;
        }
    }
    return result;
}

int BTM_GetLocalSupportedFeatures(uint8_t features[8])
{
    int result = BT_NO_ERROR;

    if (g_readLocalSupportedFeaturesResult.status == BT_NO_ERROR) {
        errno_t err = memcpy_s(
            features, LMP_FEATURES_SIZE, g_readLocalSupportedFeaturesResult.lmpFeatures.raw, LMP_FEATURES_SIZE);
        if (err != EOK) {
            result = BT_NO_MEMORY;
        }
    } else {
        result = BT_BAD_STATUS;
    }

    return result;
}

int BTM_GetLocalVersionInformation(BtmLocalVersionInformation *localVersion)
{
    if (localVersion == NULL) {
        return BT_BAD_PARAM;
    }

    if (g_readLocalVersionResult.status != HCI_SUCCESS) {
        return BT_OPERATION_FAILED;
    }

    localVersion->hciVersion = g_readLocalVersionResult.hciVersion;
    localVersion->hciRevision = g_readLocalVersionResult.hciRevision;
    localVersion->lmpVersion = g_readLocalVersionResult.lmpVersion;
    localVersion->manufacturerName = g_readLocalVersionResult.manufacturerName;
    localVersion->lmpSubversion = g_readLocalVersionResult.lmpSubversion;

    return BT_NO_ERROR;
}

int BtmGetWhiteListSize(uint8_t *whiteListSize)
{
    if (g_leReadWhiteListSizeResult.status != HCI_SUCCESS) {
        return BT_OPERATION_FAILED;
    }

    *whiteListSize = g_leReadWhiteListSizeResult.whiteListSize;

    return BT_NO_ERROR;
}

int BtmGetResolvingListSize(uint8_t *resolvingListSize)
{
    if (g_leReadResolvingListSizeResult.status != HCI_SUCCESS) {
        return BT_OPERATION_FAILED;
    }

    *resolvingListSize = g_leReadResolvingListSizeResult.resolvingListSize;

    return BT_NO_ERROR;
}

int BTM_GetLocalSupportedCodecs(BtmLocalSupportedCodecs **localSupportedCodes)
{
    if (g_readLocalSupportedCodecsResult != HCI_SUCCESS) {
        return BT_OPERATION_FAILED;
    }

    *localSupportedCodes = &g_localSupportedCodecs;

    return BT_NO_ERROR;
}

int BTM_GetAclDataPacketLength(uint16_t *aclDataPacketLength)
{
    if (g_readBufferSizeResult.status != HCI_SUCCESS) {
        return BT_OPERATION_FAILED;
    }

    *aclDataPacketLength = g_readBufferSizeResult.hcAclDataPacketLength;

    return BT_NO_ERROR;
}

int BTM_GetLeAclDataPacketLength(uint16_t *leAclDataPacketLength)
{
    if (g_leReadBufferSizeResult.status != HCI_SUCCESS) {
        return BT_OPERATION_FAILED;
    }

    if (g_leReadBufferSizeResult.hcLeAclDataPacketLength) {
        *leAclDataPacketLength = g_leReadBufferSizeResult.hcLeAclDataPacketLength;
    } else {
        *leAclDataPacketLength = g_readBufferSizeResult.hcAclDataPacketLength;
    }

    return BT_NO_ERROR;
}

void BtmCloseController()
{
    if (g_localSupportedCodecs.supportedCodecs) {
        MEM_MALLOC.free(g_localSupportedCodecs.supportedCodecs);
        g_localSupportedCodecs.supportedCodecs = NULL;
    }
    if (g_localSupportedCodecs.vendorSpecificCodecs) {
        MEM_MALLOC.free(g_localSupportedCodecs.vendorSpecificCodecs);
        g_localSupportedCodecs.vendorSpecificCodecs = NULL;
    }
}