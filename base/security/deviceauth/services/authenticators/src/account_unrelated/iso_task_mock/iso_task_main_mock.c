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

#include "iso_task_main.h"
#include "das_lite_token_manager.h"

bool IsIsoSupported(void)
{
    return false;
}

SubTaskBase *CreateIsoSubTask(const CJson *in)
{
    (void)in;
    return NULL;
}

const TokenManager *GetLiteTokenManagerInstance(void)
{
    return NULL;
}