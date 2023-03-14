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

#include "gap_le.h"
#include "gap_internal.h"

#include <securec.h>

#include "allocator.h"
#include "log.h"

#include "btm.h"
#include "hci/hci.h"
#include "hci/hci_error.h"

typedef struct {
    GapAdvCallback callback;
    void *context;
} LeAdvCallback;

typedef struct {
    GapExAdvCallback callback;
    void *context;
} LeExAdvCallback;

static LeAdvCallback g_leAdvCallback;
static LeExAdvCallback g_leExAdvCallback;

static bool GapFindExAdvInfoByAdvHandle(void *nodeData, void *param)
{
    uint8_t advHandle = *(uint8_t *)param;

    return ((LeExAdvInfo *)nodeData)->advHandle == advHandle;
}

void GapLeReadMaximumAdvertisingDataLengthComplete(const HciLeReadMaximumAdvertisingDataLengthReturnParam *param)
{
    LeExAdvBlock *exAdvBlock = GapGetLeExAdvBlock();
    if (param->status == HCI_SUCCESS) {
        exAdvBlock->exAdvDataMaxLen = param->maximumAdvertisingDataLength;
    } else {
        exAdvBlock->exAdvDataMaxLen = 0;
    }
}

void GapLeReadNumberofSupportedAdvertisingSetsComplete(
    const HciLeReadNumberofSupportedAdvertisingSetsReturnParam *param)
{
    LeExAdvBlock *exAdvBlock = GapGetLeExAdvBlock();
    if (param->status == HCI_SUCCESS) {
        exAdvBlock->exAdvMaxNumber = param->numSupportedAdvertisingSets;
    } else {
        exAdvBlock->exAdvMaxNumber = 0;
    }
}

int GAP_LeExAdvGetMaxDataLen(uint16_t *len)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    LeExAdvBlock *exAdvBlock = GapGetLeExAdvBlock();
    *len = exAdvBlock->exAdvDataMaxLen;
    return GAP_SUCCESS;
}

int GAP_LeExAdvGetMaxHandleNum(uint8_t *num)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    LeExAdvBlock *exAdvBlock = GapGetLeExAdvBlock();
    *num = exAdvBlock->exAdvMaxNumber;
    return GAP_SUCCESS;
}

int GAP_RegisterExAdvCallback(const GapExAdvCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(
            &g_leExAdvCallback.callback, sizeof(g_leExAdvCallback.callback), 0x00, sizeof(g_leExAdvCallback.callback));
    } else {
        g_leExAdvCallback.callback = *callback;
    }
    g_leExAdvCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterExAdvCallback(void)
{
    (void)memset_s(
        &g_leExAdvCallback.callback, sizeof(g_leExAdvCallback.callback), 0x00, sizeof(g_leExAdvCallback.callback));
    g_leExAdvCallback.context = NULL;
    return GAP_SUCCESS;
}

static int GapLeSetAdvertisingSetRandomAddress(uint8_t advHandle, const uint8_t addr[BT_ADDRESS_SIZE])
{
    HciLeSetAdvertisingSetRandomAddressParam hciCmdParam;

    LeExAdvInfo *info = ListForEachData(GapGetLeExAdvBlock()->exAdvInfoList, GapFindExAdvInfoByAdvHandle, &advHandle);
    if (info != NULL) {
        (void)memcpy_s(info->randomAddress, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
    } else {
        info = MEM_MALLOC.alloc(sizeof(LeExAdvInfo));
        if (info != NULL) {
            info->advHandle = advHandle;
            (void)memcpy_s(info->randomAddress, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);
            ListAddLast(GapGetLeExAdvBlock()->exAdvInfoList, info);
        }
    }

    hciCmdParam.advertisingHandle = advHandle;
    (void)memcpy_s(hciCmdParam.randomAddress, BT_ADDRESS_SIZE, addr, BT_ADDRESS_SIZE);

    return HCI_LeSetAdvertisingSetRandomAddress(&hciCmdParam);
}

NO_SANITIZE("cfi")
void GapLeSetAdvertisingSetRandomAddressComplete(const HciLeSetAdvertisingSetRandomAddressReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvSetRandAddrResult) {
        g_leExAdvCallback.callback.exAdvSetRandAddrResult(param->status, g_leExAdvCallback.context);
    }
}

