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

#ifndef OHOS_DHCP_OPTION_H
#define OHOS_DHCP_OPTION_H

#include <stdint.h>
#include <stdint.h>
#include <stddef.h>

#define DHCP_OPTION_SIZE 256
#define DHCP_END_OPTION_CODE 255

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DhcpOption DhcpOption;
struct DhcpOption {
    uint8_t code;
    uint8_t length;
    uint8_t data[DHCP_OPTION_SIZE];
};
typedef struct DhcpOption *PDhcpOption;

typedef struct DhcpOptionNode DhcpOptionNode;
struct DhcpOptionNode {
    struct DhcpOptionNode *previous;
    struct DhcpOptionNode *next;
    DhcpOption option;
};
typedef struct DhcpOptionNode *PDhcpOptionNode;

typedef struct DhcpOptionList DhcpOptionList;
struct DhcpOptionList {
    PDhcpOptionNode first;
    PDhcpOptionNode last;
    size_t size;
};
typedef struct DhcpOptionList *PDhcpOptionList;

int InitOptionList(PDhcpOptionList pOptions);
int HasInitialized(PDhcpOptionList pOptions);
int PushBackOption(PDhcpOptionList pOptions, PDhcpOption option);
int PushFrontOption(PDhcpOptionList pOptions, PDhcpOption option);
int RemoveOption(PDhcpOptionList pOptions, uint8_t code);
PDhcpOptionNode GetOptionNode(PDhcpOptionList pOptions, uint8_t code);
PDhcpOption GetOption(PDhcpOptionList pOptions, uint8_t code);
void ClearOptions(PDhcpOptionList pOptions);
void FreeOptionList(PDhcpOptionList pOptions);
int FillOption(PDhcpOption pOption, const char *data, size_t len);
int FillU32Option(PDhcpOption pOption, uint32_t u32);
int FillOptionData(PDhcpOption pOption, const uint8_t *data, size_t len);
int AppendAddressOption(PDhcpOption pOption, uint32_t address);

#ifdef __cplusplus
}
#endif
#endif