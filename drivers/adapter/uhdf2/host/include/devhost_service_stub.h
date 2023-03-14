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

#ifndef DEVHOST_SERVICE_STUB_H
#define DEVHOST_SERVICE_STUB_H

#include "hdf_remote_service.h"
#include "devhost_service_full.h"
#include "osal_mutex.h"

struct DevHostServiceStub {
    struct DevHostServiceFull super;
    struct HdfRemoteService *remote;
    struct OsalMutex hostSvcMutex;
};

struct HdfObject *DevHostServiceStubCreate(void);
void DevHostServiceStubRelease(struct HdfObject *object);

#endif /* DEVHOST_SERVICE_STUB_H */
