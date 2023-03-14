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

#ifndef DISC_SERVER_PROXY_H

#include <stdint.h>
#include "discovery_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

int32_t DiscServerProxyInit(void);
void DiscServerProxyDeInit(void);
int32_t ServerIpcPublishService(const char *pkgName, const PublishInfo *info);
int32_t ServerIpcUnPublishService(const char *pkgName, int32_t publishId);
int32_t ServerIpcStartDiscovery(const char *pkgName, const SubscribeInfo *info);
int32_t ServerIpcStopDiscovery(const char *pkgName, int32_t subscribeId);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif // !DISC_SERVER_PROXY_H
