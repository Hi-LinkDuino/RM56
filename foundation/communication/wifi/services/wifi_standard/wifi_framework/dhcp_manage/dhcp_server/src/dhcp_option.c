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

#include "dhcp_option.h"
#include <stdint.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "dhcp_define.h"
#include "dhcp_logger.h"
#include "securec.h"

#undef LOG_TAG
#define LOG_TAG "DhcpServerOption"

PDhcpOptionNode CreateOptionNode(PDhcpOption opt)
{
    if (!opt) {
        LOGE("input parameter is null.");
        return NULL;
    }
    DhcpOptionNode *pNode = (DhcpOptionNode *)calloc(1, sizeof(DhcpOptionNode));
    if (pNode == NULL) {
        LOGE("failed to create dhcp option node!");
        return NULL;
    }
    pNode->option.code = opt->code;
    pNode->option.length = opt->length;
    if (memcpy_s(pNode->option.data, sizeof(pNode->option.data), opt->data, opt->length) != EOK) {
        LOGE("create option node failed when memcpy opt data!");
        free(pNode);
        pNode = NULL;
        return NULL;
    }
    pNode->previous = pNode->next = 0;
    return pNode;
}

int HasInitialized(PDhcpOptionList pOptions)
{
    if (!pOptions) {
        LOGE("option list pointer is null.");
        return 0;
    }
    if (pOptions->first != NULL) {
        return 1;
    }
    return 0;
}

int InitOptionList(PDhcpOptionList pOptions)
{
    if (!pOptions) {
        return RET_ERROR;
    }
    if (pOptions->first != NULL && pOptions->first == pOptions->last) {
        return RET_SUCCESS;
    }

    DhcpOptionNode *pNode = (DhcpOptionNode *)calloc(1, sizeof(DhcpOptionNode));
    if (!pNode) {
        LOGE("failed to create dhcp option node!");
        return 1;
    }

    pOptions->size = 0;
    pOptions->first = pOptions->last = pNode;
    pOptions->first->previous = NULL;
    pOptions->last->next = NULL;
    return RET_SUCCESS;
}

int PushBackOption(PDhcpOptionList pOptions, PDhcpOption pOption)
{
    if (!pOptions) {
        LOGE("option list pointer is null.");
        return RET_ERROR;
    }
    if (!pOption) {
        LOGE("option pointer is null.");
        return RET_ERROR;
    }
    if (pOptions->first == NULL) {
        LOGE("option list not initialized");
        return RET_SUCCESS;
    }
    DhcpOptionNode *pNode = CreateOptionNode(pOption);
    if (!pNode) {
        LOGE("failed to crate option node.");
        return 1;
    }
    pNode->previous = pOptions->last;
    pOptions->last->next = pNode;
    pOptions->last = pNode;
    pOptions->size++;

    return RET_SUCCESS;
}

int PushFrontOption(PDhcpOptionList pOptions, PDhcpOption pOption)
{
    if (!pOptions) {
        LOGE("option list pointer is null.");
        return RET_ERROR;
    }
    if (!pOption) {
        LOGE("option pointer is null.");
        return RET_ERROR;
    }
    PDhcpOptionNode pNode = CreateOptionNode(pOption);
    if (!pNode) {
        return RET_FAILED;
    }

    if (pOptions->first == pOptions->last) {
        pNode->previous = pOptions->first;
        pOptions->first->next = pNode;
        pOptions->last = pNode;
    } else {
        pNode->next = pOptions->first->next;
        pNode->next->previous = pNode;
        pNode->previous = pOptions->first;
        pOptions->first->next = pNode;
    }
    pOptions->size++;

    return RET_SUCCESS;
}

int RemoveOption(PDhcpOptionList pOptions, uint8_t code)
{
    if (pOptions == NULL) {
        return RET_ERROR;
    }
    if (pOptions->size == 0) {
        return RET_FAILED;
    }
    DhcpOptionNode *pNode = GetOptionNode(pOptions, code);
    if (pNode == NULL) {
        return RET_FAILED;
    }
    if (pNode == pOptions->last) {
        pOptions->last = pNode->previous;
        pOptions->last->next = NULL;
    } else {
        pNode->next->previous = pNode->previous;
        pNode->previous->next = pNode->next;
    }
    pOptions->size--;
    free(pNode);
    pNode = NULL;
    return RET_SUCCESS;
}

