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

#ifndef OHOS_IPC_RPC_IREMOTTE_INVOKER_H
#define OHOS_IPC_RPC_IREMOTTE_INVOKER_H

#include "ipc_skeleton.h"
#include "rpc_types.h"
#include "serializer.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    void *connector;
    int32_t (*AcquireHandle)(int32_t handle);
    int32_t (*ReleaseHandle)(int32_t handle);
    int32_t (*SendRequest)(SvcIdentity target, uint32_t code, IpcIo *data,
        IpcIo *reply, MessageOption option, uintptr_t *buffer);
    int32_t (*FreeBuffer)(void *ptr);
    int32_t (*SetMaxWorkThread)(int32_t maxThreadNum);
    void (*JoinThread)(bool initiative);
    void (*ExitCurrentThread)(void);
    void (*JoinProcessThread)(bool initiative);
    int32_t (*SetRegistryObject)(void);
    int32_t (*AddDeathRecipient)(int32_t handle, void *cookie);
    int32_t (*RemoveDeathRecipient)(int32_t handle, void *cookie);
} RemoteInvoker;

RemoteInvoker *InitRemoteInvoker(int32_t proto);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* OHOS_IPC_RPC_IREMOTTE_INVOKER_H */