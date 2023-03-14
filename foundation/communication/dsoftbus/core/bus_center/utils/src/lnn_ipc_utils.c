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

#include "lnn_ipc_utils.h"

#include <string.h>

void ConvertVoidToPublishInfo(const void *info, PublishInfo *pubInfo)
{
    char *info1 = (char *)info;
    pubInfo->publishId = *(int32_t *)info1;
    info1 += sizeof(int32_t);
    pubInfo->mode = *(DiscoverMode *)info1;
    info1 += sizeof(DiscoverMode);
    pubInfo->medium = *(ExchanageMedium *)info1;
    info1 += sizeof(ExchanageMedium);
    pubInfo->freq = *(ExchangeFreq *)info1;
    info1 += sizeof(ExchangeFreq);
    pubInfo->capability = (const char *)info1;
    info1 += strlen(pubInfo->capability) + 1;
    pubInfo->dataLen = *(int32_t *)info1;
    info1 += sizeof(int32_t);
    if (pubInfo->dataLen == 0) {
        return;
    }
    pubInfo->capabilityData = (unsigned char *)info1;
}

void ConvertVoidToSubscribeInfo(const void *info, SubscribeInfo *subInfo)
{
    char *info1 = (char *)info;
    subInfo->subscribeId = *(int32_t *)info1;
    info1 += sizeof(int32_t);
    subInfo->mode = *(DiscoverMode *)info1;
    info1 += sizeof(DiscoverMode);
    subInfo->medium = *(ExchanageMedium *)info1;
    info1 += sizeof(ExchanageMedium);
    subInfo->freq = *(ExchangeFreq *)info1;
    info1 += sizeof(ExchangeFreq);
    subInfo->isSameAccount = *(bool *)info1;
    info1 += sizeof(bool);
    subInfo->isWakeRemote = *(bool *)info1;
    info1 += sizeof(bool);
    subInfo->capability = (const char *)info1;
    info1 += strlen(subInfo->capability) + 1;
    subInfo->dataLen = *(int32_t *)info1;
    info1 += sizeof(int32_t);
    if (subInfo->dataLen == 0) {
        return;
    }
    subInfo->capabilityData = (unsigned char *)info1;
}