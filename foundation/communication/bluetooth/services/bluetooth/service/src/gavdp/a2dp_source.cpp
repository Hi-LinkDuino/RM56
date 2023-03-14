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

#include "a2dp_source.h"

#include "log.h"

namespace OHOS {
namespace bluetooth {
A2dpSrcProfile::A2dpSrcProfile() : A2dpProfile(A2DP_ROLE_SOURCE)
{
    LOG_INFO("[A2dpSrcProfile]%{public}s\n", __func__);
}

A2dpSrcProfile::~A2dpSrcProfile()
{
    LOG_INFO("[A2dpSrcProfile]%{public}s\n", __func__);
}
A2dpSrcProfile *A2dpSrcProfile::GetInstance()
{
    static A2dpSrcProfile instance;
    return &instance;
}
}  // namespace bluetooth
}  // namespace OHOS