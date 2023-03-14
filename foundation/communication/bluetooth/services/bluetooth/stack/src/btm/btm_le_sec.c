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

#include "btm_le_sec.h"

#include "hci/hci.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"
#include "smp/smp.h"

#include "btm.h"
#include "btm_acl.h"
#include "btm_controller.h"

#define STATUS_NONE 0
#define STATUS_INITIALIZED 1

#define IS_INITIALIZED() (g_status == STATUS_INITIALIZED)

typedef struct {
    BtmLePairedDevice pairedInfo;
    BtAddr currentAddr;
    bool inResolvingList;
} BtmLePairedDeviceBlock;

static BtmKey g_localIdentityResolvingKey;
static List *g_lePairedDevices = NULL;
static Mutex *g_lePairedDevicesLock = NULL;
static uint8_t g_resolvingListSize = 0;
static uint8_t g_deviceCountInResolvingList = 0;
static Mutex *g_ownAddressTypeLock = NULL;
static uint8_t g_ownAddressType = OWN_ADDRESS_TYPE_PUBLIC;
static BtAddr g_randomAddress;
static Mutex *g_randomAddressLock = NULL;

static uint8_t g_status = STATUS_NONE;

static BtmLePairedDeviceBlock *BtmAllocLePairedDeviceBlock(const BtmLePairedDevice *device)
{
    BtmLePairedDeviceBlock *block = MEM_MALLOC.alloc(sizeof(BtmLePairedDeviceBlock));
    if (block != NULL) {
        block->pairedInfo = *device;
        block->currentAddr = device->addr;
        block->inResolvingList = false;
    }
    return block;
}

static void BtmFreeLePairedDeviceBlock(void *device)
{
    MEM_MALLOC.free(device);
}

static void BtmEnableAddressResolution()
{
    HciLeSetAddressResolutionEnableParam hciParam = {
        .addressResolutionEnable = ADDRESS_RESOLUTION_IN_CONTROLLER_ENABLEED,
    };
    HCI_LeSetAddressResolutionEnable(&hciParam);
}

static void BtmDisableAddressResolution()
{
    HciLeSetAddressResolutionEnableParam hciParam = {
        .addressResolutionEnable = ADDRESS_RESOLUTION_IN_CONTROLLER_DISABLED,
    };
    HCI_LeSetAddressResolutionEnable(&hciParam);
}

static void BtmAddToResolvingList(const BtmLePairedDevice *device)
{
    uint8_t peerIdentityAddrType = HCI_PEER_IDENTITY_ADDRESS_TYPE_PUBLIC;
    if (device->remoteIdentityAddress.type == BT_RANDOM_DEVICE_ADDRESS ||
        device->remoteIdentityAddress.type == BT_RANDOM_IDENTITY_ADDRESS) {
        peerIdentityAddrType = HCI_PEER_IDENTITY_ADDRESS_TYPE_RANDOM;
    }

    HciLeAddDeviceToResolvingListParam param = {
        .peerIdentityAddrType = peerIdentityAddrType,
        .peerIdentityAddress = {.raw = {0}},
        .peerIrk = {0},
        .localIrk = {0},
    };
    (void)memcpy_s(param.peerIdentityAddress.raw, BT_ADDRESS_SIZE, device->remoteIdentityAddress.addr, BT_ADDRESS_SIZE);
    (void)memcpy_s(param.peerIrk, KEY_SIZE, device->remoteIdentityResolvingKey.key, KEY_SIZE);
    (void)memcpy_s(param.localIrk, KEY_SIZE, g_localIdentityResolvingKey.key, KEY_SIZE);
    HCI_LeAddDeviceToResolvingList(&param);
}

