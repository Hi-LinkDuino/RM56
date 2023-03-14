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
#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "frameworks/core/components/common/layout/grid_system_manager.h"

namespace OHOS::Ace::Napi {

static napi_value JSGridGetSystemLayoutInfo(napi_env env, napi_callback_info info)
{
    std::string gridInfo = OHOS::Ace::GridSystemManager::GetInstance().GetCurrentGridInfo().ToString();
    if (gridInfo.empty()) {
        LOGE("gridInfo is empty");
        return nullptr;
    }
    napi_value globalValue;
    napi_get_global(env, &globalValue);
    napi_value jsonValue;
    napi_get_named_property(env, globalValue, "JSON", &jsonValue);
    napi_value parseValue;
    napi_get_named_property(env, jsonValue, "parse", &parseValue);
    napi_value gridInfoNApi;
    napi_create_string_utf8(env, gridInfo.c_str(), NAPI_AUTO_LENGTH, &gridInfoNApi);
    napi_value funcArgv[1] = { gridInfoNApi };
    napi_value result;
    napi_call_function(env, jsonValue, parseValue, 1, funcArgv, &result);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, result, &valueType);
    if (valueType != napi_object) {
        LOGE("parse result fail");
        return nullptr;
    }
    return result;
}

static napi_value GridExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor gridDesc[] = {
        DECLARE_NAPI_FUNCTION("getSystemLayoutInfo", JSGridGetSystemLayoutInfo),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(gridDesc) / sizeof(gridDesc[0]), gridDesc));
    return exports;
}

static napi_module gridModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = GridExport,
    .nm_modname = "grid",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void GridRegister()
{
    napi_module_register(&gridModule);
}

} // namespace OHOS::Ace::Napi