int GAP_LeExAdvSetRandAddr(uint8_t advHandle, const uint8_t addr[BT_ADDRESS_SIZE])
{
    int ret;
    LOG_INFO("%{public}s:" BT_ADDR_FMT, __FUNCTION__, BT_ADDR_FMT_OUTPUT(addr));
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetAdvertisingSetRandomAddress(advHandle, addr);
    }
    return ret;
}

static int GapLeSetExtendedAdvertisingParameters(
    uint8_t advHandle, uint8_t properties, int8_t txPower, GapLeExAdvParam advExParam)
{
    HciLeSetExtendedAdvertisingParametersParam hciCmdParam;
    hciCmdParam.advertisingHandle = advHandle;
    hciCmdParam.advertisingEventProperties = properties;
    (void)memcpy_s(hciCmdParam.priAdvertisingIntervalMin,
        sizeof(hciCmdParam.priAdvertisingIntervalMin),
        &advExParam.advIntervalMin,
        sizeof(hciCmdParam.priAdvertisingIntervalMin));
    (void)memcpy_s(hciCmdParam.priAdvertisingIntervalMax,
        sizeof(hciCmdParam.priAdvertisingIntervalMax),
        &advExParam.advIntervalMax,
        sizeof(hciCmdParam.priAdvertisingIntervalMax));
    hciCmdParam.priAdvertisingChannelMap = advExParam.advChannelMap;
    hciCmdParam.ownAddressType = BTM_GetOwnAddressType();
    hciCmdParam.peerAddressType = advExParam.peerAddr->type;
    (void)memcpy_s(hciCmdParam.peerAddress, BT_ADDRESS_SIZE, advExParam.peerAddr->addr, BT_ADDRESS_SIZE);
    hciCmdParam.advertisingFilterPolicy = advExParam.advFilterPolicy;
    hciCmdParam.advertisingTxPower = txPower;
    hciCmdParam.priAdvertisingPHY = advExParam.primaryAdvPhy;
    hciCmdParam.secondaryAdvertisingMaxSkip = advExParam.secondaryAdvMaxSkip;
    hciCmdParam.secondaryAdvertisingPHY = advExParam.secondaryAdvPhy;
    hciCmdParam.advertisingSID = advExParam.advSid;
    hciCmdParam.scanRequestNotificationEnable = advExParam.scanRequestNotifyEnable;
    return HCI_LeSetExtendedAdvertisingParameters(&hciCmdParam);
}

NO_SANITIZE("cfi")
void GapLeSetExtendedAdvertisingParametersComplete(const HciLeSetExtendedAdvertisingParametersReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvSetParamResult) {
        g_leExAdvCallback.callback.exAdvSetParamResult(
            param->status, param->selectedTxPower, g_leExAdvCallback.context);
    }
}

int GAP_LeExAdvSetParam(uint8_t advHandle, uint8_t properties, int8_t txPower, GapLeExAdvParam advExParam)
{
    int ret;
    LOG_INFO("%{public}s:", __FUNCTION__);
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetExtendedAdvertisingParameters(advHandle, properties, txPower, advExParam);
    }
    return ret;
}

static int GapLeSetExtendedAdvertisingData(
    uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference, uint8_t advDataLength, const uint8_t *advData)
{
    HciLeSetExtendedAdvertisingDataParam hciCmdParam;
    hciCmdParam.advertisingHandle = advHandle;
    hciCmdParam.fragmentPreference = fragmentPreference;
    hciCmdParam.operation = operation;
    hciCmdParam.advertisingDataLength = advDataLength;
    hciCmdParam.advertisingData = advData;

    return HCI_LeSetExtendedAdvertisingData(&hciCmdParam);
}

NO_SANITIZE("cfi") void GapLeSetExtendedAdvertisingDataComplete(const HciLeSetExtendedAdvertisingDataReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvSetDataResult) {
        g_leExAdvCallback.callback.exAdvSetDataResult(param->status, g_leExAdvCallback.context);
    }
}

int GAP_LeExAdvSetData(
    uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference, uint8_t advDataLength, const uint8_t *advData)
{
    int ret;
    LOG_INFO("%{public}s:", __FUNCTION__);
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetExtendedAdvertisingData(advHandle, operation, fragmentPreference, advDataLength, advData);
    }
    return ret;
}

