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

#include "btm_inq_db.h"

#include "platform/include/allocator.h"
#include "platform/include/list.h"
#include "platform/include/mutex.h"

#define INQUIRY_DB_MAX 40

typedef struct {
    BtAddr addr;
    BtmInquiryInfo inquiryInfo;
} BtmInquiryDBEntity;

static List *g_inquiryDb = NULL;
static Mutex *g_inquiryDbLock = NULL;

static BtmInquiryDBEntity *BtmAllocInquiryDBEntity(const BtAddr *addr, const BtmInquiryInfo *info)
{
    BtmInquiryDBEntity *entity = MEM_MALLOC.alloc(sizeof(BtmInquiryDBEntity));
    if (entity != NULL) {
        entity->addr = *addr;
        entity->inquiryInfo = *info;
    }
    return entity;
}

static void BtmFreeInquiryDBEntiry(void *entity)
{
    MEM_MALLOC.free(entity);
}

static bool BtmIsSameAddress(const BtAddr *addr1, const BtAddr *addr2)
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

static BtmInquiryDBEntity *BtmFindInquiryEntiryByAddress(const BtAddr *addr)
{
    BtmInquiryDBEntity *entity = NULL;
    ListNode *node = ListGetFirstNode(g_inquiryDb);
    while (node != NULL) {
        entity = ListGetNodeData(node);
        if (entity != NULL) {
            if (BtmIsSameAddress(addr, &entity->addr)) {
                break;
            }
            entity = NULL;
        }
        node = ListGetNextNode(node);
    }
    return entity;
}

int BtmAssignOrUpdateInquiryInfo(const BtAddr *addr, const BtmInquiryInfo *info)
{
    int result = BT_NO_ERROR;

    MutexLock(g_inquiryDbLock);

    BtmInquiryDBEntity *entity = BtmFindInquiryEntiryByAddress(addr);
    if (entity != NULL) {
        entity->inquiryInfo = *info;
    } else {
        entity = BtmAllocInquiryDBEntity(addr, info);
        if (entity != NULL) {
            if (ListGetSize(g_inquiryDb) >= INQUIRY_DB_MAX) {
                ListRemoveFirst(g_inquiryDb);
            }

            ListAddLast(g_inquiryDb, entity);
        }
    }

    MutexUnlock(g_inquiryDbLock);

    return result;
}

int BtmQueryInquiryInfoByAddr(const BtAddr *addr, BtmInquiryInfo *info)
{
    if ((addr == NULL) || (info == NULL)) {
        return BT_BAD_PARAM;
    }

    int result = BT_NO_ERROR;

    MutexLock(g_inquiryDbLock);

    BtmInquiryDBEntity *entity = BtmFindInquiryEntiryByAddress(addr);
    if (entity != NULL) {
        *info = entity->inquiryInfo;
    } else {
        result = BT_OPERATION_FAILED;
    }

    MutexUnlock(g_inquiryDbLock);

    return result;
}

void BtmClearInquiryDb()
{
    MutexLock(g_inquiryDbLock);

    ListClear(g_inquiryDb);

    MutexUnlock(g_inquiryDbLock);
}

void BtmInitInquiryDb()
{
    g_inquiryDb = ListCreate(BtmFreeInquiryDBEntiry);
    g_inquiryDbLock = MutexCreate();
}

void BtmCloseInquiryDb()
{
    if (g_inquiryDbLock != NULL) {
        MutexDelete(g_inquiryDbLock);
        g_inquiryDbLock = NULL;
    }

    if (g_inquiryDb != NULL) {
        ListDelete(g_inquiryDb);
        g_inquiryDb = NULL;
    }
}