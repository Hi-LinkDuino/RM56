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

#ifndef NAPI_BLUETOOTH_PROFILE_H_
#define NAPI_BLUETOOTH_PROFILE_H_

#include <map>
#include <memory>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_bluetooth_utils.h"

namespace OHOS {
namespace Bluetooth {

class NapiProfile {
public:
    static void DefineProfileFunctions(napi_env env, napi_value exports);
    static thread_local std::map<ProfileId, napi_value> profiles_;
    static void SetProfile(ProfileId code, napi_value profile);
    static napi_value GetProfile(napi_env env, napi_callback_info info);
    static void ProfileEnumInit(napi_env env, napi_value exports);
    static napi_value SppTypeInit(napi_env env);
    static napi_value PlayingStateInit(napi_env env);
    static napi_value ProfileIdInit(napi_env env);
};

}  // namespace Bluetooth
}  // namespace OHOS
#endif /* NAPI_BLUETOOTH_PROFILE_H_ */

