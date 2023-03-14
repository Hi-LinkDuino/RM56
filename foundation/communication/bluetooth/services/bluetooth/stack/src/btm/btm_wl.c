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

#include "btm_wl.h"

#include <securec.h>

#include "hci/hci.h"
#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#include "btm.h"
#include "btm_controller.h"

#define STATUS_NONE 0
#define STATUS_INITIALIZED 1

#define IS_INITIALIZED() (g_status == STATUS_INITIALIZED)

typedef struct {
    uint8_t addressType;
    BtAddr address;
} BtmWhiteListEntity;

static List *g_whiteList = NULL;
static Mutex *g_whiteListLock = NULL;
static uint8_t g_whiteListSize = 0;

static uint8_t g_status = STATUS_NONE;

static BtmWhiteListEntity *BtmAllocWhiteListEntity(uint8_t addressType, const BtAddr *address)
{
    BtmWhiteListEntity *entity = MEM_MALLOC.alloc(sizeof(BtmWhiteListEntity));
    if (entity != NULL) {
        entity->addressType = addressType;
        entity->address = *address;
    }
    return entity;
}

static void BtmFreeWhiteListEntity(void *entity)
{
    MEM_MALLOC.free(entity);
}

static BtmWhiteListEntity *BtmFindWhiteListEntityByType(uint8_t addressType)
{
    BtmWhiteListEntity *entity = NULL;

    ListNode *node = ListGetFirstNode(g_whiteList);
    while (node != NULL) {
        entity = ListGetNodeData(node);
        if (entity->addressType == addressType) {
            break;
        } else {
            entity = NULL;
        }
        node = ListGetNextNode(node);
    }

    return entity;
}

static bool IsSameAddress(const BtAddr *addr1, const BtAddr *addr2)
{
    bool isSame = true;
    for (uint8_t i = 0; i < BT_ADDRESS_SIZE; i++) {
        if (addr1->addr[i] != addr2->addr[i]) {
            isSame = false;
        }
    }
    return isSame;
}

static BtmWhiteListEntity *BtmFindWhiteListEntity(uint8_t addressType, const BtAddr *address)
{
    BtmWhiteListEntity *entity = NULL;

    ListNode *node = ListGetFirstNode(g_whiteList);
    while (node != NULL) {
        entity = ListGetNodeData(node);
        if ((entity->addressType == addressType) && IsSameAddress(&entity->address, address)) {
            break;
        } else {
            entity = NULL;
        }
        node = ListGetNextNode(node);
    }

    return entity;
}

int BtmAddDeviceToWhiteList(uint8_t addressType, const BtAddr *address)
{
    if (!IS_INITIALIZED()) {
        return BT_BAD_STATUS;
    }

    MutexLock(g_whiteListLock);
    BtmWhiteListEntity *entity = NULL;
    if (addressType == WHITE_LIST_ADDRESS_TYPE_ANONYMOUS_ADVERTISEMENTS) {
        entity = BtmFindWhiteListEntityByType(WHITE_LIST_ADDRESS_TYPE_ANONYMOUS_ADVERTISEMENTS);
        if (entity != NULL) {
            MutexUnlock(g_whiteListLock);
            return BT_BAD_STATUS;
        }
    } else {
        entity = BtmFindWhiteListEntity(addressType, address);
        if (entity != NULL) {
            MutexUnlock(g_whiteListLock);
            return BT_BAD_STATUS;
        }
    }

    if (ListGetSize(g_whiteList) >= g_whiteListSize) {
        MutexUnlock(g_whiteListLock);
        return BT_OPERATION_FAILED;
    }

    entity = BtmAllocWhiteListEntity(addressType, address);
    if (entity == NULL) {
        MutexUnlock(g_whiteListLock);
        return BT_NO_MEMORY;
    }

    ListAddLast(g_whiteList, entity);

    MutexUnlock(g_whiteListLock);

    HciLeAddDeviceToWhiteListParam cmdParam = {
        .addressType = addressType,
        .address =
            {
                .raw = {0},
            },
    };
    (void)memcpy_s(cmdParam.address.raw, BT_ADDRESS_SIZE, address, BT_ADDRESS_SIZE);

    HCI_LeAddDeviceToWhiteList(&cmdParam);

    return BT_NO_ERROR;
}

