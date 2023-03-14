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

#include "device_token_proxy.h"
#include "osal_mem.h"

static void HdfDevTokenProxyConstruct(struct HdfDevTokenProxy *inst, struct HdfRemoteService *remote)
{
    inst->remote = remote;
}

struct HdfDevTokenProxy *HdfDevTokenProxyObtain(struct HdfRemoteService *remote)
{
    struct HdfDevTokenProxy *instance =
        (struct HdfDevTokenProxy *)OsalMemCalloc(sizeof(struct HdfDevTokenProxy));
    if (instance != NULL) {
        HdfDevTokenProxyConstruct(instance, remote);
    }
    return instance;
}

void HdfDevTokenProxyRecycle(struct HdfDevTokenProxy *inst)
{
    if (inst != NULL) {
        if (inst->remote != NULL) {
            HdfRemoteServiceRecycle(inst->remote);
            inst->remote = NULL;
        }
        OsalMemFree(inst);
    }
}

