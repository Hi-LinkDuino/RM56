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

#include "das_standard_token_manager.h"
#include "standard_client_bind_exchange_task.h"
#include "standard_client_unbind_exchange_task.h"
#include "standard_server_bind_exchange_task.h"
#include "standard_server_unbind_exchange_task.h"

AsyBaseCurTask *CreateStandardBindExchangeClientTask()
{
    return NULL;
}

AsyBaseCurTask *CreateStandardBindExchangeServerTask()
{
    return NULL;
}

AsyBaseCurTask *CreateStandardUnbindExchangeClientTask()
{
    return NULL;
}

AsyBaseCurTask *CreateStandardUnbindExchangeServerTask()
{
    return NULL;
}

const TokenManager *GetStandardTokenManagerInstance()
{
    return NULL;
}