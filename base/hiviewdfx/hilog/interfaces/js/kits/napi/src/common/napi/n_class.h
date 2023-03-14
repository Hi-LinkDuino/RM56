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

#ifndef HIVIEWDFX_NAPI_NCALSS
#define HIVIEWDFX_NAPI_NCALSS

#pragma once

#include "uni_header.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

namespace OHOS {
namespace HiviewDFX {
class NClass {
public:
    NClass(const NClass &) = delete;
    NClass &operator = (const NClass &) = delete;
    ~NClass() {};
    static NClass &GetInstance();

    static std::tuple<bool, napi_value> DefineClass(napi_env env,
                                                    std::string className,
                                                    napi_callback constructor,
                                                    std::vector<napi_property_descriptor> &&properties);
    static bool SaveClass(napi_env env, std::string className, napi_value exClass);
    static napi_value InstantiateClass(napi_env env, std::string className, std::vector<napi_value> args);

    template<class T> static T *GetEntityOf(napi_env env, napi_value objStat)
    {
        if (!env || !objStat) {
            return nullptr;
        }
        T *t = nullptr;
        napi_status status = napi_unwrap(env, objStat, (void **)&t);
        if (status != napi_ok) {
            return nullptr;
        }
        return t;
    }

    template<class T> static bool SetEntityFor(napi_env env, napi_value obj, std::unique_ptr<T> entity)
    {
        napi_status status = napi_wrap(
            env,
            obj,
            entity.get(),
            [](napi_env env, void *data, void *hint) {
                auto entity = static_cast<T *>(data);
                delete entity;
            },
            nullptr,
            nullptr);
        entity.release();
        return status == napi_ok;
    }

private:
    NClass() = default;
    std::map<std::string, napi_ref> exClassMap;
    std::mutex exClassMapLock;
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HIVIEWDFX_NAPI_NCALSS