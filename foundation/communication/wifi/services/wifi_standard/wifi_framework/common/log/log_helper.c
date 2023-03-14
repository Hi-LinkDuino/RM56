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

#include "securec.h"
#include "log_helper.h"

const int MAX_REMAIN_CHARACTER_NUM = 3;

void EncryptLogMsg(const char *srcMsg, char *desMsg, int desLen)
{
    if (desMsg == NULL || desLen <= 0) {
        return;
    }
    if (srcMsg == NULL || strlen(srcMsg) == 0) {
        desMsg[0] = '\0';
        return;
    }
    int srcLen = strlen(srcMsg);
    if (strncpy_s(desMsg, desLen, srcMsg, srcLen) != EOK) {
        desMsg[0] = '\0';
        return;
    }
    desMsg[desLen - 1] = '\0';
    int i = 0;
    int j = (srcLen > (desLen - 1)) ? (desLen - 1) : srcLen;
    j -= 1;
    int k = MAX_REMAIN_CHARACTER_NUM;
    while (k > 0 && i < j) {
        ++i;
        --j;
        --k;
    }
    if (k == 0) {
        if (i > j) {
            desMsg[i] = '*';
            desMsg[j] = '*';
        }
        while (i <= j) {
            desMsg[i] = '*';
            ++i;
        }
    } else {
        if (i > j) {
            desMsg[i] = '*';
            desMsg[j] = '*';
        } else {
            desMsg[i] = '*';
        }
    }
    return;
}