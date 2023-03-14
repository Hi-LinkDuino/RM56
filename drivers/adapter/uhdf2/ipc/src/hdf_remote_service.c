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

#include "hdf_remote_service.h"
#include "hdf_remote_adapter_if.h"

void HdfRemoteServiceAddDeathRecipient(
    struct HdfRemoteService *service, struct HdfDeathRecipient *recipient)
{
    HdfRemoteAdapterAddDeathRecipient(service, recipient);
}

void HdfRemoteServiceRemoveDeathRecipient(
    struct HdfRemoteService *service, struct HdfDeathRecipient *recipient)
{
    HdfRemoteAdapterRemoveDeathRecipient(service, recipient);
}

struct HdfRemoteService *HdfRemoteServiceObtain(struct HdfObject *object, struct HdfRemoteDispatcher *dispatcher)
{
    struct HdfRemoteService *service = HdfRemoteAdapterObtain();
    if ((service != NULL) && (service->dispatcher == NULL)) {
        service->dispatcher = dispatcher;
        service->target = object;
    }
    return service;
}

void HdfRemoteServiceRecycle(struct HdfRemoteService *service)
{
    HdfRemoteAdapterRecycle(service);
}

struct HdfRemoteService *HdfRemoteServiceGet(int32_t serviceId)
{
    return HdfRemoteAdapterGetSa(serviceId);
}

int HdfRemoteServiceRegister(int32_t serviceId, struct HdfRemoteService *service)
{
    return HdfRemoteAdapterAddSa(serviceId, service);
}

bool HdfRemoteServiceSetInterfaceDesc(struct HdfRemoteService *service, const char *descriptor)
{
    return HdfRemoteAdapterSetInterfaceDesc(service, descriptor);
}

bool HdfRemoteServiceWriteInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data)
{
    return HdfRemoteAdapterWriteInterfaceToken(service, data);
}

bool HdfRemoteServiceCheckInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data)
{
    return HdfRemoteAdapterCheckInterfaceToken(service, data);
}