static void BtmSetPrivacyMode(const BtmLePairedDevice *device)
{
    uint8_t peerIdentityAddressType = HCI_PEER_ADDR_TYPE_PUBLIC;
    if (device->remoteIdentityAddress.type == BT_RANDOM_DEVICE_ADDRESS ||
        device->remoteIdentityAddress.type == BT_RANDOM_IDENTITY_ADDRESS) {
        peerIdentityAddressType = HCI_PEER_ADDR_TYPE_RANDOM;
    }

    HciLeSetPrivacyModeParam param = {
        .peerIdentityAddressType = peerIdentityAddressType,
        .peerIdentityAddress = {.raw = {0}},
        .privacyMode = HCI_DEVICE_PRIVACY_MODE,
    };
    (void)memcpy_s(param.peerIdentityAddress.raw, BT_ADDRESS_SIZE, device->remoteIdentityAddress.addr, BT_ADDRESS_SIZE);

    HCI_LeSetPrivacyMode(&param);
}

static void BtmRemoveFromResolvingList(const BtmLePairedDevice *device)
{
    uint8_t peerIdentityAddrType = HCI_PEER_IDENTITY_ADDRESS_TYPE_PUBLIC;
    if (device->remoteIdentityAddress.type == BT_RANDOM_DEVICE_ADDRESS ||
        device->remoteIdentityAddress.type == BT_RANDOM_IDENTITY_ADDRESS) {
        peerIdentityAddrType = HCI_PEER_IDENTITY_ADDRESS_TYPE_RANDOM;
    }
    HciLeRemoveDeviceFromResolvingListParam param = {
        .peerIdentityAddrType = peerIdentityAddrType,
        .peerIdentityAddress = {.raw = {0}},
    };
    (void)memcpy_s(param.peerIdentityAddress.raw, BT_ADDRESS_SIZE, device->remoteIdentityAddress.addr, BT_ADDRESS_SIZE);
    HCI_LeRemoveDeviceFromResolvingList(&param);
}

static bool IsSameBtAddr(const BtAddr *addr1, const BtAddr *addr2)
{
    bool isSame = true;

    for (uint8_t i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr1->addr[i] != addr2->addr[i]) {
            isSame = false;
            break;
        }
    }

    return isSame;
}

static bool IsZeroAddress(const uint8_t addr[BT_ADDRESS_SIZE])
{
    bool isZeroAddress = true;
    for (uint8_t i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr[i] != 0) {
            isZeroAddress = false;
            break;
        }
    }
    return isZeroAddress;
}

static BtmLePairedDeviceBlock *BtmFindLePairedDeviceBlockByAddress(const BtAddr *addr)
{
    BtmLePairedDeviceBlock *block = NULL;

    ListNode *node = ListGetFirstNode(g_lePairedDevices);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (IsSameBtAddr(&block->pairedInfo.addr, addr)) {
            break;
        } else {
            block = NULL;
        }

        node = ListGetNextNode(node);
    }

    return block;
}

static BtmLePairedDeviceBlock *BtmFindLePairedDeviceBlockByRemoteIdentityAddress(const BtAddr *addr)
{
    BtmLePairedDeviceBlock *block = NULL;

    ListNode *node = ListGetFirstNode(g_lePairedDevices);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (IsSameBtAddr(&block->pairedInfo.remoteIdentityAddress, addr)) {
            break;
        } else {
            block = NULL;
        }

        node = ListGetNextNode(node);
    }

    return block;
}

void BtmInitLeSecurity()
{
    g_lePairedDevices = ListCreate(BtmFreeLePairedDeviceBlock);
    g_lePairedDevicesLock = MutexCreate();

    g_ownAddressTypeLock = MutexCreate();
    g_randomAddressLock = MutexCreate();

    g_ownAddressType = OWN_ADDRESS_TYPE_PUBLIC;

    g_status = STATUS_INITIALIZED;
}

void BtmCloseLeSecurity()
{
    g_status = STATUS_NONE;

    if (g_lePairedDevices != NULL) {
        ListDelete(g_lePairedDevices);
        g_lePairedDevices = NULL;
    }

    if (g_lePairedDevicesLock != NULL) {
        MutexDelete(g_lePairedDevicesLock);
        g_lePairedDevicesLock = NULL;
    }

    if (g_ownAddressTypeLock != NULL) {
        MutexDelete(g_ownAddressTypeLock);
        g_ownAddressTypeLock = NULL;
    }

    if (g_randomAddressLock != NULL) {
        MutexDelete(g_randomAddressLock);
        g_randomAddressLock = NULL;
    }
}

