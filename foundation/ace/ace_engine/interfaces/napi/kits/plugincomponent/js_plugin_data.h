/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_NAPI_ACE_PLUGIN_DATA_H
#define OHOS_NAPI_ACE_PLUGIN_DATA_H

#include <map>
#include <string>
#include <vector>

#include "ability.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "ohos/aafwk/content/pac_map.h"
#include "ohos/aafwk/content/want.h"

namespace OHOS::Ace::Napi {
struct ACECallbackInfo {
    napi_env env = nullptr;
    napi_ref callback = nullptr;
};

enum class TACENativeValueType {
    NVT_NONE = 0,
    NVT_UNDEFINED,
    NVT_INT32,
    NVT_BOOL,
    NVT_STRING,
    NVT_DOUBLE,
};

struct ACEThreadReturnData {
    TACENativeValueType data_type = TACENativeValueType::NVT_NONE;
    int32_t int32_value = 0;
    bool bool_value = false;
    std::string str_value = {};
    double double_value = 0.0;
    AppExecFwk::PacMap paramList = {};
};

struct AceFromJSParam {
    AAFwk::Want want = {};
    AppExecFwk::PacMap paramList = {};
};

struct ACEAsyncJSCallbackInfo {
    ACECallbackInfo cbInfo = {};
    Ability* ability = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    ACEThreadReturnData native_data = {};
    AceFromJSParam jsParamList = {};
    napi_value result = nullptr;
    int error_code = 0;
    AAFwk::Want wantStage = {};
};

struct ACEAsyncParamEx {
    std::string resource = {};
    size_t argc = 0;
    napi_async_execute_callback execute = nullptr;
    napi_async_complete_callback complete = nullptr;
};

struct ACEComplexArrayData {
    std::vector<int> intList = {};
    std::vector<int64_t> longList = {};
    std::vector<bool> boolList = {};
    std::vector<double> doubleList = {};
    std::vector<std::string> stringList = {};
};
}  // namespace OHOS::Ace::Napi
#endif  // OHOS_NAPI_ACE_PLUGIN_DATA_H
