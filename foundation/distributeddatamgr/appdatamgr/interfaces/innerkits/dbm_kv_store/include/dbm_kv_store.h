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

/**
 * @addtogroup dbm_kv_store
 * @{
 *
 * @brief Provides functions for obtaining, setting, and deleting a key-value pair.
 *
 *
 * Key-value pairs can be permanently stored in the file system. \n
 * For details about the number of key-value pairs that can be stored in an application, see {@link DBM_MAX_KV_SUM}. \n
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file dbm_kv_store.h
 *
 * @brief Provides functions for obtaining, setting, and deleting a key-value pair.
 *        Files with a _DBM_KV suffix are system files and should not be used.
 *        pleals file len and file system limit.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef DBM_KV_STORE_H
#define DBM_KV_STORE_H

#include "dbm_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct DBM *KVStoreHandle;
int DBM_GetKVStore(const char* storeFullPath, KVStoreHandle* kvStore);

int DBM_Get(KVStoreHandle db, const char* key, void* value, unsigned int count, unsigned int* realValueLen);
int DBM_Put(KVStoreHandle db, const char* key, const void* value, unsigned int len);
int DBM_Delete(KVStoreHandle db, const char* key);

int DBM_CloseKVStore(KVStoreHandle db);
int DBM_DeleteKVStore(const char* storeFullPath);

void DBM_KVStoreControl(KVStoreHandle db, int cmd, void* arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // DBM_KV_STORE_H
/** @} */
