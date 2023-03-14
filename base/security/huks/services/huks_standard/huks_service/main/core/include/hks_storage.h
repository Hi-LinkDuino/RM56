/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HKS_STORAGE_H
#define HKS_STORAGE_H

#include "hks_type_inner.h"

enum HksStorageType {
    HKS_STORAGE_TYPE_KEY = 0,
    HKS_STORAGE_TYPE_CERTCHAIN,
    HKS_STORAGE_TYPE_ROOT_KEY,
};

#ifdef __cplusplus
extern "C" {
#endif

struct HksStoreInfo {
    char *processPath; /* file path include process */
    char *path; /* file path include process/key(or certchain) */
    char *fileName; /* file name that can be recognized by the file system */
    uint32_t size;
};

struct HksStoreFileInfo {
    struct HksStoreInfo mainPath;
#ifdef SUPPORT_STORAGE_BACKUP
    struct HksStoreInfo bakPath;
#endif
};

#ifndef _CUT_AUTHENTICATE_
#ifdef _STORAGE_LITE_
int32_t HksStoreKeyBlob(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    enum HksStorageType storageType, const struct HksBlob *keyBlob);

int32_t HksStoreDeleteKeyBlob(const struct HksProcessInfo *processInfo,
    const struct HksBlob *keyAlias, uint32_t storageType);
int32_t HksStoreIsKeyBlobExist(const struct HksProcessInfo *processInfo,
    const struct HksBlob *keyAlias, uint32_t storageType);
int32_t HksStoreGetKeyBlob(const struct HksProcessInfo *processInfo,
    const struct HksBlob *keyAlias, uint32_t storageType, struct HksBlob *keyBlob);
int32_t HksStoreGetKeyBlobSize(const struct HksBlob *processName,
    const struct HksBlob *keyAlias, uint32_t storageType, uint32_t *keyBlobSize);
int32_t HksGetKeyCountByProcessName(const struct HksBlob *processName, uint32_t *keyCount);
#else // _STORAGE_LITE_

int32_t HksStoreKeyBlob(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    enum HksStorageType storageType, const struct HksBlob *keyBlob);

int32_t HksStoreDeleteKeyBlob(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    uint32_t storageType);

int32_t HksStoreIsKeyBlobExist(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    uint32_t storageType);

int32_t HksStoreGetKeyBlob(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    uint32_t storageType, struct HksBlob *keyBlob);

int32_t HksStoreGetKeyBlobSize(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias,
    uint32_t storageType, uint32_t *keyBlobSize);

int32_t HksGetKeyCountByProcessName(const struct HksProcessInfo *processInfo, uint32_t *keyCount);
#endif // _STORAGE_LITE_
#endif // _CUT_AUTHENTICATE_

int32_t HksGetKeyAliasByProcessName(const struct HksProcessInfo *processInfo, struct HksKeyInfo *keyInfoList,
    uint32_t *listCount);

int32_t HksStoreDestroy(const struct HksBlob *processName);

int32_t HksGetFileInfo(const struct HksProcessInfo *processInfo, const struct HksBlob *keyAlias, uint32_t storageType,
    struct HksStoreFileInfo *fileInfo);

int32_t HksServiceDeleteUserIDKeyAliasFile(const struct HksBlob processName);

int32_t HksServiceDeleteUIDKeyAliasFile(const struct HksProcessInfo processInfo);

#ifdef _STORAGE_LITE_

#define HKS_KEY_STORE_FILE_NAME "hks_keystore"

int32_t HksLoadFileToBuffer(void);

int32_t HksFileBufferRefresh(void);

int32_t HksStoreGetToatalSize(uint32_t *size);

int32_t HksStoreGetKeyInfoList(struct HksKeyInfo *keyInfoList, uint32_t *listCount);

#endif /* _STORAGE_LITE_ */

#ifdef __cplusplus
}
#endif

#endif /* HKS_STORAGE_H */