/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_rdb_store_observer.h"

#include "js_logger.h"
#include "js_utils.h"

using OHOS::AppDataMgrJsKit::JSUtils;
using OHOS::AppDataMgrJsKit::PREFIX_LABEL;

namespace OHOS::RdbJsKit {
NapiRdbStoreObserver::NapiRdbStoreObserver(napi_env env, napi_value callback) : NapiUvQueue(env, callback)
{
    LOG_INFO("NapiRdbStoreObserver construct");
}

NapiRdbStoreObserver::~NapiRdbStoreObserver() noexcept
{
    LOG_INFO("NapiRdbStoreObserver destroy");
}

void NapiRdbStoreObserver::OnChange(const std::vector<std::string> &devices)
{
    LOG_INFO("NapiRdbStoreObserver::OnChange begin");
    CallFunction([devices](napi_env env, int &argc, napi_value *argv) {
        argc = 1;
        argv[0] = JSUtils::Convert2JSValue(env, devices);
    });
    LOG_INFO("NapiRdbStoreObserver::OnChange end");
}
} // namespace OHOS::RdbJsKit