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

#ifndef HDF_REMOTE_SERVICE_H
#define HDF_REMOTE_SERVICE_H

#include "hdf_object.h"
#include "hdf_sbuf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfRemoteDispatcher {
    int (*Dispatch)(struct HdfRemoteService *, int, struct HdfSBuf *, struct HdfSBuf *);
    int (*DispatchAsync)(struct HdfRemoteService *, int, struct HdfSBuf *, struct HdfSBuf *);
};

struct HdfDeathRecipient {
    void (*OnRemoteDied)(struct HdfDeathRecipient *, struct HdfRemoteService *);
};

struct HdfRemoteService {
    struct HdfObject object_;
    struct HdfObject *target;
    struct HdfRemoteDispatcher *dispatcher;
    uint64_t index;
};

struct HdfRemoteService *HdfRemoteServiceObtain(
    struct HdfObject *object, struct HdfRemoteDispatcher *dispatcher);

void HdfRemoteServiceRecycle(struct HdfRemoteService *service);

void HdfRemoteServiceAddDeathRecipient(struct HdfRemoteService *service, struct HdfDeathRecipient *recipient);

void HdfRemoteServiceRemoveDeathRecipient(struct HdfRemoteService *service, struct HdfDeathRecipient *recipient);

int HdfRemoteServiceRegister(int32_t serviceId, struct HdfRemoteService *service);

struct HdfRemoteService *HdfRemoteServiceGet(int32_t serviceId);

bool HdfRemoteServiceSetInterfaceDesc(struct HdfRemoteService *service, const char *descriptor);

bool HdfRemoteServiceWriteInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data);

bool HdfRemoteServiceCheckInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* HDF_REMOTE_SERVICE_H */
