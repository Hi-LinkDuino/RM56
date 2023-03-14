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
#ifndef OHOS_APPEXECFWK_LIBZIP_COMMON_H
#define OHOS_APPEXECFWK_LIBZIP_COMMON_H
#include <memory>
#include <string>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "zip_utils.h"

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
#define NAPI_CALL_BASE_BOOL(theCall, retVal) \
    do {                                     \
        if ((theCall) == false) {            \
            return retVal;                   \
        }                                    \
    } while (0)

struct CallZipUnzipParam {
    std::string src;
    std::string dest;
    OPTIONS options;
};

struct ZlibCallbackInfo {
    napi_env env;
    napi_ref callback = 0;
    napi_deferred deferred;
    CallZipUnzipParam param;
    bool isCallBack = false;
    int callbackResult = 0;
};

struct AsyncZipCallbackInfo {
    napi_async_work asyncWork;
    std::shared_ptr<ZlibCallbackInfo> aceCallback;
};
bool UnwrapIntValue(napi_env env, napi_value jsValue, int &result);
bool IsTypeForNapiValue(napi_env env, napi_value param, napi_valuetype expectType);
std::string UnwrapStringFromJS(napi_env env, napi_value param, const std::string &defaultValue);
napi_value GetCallbackErrorValue(napi_env env, int errCode);
void SetNamedProperty(napi_env env, napi_value obj, const char *propName, const int propValue);

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_APPEXECFWK_LIBZIP_COMMON_H