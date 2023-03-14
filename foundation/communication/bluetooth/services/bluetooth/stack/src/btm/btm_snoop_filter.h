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
#ifndef BTM_SNOOP_FILTER_H
#define BTM_SNOOP_FILTER_H

#include <stdint.h>

#include "btstack.h"

#include "platform/include/list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HCI_LINK_CONTROL_COMMAND_MAX_NUM 0x0045
#define HCI_LINK_POLICY_COMMAND_MAX_NUM 0x0011
#define HCI_CONTTOLLER_AND_BASEBAND_COMMAND_MAX_NUM 0x0081
#define HCI_INFORMATIONAL_PARAMETERS_COMMAND_MAX_NUM 0x000B
#define HCI_STATUS_PARAMETERS_COMMAND_MAX_NUM 0x000D
#define HCI_LE_CONTROLLER_COMMAND_MAX_NUM 0x004E

#define HCI_EVENT_MAX_NUM 0x58
#define HCI_LE_EVENT_MAX_NUM (0x14 + 1)

typedef void (*BtmFilterFunc)(uint8_t *param);

typedef struct {
    uint8_t module;
    uint8_t rfcommScn;
    uint16_t l2capPsm;
    bool isLocal;
    BtAddr remoteAddr;
} BtmSnoopFilterInfo;

void BtmInitSnoopFilter(void);
void BtmCloseSnoopFilter(void);
void BtmEnableSnoopFilter(void);
void BtmDisableSnoopFilter(void);
void BtmHciFilter(uint8_t type, const uint8_t **data, uint16_t originalLength, uint16_t *includedLength);

List *BtmGetFilterInfoList(void);
bool BtmFindFilterInfoByInfoUsePsm(void *nodeData, void *info);
bool BtmFindFilterInfoByInfoUseScn(void *nodeData, void *info);
void BtmChangeIncludeLength(uint16_t *includedLength, uint16_t len);
uint8_t *BtmCreateFilterBuffer(const uint16_t *includedLength, const uint8_t *data);
void BtmFilterData(uint8_t *data, uint8_t length);
void BtmFilterAddress(uint8_t *addr, uint8_t length);

void BtmFilterHciCmd(const uint8_t **data, uint16_t originalLength, uint16_t *includedLength);
void BtmFilterHciCmdCompleteEvt(const uint8_t **data, uint16_t originalLength, uint16_t *includedLength);

void HciEvtFilter(const uint8_t **data, uint16_t originalLength, uint16_t *includedLength);

void BtmEnableSnoopFilterAcl(void);
void BtmDisableSnoopFilterAcl(void);
void BtmFilterCheckAndSaveAclConnInfo(const uint8_t **data, uint16_t originalLength);
void BtmFilterAclData(uint8_t type, const uint8_t **data, uint16_t originalLength, uint16_t *includedLength);

#ifdef __cplusplus
}
#endif

#endif