void BtmStartLeSecurity()
{
    g_resolvingListSize = 0;
    if (BTM_IsControllerSupportLlPrivacy()) {
        uint8_t resolvingListSize = 0;
        int result = BtmGetResolvingListSize(&resolvingListSize);
        if (result == BT_NO_ERROR) {
            g_resolvingListSize = resolvingListSize;
        }

        HCI_LeClearResolvingList();
        BtmEnableAddressResolution();
    }

    g_ownAddressType = OWN_ADDRESS_TYPE_PUBLIC;
}

void BtmStopLeSecurity()
{
    MutexLock(g_lePairedDevicesLock);
    ListClear(g_lePairedDevices);
    MutexUnlock(g_lePairedDevicesLock);
}

void BTM_SetLePairedDevices(const BtmLePairedDevice *pairedDevices, uint16_t count)
{
    if (!IS_INITIALIZED()) {
        return;
    }

    MutexLock(g_lePairedDevicesLock);

    BtmStopAutoConnection();

    ListClear(g_lePairedDevices);

    if (BTM_IsControllerSupportLlPrivacy()) {
        BtmDisableAddressResolution();
        HCI_LeClearResolvingList();
    }

    BtmLePairedDeviceBlock *block = NULL;
    for (uint16_t i = 0; i < count; i++) {
        block = BtmAllocLePairedDeviceBlock(pairedDevices + i);
        if (block == NULL) {
            continue;
        }

        ListAddLast(g_lePairedDevices, block);

        if (IsZeroAddress(block->pairedInfo.remoteIdentityAddress.addr)) {
            block->inResolvingList = false;
            continue;
        }

        if (BTM_IsControllerSupportLlPrivacy() && g_deviceCountInResolvingList < g_resolvingListSize) {
            BtmAddToResolvingList(pairedDevices + i);
            block->inResolvingList = true;
            g_deviceCountInResolvingList++;

            if (BtmIsControllerSupportedLeSetPrivacyMode()) {
                BtmSetPrivacyMode(pairedDevices + i);
            }
        }
    }

    if (BTM_IsControllerSupportLlPrivacy()) {
        BtmEnableAddressResolution();
    }

    BtmStartAutoConnection();

    MutexUnlock(g_lePairedDevicesLock);
}

void BTM_AddLePairedDevice(const BtmLePairedDevice *device)
{
    if (device == NULL) {
        return;
    }

    if (!IS_INITIALIZED()) {
        return;
    }

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmAllocLePairedDeviceBlock(device);
    if (block == NULL) {
        MutexUnlock(g_lePairedDevicesLock);
        return;
    }
    ListAddLast(g_lePairedDevices, block);

    bool addToResolvingList = false;
    if (IsZeroAddress(block->pairedInfo.remoteIdentityAddress.addr)) {
        addToResolvingList = false;
    } else {
        if (g_deviceCountInResolvingList < g_resolvingListSize) {
            addToResolvingList = true;
        }
    }

    if (addToResolvingList && BTM_IsControllerSupportLlPrivacy()) {
        BtmStopAutoConnection();

        BtmDisableAddressResolution();

        BtmAddToResolvingList(device);
        block->inResolvingList = true;
        g_deviceCountInResolvingList++;

        if (BtmIsControllerSupportedLeSetPrivacyMode()) {
            BtmSetPrivacyMode(device);
        }

        BtmEnableAddressResolution();

        BtmStartAutoConnection();
    }

    MutexUnlock(g_lePairedDevicesLock);
}

void BTM_RemoveLePairedDevice(const BtAddr *addr)
{
    if (addr == NULL) {
        return;
    }

    if (!IS_INITIALIZED()) {
        return;
    }

    MutexLock(g_lePairedDevicesLock);

    if (BTM_IsControllerSupportLlPrivacy()) {
        BtmDisableAddressResolution();
    }

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByAddress(addr);
    if (block != NULL) {
        if (block->inResolvingList) {
            BtmRemoveFromResolvingList(&block->pairedInfo);
            g_deviceCountInResolvingList--;
        }
        ListRemoveNode(g_lePairedDevices, block);
    }

    if (BTM_IsControllerSupportLlPrivacy()) {
        BtmEnableAddressResolution();
    }

    MutexUnlock(g_lePairedDevicesLock);
}