static int GapLeSetExtendedScanResponseData(uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference,
    uint8_t scanResponseDataLength, const uint8_t *scanResponseData)
{
    HciLeSetExtendedScanResponseDataParam hciCmdParam;
    hciCmdParam.advertisingHandle = advHandle;
    hciCmdParam.fragmentPreference = fragmentPreference;
    hciCmdParam.operation = operation;
    hciCmdParam.scanResponseDataLength = scanResponseDataLength;
    hciCmdParam.scanResponseData = scanResponseData;

    return HCI_LeSetExtendedScanResponseData(&hciCmdParam);
}

NO_SANITIZE("cfi")
void GapLeSetExtendedScanResponseDataComplete(const HciLeSetExtendedScanResponseDataReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvSetScanRspDataResult) {
        g_leExAdvCallback.callback.exAdvSetScanRspDataResult(param->status, g_leExAdvCallback.context);
    }
}

int GAP_LeExAdvSetScanRspData(uint8_t advHandle, uint8_t operation, uint8_t fragmentPreference,
    uint8_t scanResponseDataLen, const uint8_t *scanResponseData)
{
    int ret;
    LOG_INFO("%{public}s:", __FUNCTION__);
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetExtendedScanResponseData(
            advHandle, operation, fragmentPreference, scanResponseDataLen, scanResponseData);
    }
    return ret;
}

static int GapLeSetExtendedAdvertisingEnable(uint8_t enable, uint8_t numberOfSet, const GapExAdvSet *advSet)
{
    int ret;
    HciLeSetExtendedAdvertisingEnableParam hciCmdParam;

    hciCmdParam.enable = enable;
    hciCmdParam.numberofSets = numberOfSet;
    hciCmdParam.sets = MEM_MALLOC.alloc(numberOfSet * sizeof(HciLeExtendedAdvertisingParamSet));
    if (hciCmdParam.sets) {
        for (int i = 0; i < numberOfSet; i++) {
            hciCmdParam.sets[i].adverHandle = advSet[i].advHandle;
            hciCmdParam.sets[i].duration = advSet[i].duration;
            hciCmdParam.sets[i].maxExtendAdvertisingEvents = advSet[i].maxExAdvEvt;
        }

        ret = HCI_LeSetExtendedAdvertisingEnable(&hciCmdParam);
        MEM_MALLOC.free(hciCmdParam.sets);
    } else {
        ret = GAP_ERR_OUT_OF_RES;
    }

    return ret;
}

NO_SANITIZE("cfi")
void GapLeSetExtendedAdvertisingEnableComplete(const HciLeSetExtendedAdvertisingEnableReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvSetEnableResult) {
        g_leExAdvCallback.callback.exAdvSetEnableResult(param->status, g_leExAdvCallback.context);
    }
}

int GAP_LeExAdvSetEnable(uint8_t enable, uint8_t numberOfSet, const GapExAdvSet *advSet)
{
    int ret;
    LOG_INFO("%{public}s:", __FUNCTION__);
    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeSetExtendedAdvertisingEnable(enable, numberOfSet, advSet);
    }
    return ret;
}

void GapOnLeScanRequestReceivedEvent(const HciLeScanRequestReceivedEventParam *eventParam)
{
    if (g_leExAdvCallback.callback.exAdvScanRequestReceived) {
        BtAddr addr;
        GapChangeHCIAddr(&addr, &eventParam->scannerAddress, eventParam->scannerAddressType);
        g_leExAdvCallback.callback.exAdvScanRequestReceived(
            eventParam->advertisingHandle, &addr, g_leExAdvCallback.context);
    }
}

void GapLeRemoveAdvertisingSetComplete(const HciLeRemoveAdvertisingSetReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvRemoveHandleResult) {
        g_leExAdvCallback.callback.exAdvRemoveHandleResult(param->status, g_leExAdvCallback.context);
    }
}

static int GapLeClearAdvertisingSets(void)
{
    ListClear(GapGetLeExAdvBlock()->exAdvInfoList);

    return HCI_LeClearAdvertisingSets();
}

