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

#ifndef TRANS_SESSION_MANAGER_H
#define TRANS_SESSION_MANAGER_H

#include "softbus_def.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ListNode node;
    SoftBusSecType type;
    char pkgName[PKG_NAME_SIZE_MAX];
    char sessionName[SESSION_NAME_SIZE_MAX];
    int32_t uid;
    int32_t pid;
} SessionServer;

int32_t TransSessionMgrInit(void);

void TransSessionMgrDeinit(void);

bool TransSessionServerIsExist(const char *sessionName);

int32_t TransSessionServerAddItem(SessionServer *newNode);

int32_t TransSessionServerDelItem(const char *sessionName);

void TransDelItemByPackageName(const char *pkgName);

int32_t TransGetPkgNameBySessionName(const char *sessionName, char *pkgName, uint16_t len);

int32_t TransGetUidAndPid(const char *sessionName, int32_t *uid, int32_t *pid);

void TransOnLinkDown(const char *networkId, int32_t routeType);

#ifdef __cplusplus
}
#endif
#endif
