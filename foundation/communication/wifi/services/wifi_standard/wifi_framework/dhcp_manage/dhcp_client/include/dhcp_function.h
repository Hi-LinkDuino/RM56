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
#ifndef OHOS_FUNCTION_H
#define OHOS_FUNCTION_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

bool Ip4StrConToInt(const char *strIp, uint32_t *uIp, bool bHost);
char *Ip4IntConToStr(uint32_t uIp, bool bHost);
bool Ip6StrConToChar(const char *strIp, uint8_t chIp[], size_t chlen);
const char *MacChConToMacStr(const unsigned char *pChMac, size_t chLen, char *pStrMac, size_t strLen);
int GetLocalInterface(const char *ifname, int *ifindex, unsigned char *hwaddr, uint32_t *ifaddr4);
int GetLocalIp(const char *ifname, uint32_t *ifaddr4);
int SetLocalInterface(const char *ifname, uint32_t ifaddr4);
int InitPidfile(const char *pidDir, const char *pidFile, pid_t pid);
pid_t GetPID(const char *pidFile);
int CreateDirs(const char *dirs, int mode);

#ifdef __cplusplus
}
#endif
#endif
