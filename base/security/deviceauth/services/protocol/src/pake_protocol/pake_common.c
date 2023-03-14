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

#include "pake_defs.h"
#include "hc_types.h"
#include "protocol_common.h"

void CleanPakeSensitiveKeys(PakeBaseParams *params)
{
    if (params == NULL) {
        return;
    }
    FreeAndCleanKey(&params->psk);
    FreeAndCleanKey(&params->base);
    FreeAndCleanKey(&params->eskSelf);
    FreeAndCleanKey(&params->sharedSecret);
    FreeAndCleanKey(&params->sessionKey);
    FreeAndCleanKey(&params->hmacKey);
}