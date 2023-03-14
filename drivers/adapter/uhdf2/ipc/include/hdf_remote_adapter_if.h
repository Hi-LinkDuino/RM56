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

#ifndef HDF_REMOTE_ADAPATER_IF_H
#define HDF_REMOTE_ADAPATER_IF_H

#include "hdf_remote_service.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfRemoteService *HdfRemoteAdapterObtain(void);

void HdfRemoteAdapterRecycle(struct HdfRemoteService *service);

int HdfRemoteAdapterAddService(const char *name, struct HdfRemoteService *service);

struct HdfRemoteService *HdfRemoteAdapterGetService(const char *name);

int HdfRemoteAdapterAddSa(int32_t saId, struct HdfRemoteService *service);

struct HdfRemoteService *HdfRemoteAdapterGetSa(int32_t saId);

void HdfRemoteAdapterAddDeathRecipient(struct HdfRemoteService *service, struct HdfDeathRecipient *recipient);

void HdfRemoteAdapterRemoveDeathRecipient(struct HdfRemoteService *service, struct HdfDeathRecipient *recipient);

bool HdfRemoteAdapterSetInterfaceDesc(struct HdfRemoteService *service, const char *descriptor);

bool HdfRemoteAdapterWriteInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data);

bool HdfRemoteAdapterCheckInterfaceToken(struct HdfRemoteService *service, struct HdfSBuf *data);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* HDF_REMOTE_ADAPATER_IF_H */