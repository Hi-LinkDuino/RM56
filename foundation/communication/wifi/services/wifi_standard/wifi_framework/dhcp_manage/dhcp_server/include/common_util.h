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

#ifndef OHOS_COMMON_UTIL_H
#define OHOS_COMMON_UTIL_H

#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif
void LogTime(void);
uint64_t Tmspsec(void);
uint64_t Tmspusec(void);
void LeftTrim(char *buf);
void RightTrim(char *buf);
void TrimString(char *buf);
const char *GetFilePath(const char *fileName);

const char *GetLeaseFile(const char *fileName, const char *ifname);
int CreatePath(const char *fileName);
int RemoveSpaceCharacters(char *buf, size_t bufSize);

#ifdef __cplusplus
}
#endif
#endif