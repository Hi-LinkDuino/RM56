/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef HC_FILE_H
#define HC_FILE_H

#include "hc_string_vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    void *pfd;
    int fd;
} FileHandle;

#define MODE_FILE_READ 0
#define MODE_FILE_WRITE 1

// 0 indicates success
// -1 indicates fail
int HcFileOpen(const char *path, int mode, FileHandle *file);
int HcFileSize(FileHandle file);
int HcFileRead(FileHandle file, void *dst, int dstSize);
int HcFileWrite(FileHandle file, const void *src, int srcSize);
void HcFileClose(FileHandle file);
void HcFileRemove(const char *path);
void HcFileGetSubFileName(const char *path, StringVector *nameVec);

#ifdef __cplusplus
}
#endif
#endif