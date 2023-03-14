/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "n_class.h"

namespace OHOS {
namespace HiviewDFX {
using namespace std;
NClass &NClass::GetInstance()
{
    static NClass nClass;
    return nClass;
}

tuple<bool, napi_value> NClass::DefineClass(napi_env env, string className, napi_callback constructor,
    vector<napi_property_descriptor> &&properties)
{
    napi_value classVal = nullptr;
    napi_status stat = napi_define_class(env, className.c_str(), className.length(), constructor,
        nullptr, properties.size(), properties.data(), &classVal);
    return { stat == napi_ok, classVal };
}

napi_value NClass::InstantiateClass(napi_env env, string className, vector<napi_value> args)
{
    NClass &nClass = NClass::GetInstance();
    lock_guard(nClass.exClassMapLock);

    auto it = nClass.exClassMap.find(className);
    if (it == nClass.exClassMap.end()) {
        return nullptr;
    }
    napi_value cons = nullptr;
    napi_status status = napi_get_reference_value(env, it->second, &cons);
    if (status != napi_ok) {
        return nullptr;
    }
    napi_value instance = nullptr;
    status = napi_new_instance(env, cons, args.size(), args.data(), &instance);
    if (status != napi_ok) {
        return nullptr;
    }
    return instance;
}
} // namespace HiviewDFX
} // namespace OHOS