void BtmRemoveDeviceFromWhiteList(uint8_t addressType, const BtAddr *address)
{
    if (!IS_INITIALIZED()) {
        return;
    }

    bool removed = false;

    MutexLock(g_whiteListLock);

    BtmWhiteListEntity *entity = NULL;
    ListNode *node = ListGetFirstNode(g_whiteList);
    while (node != NULL) {
        entity = ListGetNodeData(node);
        if (entity->addressType == addressType) {
            if (addressType == WHITE_LIST_ADDRESS_TYPE_ANONYMOUS_ADVERTISEMENTS) {
                ListRemoveNode(g_whiteList, entity);
                removed = true;
                break;
            }
            if (IsSameAddress(&entity->address, address)) {
                ListRemoveNode(g_whiteList, entity);
                removed = true;
                break;
            }
        }
        node = ListGetNextNode(node);
    }

    MutexUnlock(g_whiteListLock);

    if (removed) {
        HciLeRemoveDeviceFromWhiteListParam cmdParam = {
            .addressType = addressType,
            .address =
                {
                    .raw = {0},
                },
        };
        (void)memcpy_s(cmdParam.address.raw, BT_ADDRESS_SIZE, address, BT_ADDRESS_SIZE);
        HCI_LeRemoveDeviceFromWhiteList(&cmdParam);
    }
}

void BtmClearWhiteList()
{
    if (!IS_INITIALIZED()) {
        return;
    }

    MutexLock(g_whiteListLock);

    ListClear(g_whiteList);

    MutexUnlock(g_whiteListLock);

    HCI_LeClearWhiteList();
}

int BtmGetDeviceCountInWhiteList()
{
    if (!IS_INITIALIZED()) {
        return 0;
    }

    MutexLock(g_whiteListLock);

    int deviceCount = ListGetSize(g_whiteList);

    MutexUnlock(g_whiteListLock);

    return deviceCount;
}

void BtmInitWhiteList()
{
    g_whiteList = ListCreate(BtmFreeWhiteListEntity);
    g_whiteListLock = MutexCreate();

    g_status = STATUS_INITIALIZED;
}

void BtmCloseWhiteList()
{
    g_status = STATUS_NONE;

    if (g_whiteListLock != NULL) {
        MutexDelete(g_whiteListLock);
        g_whiteListLock = NULL;
    }

    if (g_whiteList != NULL) {
        ListDelete(g_whiteList);
        g_whiteList = NULL;
    }
}

void BtmStartWhiteList()
{
    uint8_t whiteListSize = 0;
    int result = BtmGetWhiteListSize(&whiteListSize);
    if (result == BT_NO_ERROR) {
        g_whiteListSize = whiteListSize;
    }

    HCI_LeClearWhiteList();
}

void BtmStopWhiteList()
{
    MutexLock(g_whiteListLock);
    ListClear(g_whiteList);
    MutexUnlock(g_whiteListLock);
}

bool BtmIsDeviceInWhiteList(uint8_t addressType, const BtAddr *address)
{
    bool inWhiteList = false;
    MutexLock(g_whiteListLock);
    BtmWhiteListEntity *entity = NULL;
    ListNode *node = ListGetFirstNode(g_whiteList);
    while (node != NULL) {
        entity = ListGetNodeData(node);
        if (entity != NULL && (entity->addressType == addressType) && IsSameAddress(&entity->address, address)) {
            inWhiteList = true;
            break;
        }
        node = ListGetNextNode(node);
    }
    MutexUnlock(g_whiteListLock);
    return inWhiteList;
}