void BTM_SetLocalIdentityResolvingKey(const BtmKey *key)
{
    if (key == NULL) {
        return;
    }

    if (!IS_INITIALIZED()) {
        return;
    }

    g_localIdentityResolvingKey = *key;
}

int BTM_GetLocalIdentityResolvingKey(BtmKey *irk)
{
    if (irk == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    *irk = g_localIdentityResolvingKey;
    return BT_NO_ERROR;
}

int BTM_GetRemoteIdentityResolvingKey(const BtAddr *addr, BtmKey *irk)
{
    if (addr == NULL || irk == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;
    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByAddress(addr);
    if (block != NULL) {
        *irk = block->pairedInfo.remoteIdentityResolvingKey;
    } else {
        result = BT_BAD_STATUS;
    }

    MutexUnlock(g_lePairedDevicesLock);
    return result;
}

int BTM_GetAllRemoteIdentityResolvingKey(BtmIdentityResolvingKey **irks, uint16_t *count)
{
    if (irks == NULL || count == 0) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_lePairedDevicesLock);

    uint8_t devCount = ListGetSize(g_lePairedDevices);
    if (devCount) {
        BtmIdentityResolvingKey *devIrks = MEM_MALLOC.alloc(sizeof(BtmIdentityResolvingKey) * devCount);
        if (devIrks != NULL) {
            uint16_t index = 0;
            BtmLePairedDeviceBlock *block = NULL;
            ListNode *node = ListGetFirstNode(g_lePairedDevices);
            while (node != NULL) {
                block = ListGetNodeData(node);
                devIrks[index].addr = block->pairedInfo.addr;
                devIrks[index].irk = block->pairedInfo.remoteIdentityResolvingKey;
                node = ListGetNextNode(node);
                index++;
            }

            *irks = devIrks;
            *count = devCount;
        }
    } else {
        *irks = NULL;
        *count = 0;
    }

    MutexUnlock(g_lePairedDevicesLock);
    return BT_NO_ERROR;
}

int BTM_GetAllPairedDevices(BtmLePairedDevice **devices, uint16_t *count)
{
    if ((devices == NULL) || (count == NULL)) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_lePairedDevicesLock);

    uint8_t devCount = ListGetSize(g_lePairedDevices);
    if (devCount) {
        BtmLePairedDevice *devices_ = MEM_MALLOC.alloc(sizeof(BtmLePairedDevice) * devCount);
        if (devices_ != NULL) {
            uint16_t index = 0;
            BtmLePairedDeviceBlock *block = NULL;
            ListNode *node = ListGetFirstNode(g_lePairedDevices);
            while (node != NULL) {
                block = ListGetNodeData(node);
                devices_[index] = block->pairedInfo;
                node = ListGetNextNode(node);
                index++;
            }

            *devices = devices_;
            *count = devCount;
        }
    } else {
        *devices = NULL;
        *count = 0;
    }

    MutexUnlock(g_lePairedDevicesLock);

    return BT_NO_ERROR;
}

