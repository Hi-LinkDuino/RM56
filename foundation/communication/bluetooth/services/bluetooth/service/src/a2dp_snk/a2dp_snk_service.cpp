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

#include "a2dp_snk_service.h"
#include "class_creator.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
std::string A2dpSnkService::g_name = "A2dpSnkService";
std::string A2dpSnkService::g_version = "1.3.2";
uint8_t A2dpSnkService::g_role = A2DP_ROLE_SINK;

IProfileA2dp *IProfileA2dp::GetSnkProfile()
{
    return A2dpSnkService::GetProfile();
}

A2dpSnkService::A2dpSnkService() : A2dpService(g_name, g_version, g_role)
{
    HILOGI("enter");
}

A2dpSnkService *A2dpSnkService::GetProfile(void)
{
    HILOGI("enter");

    static A2dpSnkService service;
    return &service;
}

REGISTER_CLASS_CREATOR(A2dpSnkService);
}  // namespace bluetooth
}  // namespace OHOS