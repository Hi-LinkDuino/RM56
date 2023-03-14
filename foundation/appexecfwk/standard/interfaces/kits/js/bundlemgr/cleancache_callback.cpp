/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <uv.h>

#include "cleancache_callback.h"

#include "napi/native_common.h"

namespace {
constexpr int32_t OPERATION_SUCCESS = 0;
constexpr int32_t OPERATION_FAILED = 1;
}

CleanCacheCallback::CleanCacheCallback(int32_t err) : err_(err)
{
    uv_sem_init(&uvSem_, 0);
}

CleanCacheCallback::~CleanCacheCallback() {}

void CleanCacheCallback::OnCleanCacheFinished(bool err)
{
    err_ = err ? OPERATION_FAILED : OPERATION_SUCCESS;
    uv_sem_post(&uvSem_);
}