int BTM_UpdateCurrentRemoteAddress(const BtAddr *pairedAddr, const BtAddr *currentAddr)
{
    if (pairedAddr == NULL || currentAddr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByAddress(pairedAddr);
    if (block != NULL) {
        block->currentAddr = *currentAddr;
    } else {
        result = BT_BAD_STATUS;
    }

    MutexUnlock(g_lePairedDevicesLock);

    return result;
}

int BTM_GetCurrentRemoteAddress(const BtAddr *pairedAddr, BtAddr *currentAddr)
{
    if (pairedAddr == NULL || currentAddr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByAddress(pairedAddr);
    if (block != NULL) {
        *currentAddr = block->currentAddr;
    } else {
        result = BT_BAD_STATUS;
    }

    MutexUnlock(g_lePairedDevicesLock);

    return result;
}

void BTM_SetOwnAddressType(uint32_t ownAddressType)
{
    MutexLock(g_ownAddressTypeLock);
    g_ownAddressType = ownAddressType;
    MutexUnlock(g_ownAddressTypeLock);
}

uint8_t BTM_GetOwnAddressType()
{
    MutexLock(g_ownAddressTypeLock);
    uint8_t type = g_ownAddressType;
    MutexUnlock(g_ownAddressTypeLock);
    return type;
}

int BTM_GetRemoteIdentityAddress(const BtAddr *addr, BtAddr *identityAddress)
{
    if (addr == NULL && identityAddress == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByAddress(addr);
    if (block != NULL) {
        *identityAddress = block->pairedInfo.remoteIdentityAddress;
    } else {
        result = BT_BAD_STATUS;
    }

    MutexUnlock(g_lePairedDevicesLock);

    return result;
}

int BTM_GetPairdAddressFromRemoteIdentityAddress(const BtAddr *identityAddress, BtAddr *addr)
{
    if (addr == NULL || identityAddress == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByRemoteIdentityAddress(identityAddress);
    if (block != NULL) {
        *addr = block->pairedInfo.addr;
    } else {
        result = BT_BAD_STATUS;
    }

    MutexUnlock(g_lePairedDevicesLock);

    return result;
}

bool BTM_IsDeviceInResolvingList(const BtAddr *addr)
{
    if (addr == NULL) {
        return false;
    }

    if (!IS_INITIALIZED()) {
        return false;
    }

    bool inResolvingList = false;

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = BtmFindLePairedDeviceBlockByAddress(addr);
    if (block != NULL) {
        inResolvingList = block->inResolvingList;
    }

    MutexUnlock(g_lePairedDevicesLock);

    return inResolvingList;
}

int BTM_ConvertToPairedAddress(const BtAddr *addr, BtAddr *pairedAddress)
{
    if (addr == NULL || pairedAddress == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    int result = BT_BAD_STATUS;

    MutexLock(g_lePairedDevicesLock);

    BtmLePairedDeviceBlock *block = NULL;

    ListNode *node = ListGetFirstNode(g_lePairedDevices);
    while (node != NULL) {
        block = ListGetNodeData(node);
        if (IsSameBtAddr(&block->currentAddr, addr)) {
            *pairedAddress = block->pairedInfo.addr;
            result = BT_NO_ERROR;
            break;
        } else if (IsSameBtAddr(&block->pairedInfo.remoteIdentityAddress, addr)) {
            *pairedAddress = block->pairedInfo.addr;
            result = BT_NO_ERROR;
            break;
        } else if (SMP_ResolveRPA(addr->addr, block->pairedInfo.remoteIdentityResolvingKey.key) ==
                   SMP_RESOLVE_RPA_RESULT_YES) {
            *pairedAddress = block->pairedInfo.addr;
            result = BT_NO_ERROR;
            break;
        }

        node = ListGetNextNode(node);
    }

    MutexUnlock(g_lePairedDevicesLock);

    return result;
}

int BTM_SetLeRandomAddress(const BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    BtmStopAutoConnection();

    HciLeSetRandomAddressParam param = {
        .randomAddess = {0},
    };
    (void)memcpy_s(param.randomAddess, BT_ADDRESS_SIZE, addr->addr, BT_ADDRESS_SIZE);

    int result = HCI_LeSetRandomAddress(&param);
    if (result == BT_NO_ERROR) {
        MutexLock(g_randomAddressLock);
        g_randomAddress = *addr;
        MutexUnlock(g_randomAddressLock);
    }

    BtmStartAutoConnection();
    return result;
}

int BTM_GetLeRandomAddress(BtAddr *addr)
{
    if (addr == NULL) {
        return BT_BAD_PARAM;
    }

    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_randomAddressLock);
    *addr = g_randomAddress;
    MutexUnlock(g_randomAddressLock);
    return BT_NO_ERROR;
}