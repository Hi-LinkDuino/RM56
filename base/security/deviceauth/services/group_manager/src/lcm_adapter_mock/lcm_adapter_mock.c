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

#include "lcm_adapter.h"

#include "device_auth_defines.h"

int32_t ProcessCredential(int operationCode, const char *reqJsonStr, char **returnJsonStr)
{
    (void)operationCode;
    (void)reqJsonStr;
    (void)returnJsonStr;
    return HC_ERR_NOT_SUPPORT;
}

int32_t GetRegisterInfo(char **returnRegisterInfo)
{
    (void)returnRegisterInfo;
    return HC_ERR_NOT_SUPPORT;
}