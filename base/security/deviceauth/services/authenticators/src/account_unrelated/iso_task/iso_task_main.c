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
#include "hc_log.h"
#include "hc_types.h"
#include "iso_base_cur_task.h"
#include "iso_client_task.h"
#include "iso_server_task.h"

bool IsIsoSupported(void)
{
    return true;
}

SubTaskBase *CreateIsoSubTask(const CJson *in)
{
    bool isClient = true;
    if (GetBoolFromJson(in, FIELD_IS_CLIENT, &isClient) != HC_SUCCESS) {
        LOGE("Get isClient failed.");
        return NULL;
    }
    if (isClient) {
        return CreateIsoClientTask(in);
    } else {
        return CreateIsoServerTask(in);
    }
}