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

#include "account_module.h"
#include "device_auth_defines.h"
#include "hc_log.h"

int32_t CheckAccountMsgRepeatability(const CJson *in)
{
    (void)in;
    LOGE("Account module is not supported.");
    return HC_ERR_NOT_SUPPORT;
}

bool IsAccountSupported(void)
{
    return false;
}

AuthModuleBase *CreateAccountModule(void)
{
    LOGE("Account module is not supported.");
    return NULL;
}

int32_t ProcessAccountCredentials(int credentialOpCode, const CJson *in, CJson *out)
{
    (void)credentialOpCode;
    (void)in;
    (void)out;
    LOGE("Account credentials manager is not supported.");
    return HC_ERR_NOT_SUPPORT;
}