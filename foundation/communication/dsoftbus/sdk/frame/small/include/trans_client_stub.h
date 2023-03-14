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

#ifndef TRANS_CLIENT_STUB_H
#define TRANS_CLIENT_STUB_H

#include "serializer.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t ClientOnChannelOpened(IpcIo *reply, const IpcContext *ctx, void *ipcMsg);
int32_t ClientOnChannelOpenfailed(IpcIo *reply, const IpcContext *ctx, void *ipcMsg);
int32_t ClientOnChannelClosed(IpcIo *reply, const IpcContext *ctx, void *ipcMsg);
int32_t ClientOnChannelMsgreceived(IpcIo *reply, const IpcContext *ctx, void *ipcMsg);

#ifdef __cplusplus
}
#endif
#endif