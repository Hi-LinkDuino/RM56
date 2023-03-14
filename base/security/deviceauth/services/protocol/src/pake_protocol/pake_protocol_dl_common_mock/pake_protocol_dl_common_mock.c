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

#include "pake_protocol_dl_common.h"
#include "device_auth_defines.h"
#include "hc_log.h"
#include "hc_types.h"
#include "pake_defs.h"
#include "string_util.h"

uint32_t GetPakeDlAlg(void)
{
    return PAKE_ALG_NONE;
}

int32_t GenerateDlPakeParams(PakeBaseParams *params, const Uint8Buff *secret)
{
    (void)params;
    (void)secret;
    LOGE("PAKE-DL unsupported.");
    return HC_ERR_NOT_SUPPORT;
}

int32_t AgreeDlSharedSecret(PakeBaseParams *params, Uint8Buff *sharedSecret)
{
    (void)params;
    (void)sharedSecret;
    LOGE("PAKE-DL unsupported.");
    return HC_ERR_NOT_SUPPORT;
}