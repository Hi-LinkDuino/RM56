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

#include "hc_init_protection.h"
#include <stdbool.h>
#include <stdlib.h>
#include "hc_log.h"
#include "hc_thread.h"
#include "ohos_init.h"

static HcMutex g_countMutex;
static int g_singleCount = 0;

int CheckInit(void)
{
    g_countMutex.lock(&g_countMutex);
    if (g_singleCount < 0) {
        g_singleCount = 0;
    } else if (g_singleCount > 0) {
        g_singleCount++;
        g_countMutex.unlock(&g_countMutex);
        return FINISH_INIT;
    }
    g_countMutex.unlock(&g_countMutex);
    return CONTINUE_INIT;
}

int CheckDestroy(void)
{
    g_countMutex.lock(&g_countMutex);
    if (g_singleCount == 0) {
        g_countMutex.unlock(&g_countMutex);
        return FINISH_DESTROY;
    }
    if (g_singleCount < 0) {
        g_singleCount = 0;
        g_countMutex.unlock(&g_countMutex);
        return FINISH_DESTROY;
    }
    if (g_singleCount > 1) {
        g_singleCount--;
        g_countMutex.unlock(&g_countMutex);
        return FINISH_DESTROY;
    }
    g_countMutex.unlock(&g_countMutex);
    return CONTINUE_DESTROY;
}

void SetInitStatus(void)
{
    g_countMutex.lock(&g_countMutex);
    g_singleCount = 1;
    g_countMutex.unlock(&g_countMutex);
    return;
}

void SetDeInitStatus(void)
{
    g_countMutex.lock(&g_countMutex);
    g_singleCount = 0;
    g_countMutex.unlock(&g_countMutex);
    return;
}

void InitSecurityDevAuth(void)
{
    int32_t res = InitHcMutex(&g_countMutex);
    if (res != 0) {
        LOGE("When init security auth, init mutex failed");
    }
}
SYS_SERVICE_INIT(InitSecurityDevAuth);

void DestroySecurityDevAuth(void)
{
    DestroyHcMutex(&g_countMutex);
}