NO_SANITIZE("cfi") void GapLeClearAdvertisingSetsComplete(const HciLeClearAdvertisingSetsReturnParam *param)
{
    if (g_leExAdvCallback.callback.exAdvClearHandleResult) {
        g_leExAdvCallback.callback.exAdvClearHandleResult(param->status, g_leExAdvCallback.context);
    }
}

int GAP_LeExAdvClearHandle(void)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = GapLeClearAdvertisingSets();
    }
    return ret;
}

void GapOnLeAdvertisingSetTerminated(const HciLeAdvertisingSetTerminatedEventParam *eventParam)
{
    if (eventParam->status == HCI_SUCCESS) {
        LeDeviceInfo *deviceInfo = ListForEachData(GapGetLeConnectionInfoBlock()->deviceList,
            GapFindLeConnectionDeviceByHandle,
            (void *)&eventParam->connectionHandle);
        if (deviceInfo != NULL && deviceInfo->ownAddr.type == BT_RANDOM_DEVICE_ADDRESS) {
            LeExAdvInfo *info = ListForEachData(GapGetLeExAdvBlock()->exAdvInfoList,
                GapFindExAdvInfoByAdvHandle,
                (void *)&eventParam->advertisingHandle);
            if (info != NULL) {
                LOG_INFO("%{public}s: change own address " BT_ADDR_FMT " -> " BT_ADDR_FMT,
                    __FUNCTION__,
                    BT_ADDR_FMT_OUTPUT(deviceInfo->ownAddr.addr),
                    BT_ADDR_FMT_OUTPUT(info->randomAddress));
                (void)memcpy_s(deviceInfo->ownAddr.addr, BT_ADDRESS_SIZE, info->randomAddress, BT_ADDRESS_SIZE);
            }
        }
        if (deviceInfo != NULL && GapLeDeviceNeedBond(deviceInfo)) {
            GapLeDoPair(&deviceInfo->addr);
        }
        if (eventParam->status == HCI_SUCCESS) {
            if (deviceInfo != NULL) {
                deviceInfo->ownAddrUpdated = true;
            }
            if (deviceInfo != NULL && deviceInfo->securityReq != NULL) {
                GapLeRequestSecurityProcess(deviceInfo);
            }
        }
    }

    if (g_leExAdvCallback.callback.exAdvTerminatedAdvSet) {
        g_leExAdvCallback.callback.exAdvTerminatedAdvSet(eventParam->status,
            eventParam->advertisingHandle,
            eventParam->connectionHandle,
            eventParam->numCompletedExtendedAdvertisingEvents,
            g_leExAdvCallback.context);
    }
}

int GAP_RegisterAdvCallback(const GapAdvCallback *callback, void *context)
{
    LOG_INFO("%{public}s:%{public}s", __FUNCTION__, callback ? "register" : "NULL");
    if (callback == NULL) {
        (void)memset_s(
            &g_leAdvCallback.callback, sizeof(g_leAdvCallback.callback), 0x00, sizeof(g_leAdvCallback.callback));
    } else {
        g_leAdvCallback.callback = *callback;
    }
    g_leAdvCallback.context = context;
    return GAP_SUCCESS;
}

int GAP_DeregisterAdvCallback(void)
{
    (void)memset_s(&g_leAdvCallback.callback, sizeof(g_leAdvCallback.callback), 0x00, sizeof(g_leAdvCallback.callback));
    g_leAdvCallback.context = NULL;
    return GAP_SUCCESS;
}

int GAP_LeAdvSetParam(uint8_t advType, GapLeAdvParam advParam)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else if (GapLeRolesCheck(GAP_LE_ROLE_PREIPHERAL) == false && advType != GAP_ADV_TYPE_SCAN_UNDIR &&
               advType != GAP_ADV_TYPE_NON_CONN_UNDIR) {
        ret = GAP_ERR_INVAL_PARAM;
    } else {
        HciLeSetAdvertisingParametersParam hciCmdParam;

        hciCmdParam.advertisingType = advType;
        hciCmdParam.advertisingIntervalMin = advParam.advIntervalMin;
        hciCmdParam.advertisingIntervalMax = advParam.advIntervalMax;
        hciCmdParam.ownAddressType = BTM_GetOwnAddressType();
        hciCmdParam.peerAddressType = advParam.peerAddr->type;
        (void)memcpy_s(hciCmdParam.peerAddress.raw, BT_ADDRESS_SIZE, advParam.peerAddr->addr, BT_ADDRESS_SIZE);
        hciCmdParam.advertisingChannelMap = advParam.advChannelMap;
        hciCmdParam.advertisingFilterPolicy = advParam.advFilterPolicy;

        ret = HCI_LeSetAdvertisingParameters(&hciCmdParam);
        if (ret != BT_NO_ERROR) {
            LOG_ERROR("HCI Command Error ret = %{public}d.", ret);
        }
    }

    return ret;
}

