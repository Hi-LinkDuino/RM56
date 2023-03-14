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

#include "a2dp_src_service.h"
#include "class_creator.h"
#include "log.h"

namespace OHOS {
namespace bluetooth {
std::string A2dpSrcService::g_name = "A2dpSrcService";
std::string A2dpSrcService::g_version = "1.3.2";
uint8_t A2dpSrcService::g_role = A2DP_ROLE_SOURCE;

IProfileA2dp *IProfileA2dp::GetSrcProfile()
{
    return A2dpSrcService::GetProfile();
}

A2dpSrcService::A2dpSrcService() : A2dpService(g_name, g_version, g_role)
{
    HILOGI("enter");
}

A2dpSrcService *A2dpSrcService::GetProfile(void)
{
    HILOGI("enter");

    static A2dpSrcService service;
    return &service;
}

REGISTER_CLASS_CREATOR(A2dpSrcService);
}  // namespace bluetooth
}  // namespace OHOS