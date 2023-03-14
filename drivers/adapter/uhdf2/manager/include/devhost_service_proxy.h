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

#ifndef DEVHOST_SERVICE_PROXY_H
#define DEVHOST_SERVICE_PROXY_H

#include "devhost_service_if.h"
#include "hdf_remote_service.h"

struct DevHostServiceProxy {
    struct IDevHostService super;
    struct HdfRemoteService *remote;
    struct HdfDeathRecipient recipient;
    uint32_t hostId;
};

enum {
    DEVHOST_SERVICE_ADD_DEVICE = 1,
    DEVHOST_SERVICE_DEL_DEVICE,
};

struct IDevHostService *DevHostServiceProxyObtain(uint32_t hostId, struct HdfRemoteService *remote);
void DevHostServiceProxyRecycle(struct DevHostServiceProxy *inst);

#endif /* DEVHOST_SERVICE_PROXY_H */
