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

#include "a2dp_sink.h"

#include "log.h"

namespace OHOS {
namespace bluetooth {
A2dpSnkProfile::A2dpSnkProfile() : A2dpProfile(A2DP_ROLE_SINK)
{
    LOG_INFO("[A2dpSnkProfile]%{public}s\n", __func__);
}

A2dpSnkProfile::~A2dpSnkProfile()
{
    LOG_INFO("[A2dpSnkProfile]%{public}s\n", __func__);
}

A2dpSnkProfile *A2dpSnkProfile::GetInstance()
{
    LOG_INFO("[A2dpSnkProfile]%{public}s\n", __func__);
    static A2dpSnkProfile instance;
    return &instance;
}
}  // namespace bluetooth
}  // namespace OHOS