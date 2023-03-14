/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/* This function is not open for user now */
#include "dbm_kv_store.h"

#include "dbm_errno.h"

#ifdef DBM_DEBUG_ENABLE
#define DBM_LOG_TAG "[low_posix][kv]"
#endif

int DBM_GetKVStore(const char* storeFullPath, KVStoreHandle* db)
{
    return DBM_NOT_SUPPORT;
}

int DBM_Get(KVStoreHandle db, const char* key, void* value, unsigned int count, unsigned int* realValueLen)
{
    return DBM_NOT_SUPPORT;
}
int DBM_Put(KVStoreHandle db, const char* key, const void* value, unsigned int len)
{
    return DBM_NOT_SUPPORT;
}
int DBM_Delete(KVStoreHandle db, const char* key)
{
    return DBM_NOT_SUPPORT;
}

int DBM_CloseKVStore(KVStoreHandle db)
{
    return DBM_NOT_SUPPORT;
}
int DBM_DeleteKVStore(const char* storeFullPath)
{
    return DBM_NOT_SUPPORT;
}

void DBM_KVStoreControl(KVStoreHandle db, int cmd, void* arg)
{
}