PDhcpOptionNode GetOptionNode(PDhcpOptionList pOptions, uint8_t code)
{
    if (pOptions->first == NULL) {
        return NULL;
    }
    PDhcpOptionNode pNode = pOptions->first->next;
    while (pNode != NULL && pNode->option.code != code) {
        pNode = pNode->next;
    }
    return pNode;
}

PDhcpOption GetOption(PDhcpOptionList pOptions, uint8_t code)
{
    PDhcpOptionNode pNode = GetOptionNode(pOptions, code);
    if (pNode) {
        return &pNode->option;
    }
    return NULL;
}

void ClearOptions(PDhcpOptionList pOptions)
{
    if (pOptions == NULL || pOptions->size == 0) {
        return;
    }
    DhcpOptionNode *pNode = pOptions->first->next;
    while (pNode != NULL) {
        if (pNode == pOptions->last) {
            pOptions->last = pOptions->first;
            pOptions->last->next = NULL;
        } else {
            pNode->next->previous = pNode->previous;
            pNode->previous->next = pNode->next;
        }
        free(pNode);
        pNode = pOptions->first->next;
    }
    pNode = pOptions->first;
    pOptions->size = 0;
    pOptions->first = pOptions->last = pNode;
    pOptions->first->previous = NULL;
    pOptions->last->next = NULL;
}

void FreeOptionList(PDhcpOptionList pOptions)
{
    if (pOptions == NULL) {
        return;
    }
    if (pOptions->first == NULL) {
        return;
    }
    DhcpOptionNode *pNode = pOptions->first->next;
    while (pNode != NULL) {
        if (pNode == pOptions->last) {
            pOptions->last = pOptions->first;
            pOptions->last->next = NULL;
        } else {
            pNode->next->previous = pNode->previous;
            pNode->previous->next = pNode->next;
        }
        free(pNode);
        pNode = pOptions->first->next;
    }
    pOptions->size = 0;
    free(pOptions->first);
    pOptions->first = pOptions->last = NULL;
    return;
}

int FillOption(PDhcpOption pOption, const char *data, size_t len)
{
    if (!pOption) {
        return RET_ERROR;
    }
    if (!data) {
        return RET_FAILED;
    }
    size_t flen = len;
    if (flen > (DHCP_OPTION_SIZE - 1)) {
        flen = DHCP_OPTION_SIZE - 1;
    }
    if (memcpy_s(pOption->data, sizeof(pOption->data) - 1, data, flen) != EOK) {
        return RET_ERROR;
    }
    pOption->length = flen;
    return RET_SUCCESS;
}

int FillU32Option(PDhcpOption pOption, uint32_t u32)
{
    if (!pOption) {
        return RET_ERROR;
    }
    if (memcpy_s(pOption->data, sizeof(pOption->data), &u32, sizeof(uint32_t)) != EOK) {
        return RET_ERROR;
    }
    pOption->length = sizeof(uint32_t);
    return RET_SUCCESS;
}

int FillOptionData(PDhcpOption pOption, const uint8_t *data, size_t len)
{
    int flen = len;
    if (!pOption) {
        return RET_ERROR;
    }
    if (!data) {
        return RET_FAILED;
    }
    if (flen > (DHCP_OPTION_SIZE)) {
        flen = DHCP_OPTION_SIZE;
    }
    if (memcpy_s(pOption->data, sizeof(pOption->data), data, flen) != EOK) {
        return RET_ERROR;
    }
    pOption->length = flen;
    return RET_SUCCESS;
}

int AppendAddressOption(PDhcpOption pOption, uint32_t address)
{
    if (!pOption) {
        return RET_ERROR;
    }
    uint8_t addrLen = pOption->length;
    uint8_t *pData = pOption->data;
    int spaceSize = sizeof(pOption->data) - addrLen;
    if (spaceSize < DHCP_ADDRESS_LENGTH) {
        LOGE("failed to append address, not enough space for option data.");
        return RET_ERROR;
    }
    if ((int)addrLen > 0) {
        pData += addrLen;
    }
    if (memcpy_s(pData, spaceSize, &address, DHCP_ADDRESS_LENGTH) != EOK) {
        return RET_ERROR;
    }
    pOption->length += DHCP_ADDRESS_LENGTH;
    return RET_SUCCESS;
}
