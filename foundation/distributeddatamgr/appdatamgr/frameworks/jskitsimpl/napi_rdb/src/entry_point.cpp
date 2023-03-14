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

#include "napi_rdb_predicates.h"
#include "napi_rdb_store.h"
#include "napi_rdb_store_helper.h"
#include "napi_rdb_const_properties.h"
#include "napi/native_api.h"

using namespace OHOS::RdbJsKit;

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    InitRdbHelper(env, exports);
    RdbStoreProxy::Init(env, exports);
    RdbPredicatesProxy::Init(env, exports);
    InitConstProperties(env, exports);
    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "data.rdb",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

/*
 * Module register function
 */
static __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
