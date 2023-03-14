/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DAS_MODULE_H
#define DAS_MODULE_H

#include "common_defs.h"
#include "string_util.h"
#include "dev_auth_module_manager.h"

typedef struct DasAuthModuleT {
    AuthModuleBase moduleBase;
    int32_t (*registerLocalIdentity)(const char *, const char *, Uint8Buff *, int);
    int32_t (*unregisterLocalIdentity)(const char *, const char *, Uint8Buff *, int);
    int32_t (*deletePeerAuthInfo)(const char *, const char *, Uint8Buff *, int);
    int32_t (*getPublicKey)(const char *, const char *, Uint8Buff *, int, Uint8Buff *);
} DasAuthModule;

bool IsDasSupported(void);
bool IsDasMsgNeedIgnore(const CJson *in);
AuthModuleBase *CreateDasModule(void);

#endif