void GapLeAdvSetParamComplete(const HciLeSetAdvertisingParametersReturnParam *param)
{
    if (g_leAdvCallback.callback.advSetParamResult) {
        g_leAdvCallback.callback.advSetParamResult(param->status, g_leAdvCallback.context);
    }
}

int GAP_LeAdvReadTxPower(void)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        ret = HCI_LeReadAdvertisingChannelTxPower();
    }

    return ret;
}

void GapLeAdvReadTxPowerComplete(const HciLeReadAdvertisingChannelTxPowerReturnParam *param)
{
    if (g_leAdvCallback.callback.advReadTxPower) {
        g_leAdvCallback.callback.advReadTxPower(param->status, param->transmitPowerLevel, g_leAdvCallback.context);
    }
}

int GAP_LeAdvSetData(uint8_t advDataLength, const uint8_t *advData)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (advDataLength > GAP_ADVERTISING_DATA_LENGTH_MAX) {
        return GAP_ERR_INVAL_PARAM;
    }

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        HciLeSetAdvertisingDataParam hciCmdParam = {
            .advertisingDataLen = advDataLength,
        };
        (void)memcpy_s(hciCmdParam.advertisingData, sizeof(hciCmdParam.advertisingData), advData, advDataLength);

        ret = HCI_LeSetAdvertisingData(&hciCmdParam);
    }

    return ret;
}

void GapLeAdvSetDataComplete(const HciLeSetAdvertisingDataReturnParam *param)
{
    if (g_leAdvCallback.callback.advSetDataResult) {
        g_leAdvCallback.callback.advSetDataResult(param->status, g_leAdvCallback.context);
    }
}

int GAP_LeAdvSetScanRspData(uint8_t advDataLength, const uint8_t *advData)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (advDataLength > GAP_ADVERTISING_DATA_LENGTH_MAX) {
        return GAP_ERR_INVAL_PARAM;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        HciLeSetScanResponseDataParam hciCmdParam = {
            .scanResponseDataLength = advDataLength,
        };
        (void)memcpy_s(hciCmdParam.scanResponseData, sizeof(hciCmdParam.scanResponseData), advData, advDataLength);

        ret = HCI_LeSetScanResponseData(&hciCmdParam);
    }

    return ret;
}

void GapLeAdvSetScanRspDataComplete(const HciLeSetScanResponseDataReturnParam *param)
{
    if (g_leAdvCallback.callback.advSetScanRspDataResult) {
        g_leAdvCallback.callback.advSetScanRspDataResult(param->status, g_leAdvCallback.context);
    }
}

int GAP_LeAdvSetEnable(uint8_t enable)
{
    LOG_INFO("%{public}s:", __FUNCTION__);
    int ret;

    if (GapIsLeEnable() == false) {
        return GAP_ERR_NOT_ENABLE;
    }

    if (GapLeRolesCheck(GAP_LE_ROLE_BROADCASTER | GAP_LE_ROLE_PREIPHERAL) == false) {
        ret = GAP_ERR_INVAL_STATE;
    } else {
        HciLeSetAdvertisingEnableParam hciCmdParam = {
            .advertisingEnable = enable,
        };

        ret = HCI_LeSetAdvertisingEnable(&hciCmdParam);
    }

    return ret;
}

void GapLeAdvSetEnableComplete(const HciLeSetAdvertisingEnableReturnParam *param)
{
    if (g_leAdvCallback.callback.advSetEnableResult) {
        g_leAdvCallback.callback.advSetEnableResult(param->status, g_leAdvCallback.context);
    }
}
