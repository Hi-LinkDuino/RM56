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

#include <securec.h>
#include <cassert>
#include "ability_manager.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ability_errors.h"

#define GET_PARAMS(env, info, num) \
    do {                           \
    size_t argc = num;             \
    napi_value argv[num];          \
    napi_value thisVar;            \
    void* data;                    \
    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data) \
    while (0)

using struct _SetTimeAsyncContext {
    napi_env env;
    napi_async_work work;

    int64_t time;
    napi_deferred deferred;
    napi_ref callbackRef;

    int status;
} SetTimeAsyncContext;

static int JSAafwkStartAbility(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1] = {0};
    status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    assert(status == napi_ok);
    
    napi_valuetype types[1];
    status = napi_typeof(env, args[0], types);
    assert(status == napi_ok);
    assert(argc == 1 && types[0] == napi_object);

    Want want;
    if (memset_s(&want, sizeof(Want), 0x00, sizeof(Want)) != 0) {
        return MEMORY_MALLOC_ERROR;
    }
    if (GetWantFromNapiValue(env, args[0], want) == false) {
        return PARAM_CHECK_ERROR;
    }
    StartAbility(&want);
    ClearWant(&want);
}

static int JSAafwkStopAbility(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 1;
    napi_value args[1] = {0};;
    status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    assert(status == napi_ok);
    
    napi_valuetype types[1];
    status = napi_typeof(env, args[0], types);
    assert(status == napi_ok);
    assert(argc == 1 && types[0] == napi_object);

    Want want;
    if (memset_s(&want, sizeof(Want), 0x00, sizeof(Want)) != 0) {
        return MEMORY_MALLOC_ERROR;
    }
    if (GetWantFromNapiValue(env, args[0], want) == false) {
        return PARAM_CHECK_ERROR;
    }
    StopAbility(&want);
    ClearWant(&want);
}

static bool GetWantFromNapiValue(napi_env env, napi_value args, Want& want)
{
    ElementName element;
    if (memset_s(&element, sizeof(ElementName), 0x00, sizeof(ElementName)) != 0) {
        return MEMORY_MALLOC_ERROR;
    }

    napi_value data;
    napi_get_named_property(env, args, "want_param", &data);

    napi_value elementName;
    if (napi_get_named_property(env, args, "elementName", &elementName) != napi_ok) {
        return COMMAND_ERROR;
    }

    napi_value napi_deviceId;
    napi_get_named_property(env, elementName, "deviceId", &napi_deviceId);
    char *deviceId = nullptr;
    GetCharPointerArgument(env, napi_deviceId, deviceId);
    SetElementDeviceID(&element, deviceId);
    free(deviceId);
    deviceId = nullptr;

    napi_value napi_bundleName;
    napi_get_named_property(env, elementName, "bundleName", &napi_bundleName);
    char *bundleName = nullptr;
    GetCharPointerArgument(env, napi_bundleName, bundleName);
    SetElementBundleName(&element, bundleName);
    free(bundleName);
    bundleName = nullptr;

    napi_value napi_abilityName;
    napi_get_named_property(env, elementName, "abilityName", &napi_abilityName);
    char *abilityName = nullptr;
    GetCharPointerArgument(env, napi_abilityName, abilityName);
    SetElementAbilityName(&element, abilityName);
    free(abilityName);
    abilityName = nullptr;

    SetWantData(&want, (void *)data, sizeof(data));
    SetWantElement(&want, element);
    ClearElement(&element);
}

// Function to read string argument from napi_value
static bool GetCharPointerArgument(napi_env env, napi_value value, char* result)
{
    napi_status status;
    size_t bufLength = 0;
    result = nullptr;
    bool ret = false;
    // get buffer length first and get buffer based on length
    status = napi_get_value_string_utf8(env, value, nullptr, 0, &bufLength);
    if (status == napi_ok && bufLength > 0) {
        // Create a buffer and create std::string later from it
        result = (char *) malloc((bufLength + 1) * sizeof(char));
        if (result != nullptr) {
            status = napi_get_value_string_utf8(env, value, result, bufLength + 1, &bufLength);
            if (status == napi_ok) {
                ret = true;
            }
        }
    }
    return ret;
}

EXTERN_C_START
using namespace OHOS {
napi_value AafwkExport(napi_env env, napi_value exports)
{
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("startAbility", JSAafwkStartAbility),
        DECLARE_NAPI_FUNCTION("stopAbility", JSAafwkStopAbility),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}
}
EXTERN_C_END

static napi_module aafwk_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = AafwkExport,
    .nm_modname = "aafwk",
    .nm_priv = ((void*)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void AafwkRegister()
{
    napi_module_register(&aafwk_module);
}