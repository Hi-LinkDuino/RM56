/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef HKS_FILE_OPERATOR_H
#define HKS_FILE_OPERATOR_H

#include "hks_type.h"

#define HKS_MAX_FILE_NAME_LEN 512

#ifdef _STORAGE_LITE_
    #define HKS_KEY_STORE_PATH                HKS_CONFIG_KEY_STORE_PATH
#else
    #ifdef L2_STANDARD
        #define HKS_KEY_STORE_PATH            "/data/data/maindata"
        #define HKS_KEY_STORE_BAK_PATH        "/data/data/bakdata"
    #else
        #define HKS_KEY_STORE_PATH            "/storage/maindata"
        #define HKS_KEY_STORE_BAK_PATH        "/storage/bakdata"
    #endif
#endif
#define HKS_KEY_STORE_KEY_PATH        "key"
#define HKS_KEY_STORE_CERTCHAIN_PATH  "certchain"
#define HKS_KEY_STORE_ROOT_KEY_PATH   "info"

#define HKS_PROCESS_INFO_LEN    128
#define HKS_MAX_DIRENT_FILE_LEN 128
struct HksFileDirentInfo {
    char fileName[HKS_MAX_DIRENT_FILE_LEN]; /* point to dirent->d_name */
};

enum HksStoragePathType {
    HKS_STORAGE_MAIN_PATH,
    HKS_STORAGE_BACKUP_PATH,
};

#ifdef __cplusplus
extern "C" {
#endif

uint32_t HksFileRead(const char *path, const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len);

int32_t HksFileWrite(const char *path, const char *fileName, uint32_t offset, const uint8_t *buf, uint32_t len);

int32_t HksFileRemove(const char *path, const char *fileName);

uint32_t HksFileSize(const char *path, const char *fileName);

int32_t HksIsFileExist(const char *path, const char *fileName);

int32_t HksIsDirExist(const char *path);

int32_t HksMakeDir(const char *path);

void *HksOpenDir(const char *path);

int32_t HksCloseDir(void *dirp);

int32_t HksGetDirFile(void *dirp, struct HksFileDirentInfo *direntInfo);

int32_t HksRemoveDir(const char *dirPath);

int32_t HksDeleteDir(const char *dirPath);

/* return < 0 error; > 0 realFileSize; == 0 no data */
int32_t HksOldVersionFileRead(const char *fileName, uint32_t offset, uint8_t *buf, uint32_t len);

int32_t HksOldVersionFileRemove(const char *fileName);

/* return < 0 error; >= 0 realFileSize */
int32_t HksOldVersionFileSize(const char *fileName);

/* return true, exist; false not exist */
bool HksOldVersionIsFileExist(const char *fileName);

int32_t HksGetStoragePath(enum HksStoragePathType pathType, char *path, uint32_t *len);

int32_t HksGetFileName(const char *path, const char *fileName, char *fullFileName, uint32_t fullFileNameLen);

#ifdef __cplusplus
}
#endif

#endif /* HKS_FILE_OPERATOR_H */
