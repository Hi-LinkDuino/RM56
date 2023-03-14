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

#ifndef DEVSVC_MANAGER_STUB_H
#define DEVSVC_MANAGER_STUB_H

#include "devsvc_manager.h"
#include "hdf_remote_service.h"

#define DEVICE_SERVICE_MANAGER "hdf_device_service_manager"
#define DEVICE_SERVICE_MANAGER_SA_ID 5100

struct DevSvcManagerStub {
    struct DevSvcManager super;
    struct HdfRemoteService *remote;
    struct HdfDeathRecipient recipient;
    bool started;
};

struct HdfObject *DevSvcManagerStubCreate(void);
void DevSvcManagerStubRelease(struct HdfObject *object);

#endif /* DEVSVC_MANAGER_STUB_H */
