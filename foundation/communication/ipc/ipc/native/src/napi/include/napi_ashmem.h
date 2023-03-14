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

#ifndef NAPI_IPC_OHOS_ASHMEM_H
#define NAPI_IPC_OHOS_ASHMEM_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "ashmem.h"

namespace OHOS {
class NAPIAshmem {
public:
    enum {
        PROT_EXEC = 4,
        PROT_NONE = 0,
        PROT_READ = 1,
        PROT_WRITE = 2,
    };
    explicit NAPIAshmem(sptr<Ashmem> &ashmem);
    NAPIAshmem():ashmem_(nullptr){}
    ~NAPIAshmem() = default;
    const sptr<Ashmem> &GetAshmem()
    {
        return ashmem_;
    }
    void SetAshmem(sptr<Ashmem> &ashmem)
    {
        ashmem_ = ashmem;
    }
    static napi_value AshmemExport(napi_env env, napi_value exports);
private:
    static napi_value Ashmem_JS_Constructor(napi_env env, napi_callback_info cbinfo);
    static napi_value CloseAshmem(napi_env env, napi_callback_info info);
    static napi_value CreateAshmem(napi_env env, napi_callback_info info);
    static napi_value CreateAshmemFromExisting(napi_env env, napi_callback_info info);
    static napi_value GetAshmemSize(napi_env env, napi_callback_info info);
    static napi_value MapAshmem(napi_env env, napi_callback_info info);
    static napi_value MapReadAndWriteAshmem(napi_env env, napi_callback_info info);
    static napi_value MapReadOnlyAshmem(napi_env env, napi_callback_info info);
    static napi_value ReadFromAshmem(napi_env env, napi_callback_info info);
    static napi_value SetProtection(napi_env env, napi_callback_info info);
    static napi_value UnmapAshmem(napi_env env, napi_callback_info info);
    static napi_value WriteToAshmem(napi_env env, napi_callback_info info);
    sptr<Ashmem> ashmem_;
};
} // namespace OHOS
#endif
