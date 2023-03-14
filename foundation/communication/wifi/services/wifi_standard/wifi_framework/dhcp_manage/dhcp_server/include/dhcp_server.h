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

#ifndef OHOS_DHCP_SERVER_H
#define OHOS_DHCP_SERVER_H
#include "dhcp_config.h"
#include "dhcp_address_pool.h"

enum DhcpServerState { ST_IDEL = 0, ST_STARTING, ST_RUNNING, ST_RELOADNG, ST_STOPING, ST_STOPED };
typedef int (*DhcpServerCallback)(int, int, const char *ifname);

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ServerContext ServerContext;
typedef struct {
    char ifname[IFACE_NAME_SIZE];
    ServerContext *instance;
} DhcpServerContext, *PDhcpServerContext;

PDhcpServerContext InitializeServer(DhcpConfig *config);
int StartDhcpServer(PDhcpServerContext ctx);
int StopDhcpServer(PDhcpServerContext ctx);
int GetServerStatus(PDhcpServerContext ctx);
void RegisterDhcpCallback(PDhcpServerContext ctx, DhcpServerCallback callback);
int FreeServerContext(PDhcpServerContext ctx);
int SaveLease(PDhcpServerContext ctx);

#ifdef __cplusplus
}
#endif
#endif