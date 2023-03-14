/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "protocol_common.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "string_util.h"

void FreeAndCleanKey(Uint8Buff *key)
{
    if (key == NULL || key->val == NULL) {
        return;
    }
    (void)memset_s(key->val, key->length, 0, key->length);
    HcFree(key->val);
    key->val = NULL;
    key->length = 0;
}

int32_t InitSingleParam(Uint8Buff *param, uint32_t len)
{
    if (param == NULL) {
        LOGE("Param is null.");
        return HC_ERR_NULL_PTR;
    }
    if (param->val != NULL) {
        (void)memset_s(param->val, param->length, 0, param->length);
        HcFree(param->val);
    }
    param->length = len;
    param->val = (uint8_t *)HcMalloc(param->length, 0);
    if (param->val == NULL) {
        LOGE("Malloc for param failed.");
        return HC_ERR_ALLOC_MEMORY;
    }
    return HC_SUCCESS;
}