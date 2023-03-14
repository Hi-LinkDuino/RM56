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
#ifndef OHOS_OPTIONS_H
#define OHOS_OPTIONS_H


#include <stdint.h>
#include <stdbool.h>

#include "dhcp_define.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t GetDhcpOptionDataLen(const uint8_t code);
const uint8_t *GetDhcpOption(const struct DhcpPacket *packet, int code, size_t *length);
bool GetDhcpOptionUint8(const struct DhcpPacket *packet, int code, uint8_t *data);
bool GetDhcpOptionUint32(const struct DhcpPacket *packet, int code, uint32_t *data);
bool GetDhcpOptionUint32n(const struct DhcpPacket *packet, int code, uint32_t *data1, uint32_t *data2);
char *GetDhcpOptionString(const struct DhcpPacket *packet, int code);
int GetEndOptionIndex(uint8_t *pOpts);
int AddOptStrToOpts(uint8_t *pOpts, uint8_t *pOpt, int nOptLen);
int AddOptValueToOpts(uint8_t *pOpts, uint8_t code, uint32_t value);

#ifdef __cplusplus
}
#endif
#endif
