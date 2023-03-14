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
#include "napi_faultlogger.h"

#include <sstream>
#include <unistd.h>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "common_utils.h"
#include "logger.h"

#include "faultlog_query_result.h"
#include "napi_util.h"

namespace OHOS {
namespace HiviewDFX {
DEFINE_LOG_TAG("Faultlogger-napi");
namespace {
    constexpr size_t ONE_PARAMETER = 1;
    constexpr size_t TWO_PARAMETER = 2;
    constexpr size_t THREE_PARAMETER = 3;
    constexpr size_t FOUR_PARAMETER = 4;
}
static FaultLogNapiInfo ConversionInform(std::unique_ptr<FaultLogInfo> faultLogInfo)
{
    FaultLogNapiInfo ret = {
        .pid = faultLogInfo->GetProcessId(),
        .uid = faultLogInfo->GetId(),
        .type = faultLogInfo->GetFaultType(),
        .ts = faultLogInfo->GetTimeStamp(),
        .reason = faultLogInfo->GetFaultReason(),
        .module = faultLogInfo->GetModuleName(),
        .summary = faultLogInfo->GetFaultSummary(),
    };
    int fd = faultLogInfo->GetRawFileDescriptor();
    if (fd < 0) {
        HIVIEW_LOGE("pid %{public}d Fail to get fd:%{public}d\n", faultLogInfo->GetProcessId(), fd);
        ret.fullLog = "Fail to get log, fd is " + std::to_string(fd);
        return ret;
    }
    while (true) {
        char buf[BUF_SIZE_512] = {0};
        int rn = read((fd), buf, BUF_SIZE_512 - 1);
        if (rn == -1) {
            if (errno == EAGAIN) {
                continue;
            } else {
                break;
            }
        } else if (rn == 0) {
            break;
        }
        ret.fullLog += buf;
    }
    close(fd);
    
    return ret;
}

static void FaultLogExecuteCallback(napi_env env, void *data)
{
    FaultLogInfoContext* faultLogInfoContext = static_cast<FaultLogInfoContext *>(data);
    const int maxQueryCount = 10;
    int currentCount = 0;
    auto faultLogResult = QuerySelfFaultLog((FaultLogType)faultLogInfoContext->faultType,
        maxQueryCount);
    if (faultLogResult == nullptr) {
        faultLogInfoContext->resolved = true;
        return;
    }

    while (faultLogResult->HasNext()) {
        if (currentCount >= maxQueryCount) {
            break;
        }
        auto faultLogInfo = faultLogResult->Next();
        if (faultLogInfo == nullptr) {
            break;
        }
        currentCount++;
        faultLogInfoContext->infoVector.push_back(ConversionInform(std::move(faultLogInfo)));
    }
    faultLogInfoContext->resolved = true;
}

static void FaultLogCompleteCallback(napi_env env, napi_status status, void *data)
{
    FaultLogInfoContext* faultLogInfoContext = static_cast<FaultLogInfoContext *>(data);
    if (faultLogInfoContext == nullptr) {
        return;
    }
    napi_value callbackValue = nullptr;
    if (faultLogInfoContext->resolved) {
        napi_create_array(env, &callbackValue);
        int i = 0;
        for (auto& infoItem : faultLogInfoContext->infoVector) {
            napi_value info = nullptr;
            napi_create_object(env, &info);
            NapiUtil::SetPropertyInt32(env, info, "pid", infoItem.pid);
            NapiUtil::SetPropertyInt32(env, info, "uid", infoItem.uid);
            NapiUtil::SetPropertyInt32(env, info, "type", infoItem.type);
            NapiUtil::SetPropertyInt64(env, info, "ts", infoItem.ts);
            NapiUtil::SetPropertyStringUtf8(env, info, "reason", infoItem.reason);
            NapiUtil::SetPropertyStringUtf8(env, info, "module", infoItem.module);
            NapiUtil::SetPropertyStringUtf8(env, info, "summary", infoItem.summary);
            NapiUtil::SetPropertyStringUtf8(env, info, "fullLog", infoItem.fullLog);
            napi_set_element(env, callbackValue, i, info);
            ++i;
            HIVIEW_LOGI("add element when resovled pid = %{public}d, uid = %{public}d, ts = %{public}ld",
                infoItem.pid, infoItem.uid, infoItem.ts);
        }
    } else {
        callbackValue = NapiUtil::CreateErrorMessage(env, "get signal info list failed");
    }

    if (faultLogInfoContext->callbackRef != nullptr) {
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, faultLogInfoContext->callbackRef, &callbackFunc);
        napi_value callbackValues[] = {nullptr, nullptr};
        callbackValues[0] = faultLogInfoContext->resolved ? NapiUtil::CreateUndefined(env) : callbackValue;
        callbackValues[1] = faultLogInfoContext->resolved ? callbackValue : NapiUtil::CreateUndefined(env);
        napi_value recv = NapiUtil::CreateUndefined(env);
        napi_value result = nullptr;
        napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
        napi_delete_reference(env, faultLogInfoContext->callbackRef);
    } else if (faultLogInfoContext->deferred != nullptr) {
        if (faultLogInfoContext->resolved) {
            napi_resolve_deferred(env, faultLogInfoContext->deferred, callbackValue);
        } else {
            napi_reject_deferred(env, faultLogInfoContext->deferred, callbackValue);
        }
    }

    napi_delete_async_work(env, faultLogInfoContext->work);
    delete faultLogInfoContext;
}

static napi_value QuerySelfFaultLog(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 2;
    napi_value parameters[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data));

    napi_value result = NapiUtil::CreateUndefined(env);
    if (parameterCount < ONE_PARAMETER && parameterCount > TWO_PARAMETER) {
        HIVIEW_LOGE("parameterCount Incorrect %{public}d", parameterCount);
        return result;
    }

    if (!NapiUtil::IsMatchType(env, parameters[ONE_PARAMETER - 1], napi_number)) {
        HIVIEW_LOGE("parameters[0] type isn't number");
        return result;
    }
    
    auto faultLogInfoContext = std::make_unique<FaultLogInfoContext>().release();
    if (faultLogInfoContext == nullptr) {
        HIVIEW_LOGE("faultLogInfoContext == nullptr");
        return result;
    }

    napi_get_value_int32(env, parameters[ONE_PARAMETER - 1], &faultLogInfoContext->faultType);
    if (parameterCount == ONE_PARAMETER) {
        napi_create_promise(env, &faultLogInfoContext->deferred, &result);
    } else if (parameterCount == TWO_PARAMETER) {
        if (!NapiUtil::IsMatchType(env, parameters[TWO_PARAMETER - 1], napi_function)) {
            HIVIEW_LOGE("parameters[1] type isn't function");
            delete faultLogInfoContext;
            return result;
        }
        NAPI_CALL(env, napi_create_reference(env, parameters[TWO_PARAMETER - 1], 1, &faultLogInfoContext->callbackRef));
    }

    napi_value resource = NapiUtil::CreateUndefined(env);
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "QuerySelfFaultLog", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env, resource, resourceName, FaultLogExecuteCallback,
        FaultLogCompleteCallback, (void *)faultLogInfoContext, &faultLogInfoContext->work);
    napi_queue_async_work(env, faultLogInfoContext->work);
    return result;
}

static napi_value AddFaultLog(napi_env env, napi_callback_info info)
{
    size_t parameterCount = 4;
    napi_value parameters[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &parameterCount, parameters, &thisVar, &data));

    napi_value result = NapiUtil::CreateUndefined(env);
    if (parameterCount != FOUR_PARAMETER) {
        return result;
    }
    if (!NapiUtil::IsMatchType(env, parameters[ONE_PARAMETER - 1], napi_number)) {
        HIVIEW_LOGE("parameters[0] type isn't number");
        return result;
    }
    if (!NapiUtil::IsMatchType(env, parameters[TWO_PARAMETER - 1], napi_number)) {
        HIVIEW_LOGE("parameters[1] type isn't number");
        return result;
    }
    if (!NapiUtil::IsMatchType(env, parameters[THREE_PARAMETER - 1], napi_string)) {
        HIVIEW_LOGE("parameters[2] type isn't napi_string");
        return result;
    }
    if (!NapiUtil::IsMatchType(env, parameters[FOUR_PARAMETER - 1], napi_string)) {
        HIVIEW_LOGE("parameters[3] type isn't napi_string");
        return result;
    }
    int32_t nowTmp;
    size_t resultString;
    napi_get_value_int32(env, parameters[ONE_PARAMETER - 1], &nowTmp);
    int64_t now = time(nullptr) + nowTmp;
    int32_t logType;
    napi_get_value_int32(env, parameters[TWO_PARAMETER - 1], &logType);

    char module[BUF_SIZE_64];
    napi_get_value_string_utf8(env, parameters[THREE_PARAMETER - 1], module, BUF_SIZE_64, &resultString);
    char summary[BUF_SIZE_64];
    napi_get_value_string_utf8(env, parameters[FOUR_PARAMETER - 1], summary, BUF_SIZE_64, &resultString);
    AddFaultLog(now, logType, module, summary);
    return result;
}

napi_value FaultLogTypeConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void* data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static void FaultLogTypeEnumInit(napi_env env, napi_value exports)
{
    napi_value noSpecific = nullptr;
    napi_value javaCrash = nullptr;
    napi_value cppCrash = nullptr;
    napi_value jsCrash = nullptr;
    napi_value appCrash = nullptr;

    napi_create_int32(env, FaultLogType::NO_SPECIFIC, &noSpecific);
    napi_create_int32(env, FaultLogType::JAVA_CRASH, &javaCrash);
    napi_create_int32(env, FaultLogType::CPP_CRASH, &cppCrash);
    napi_create_int32(env, FaultLogType::JS_CRASH, &jsCrash);
    napi_create_int32(env, FaultLogType::APP_FREEZE, &appCrash);

    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NO_SPECIFIC", noSpecific),
        DECLARE_NAPI_STATIC_PROPERTY("JAVA_CRASH", javaCrash),
        DECLARE_NAPI_STATIC_PROPERTY("CPP_CRASH", cppCrash),
        DECLARE_NAPI_STATIC_PROPERTY("JS_CRASH", jsCrash),
        DECLARE_NAPI_STATIC_PROPERTY("APP_FREEZE", appCrash),
    };

    napi_value result = nullptr;
    napi_define_class(env, "FaultType", NAPI_AUTO_LENGTH, FaultLogTypeConstructor, nullptr,
                      sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "FaultType", result);
}

napi_value FaultLogInfoConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void* data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static void FaultLogInfoClassInit(napi_env env, napi_value exports)
{
    napi_value pid = nullptr;
    napi_value uid = nullptr;
    napi_value type = nullptr;
    napi_value ts = nullptr;
    napi_value reason = nullptr;
    napi_value module = nullptr;
    napi_value summary = nullptr;
    napi_value fullLog = nullptr;

    napi_create_int32(env, 0, &pid);
    napi_create_int32(env, 0, &uid);
    napi_create_int32(env, FaultLogType::NO_SPECIFIC, &type);
    napi_create_int64(env, 0, &ts);
    napi_create_string_latin1(env, "reason", NAPI_AUTO_LENGTH, &reason);
    napi_create_string_latin1(env, "module", NAPI_AUTO_LENGTH, &module);
    napi_create_string_latin1(env, "summary", NAPI_AUTO_LENGTH, &summary);
    napi_create_string_latin1(env, "fullLog", NAPI_AUTO_LENGTH, &fullLog);

    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_PROPERTY("pid", pid),
        DECLARE_NAPI_PROPERTY("uid", uid),
        DECLARE_NAPI_PROPERTY("type", type),
        DECLARE_NAPI_PROPERTY("ts", ts),
        DECLARE_NAPI_PROPERTY("reason", reason),
        DECLARE_NAPI_PROPERTY("module", module),
        DECLARE_NAPI_PROPERTY("summary", summary),
        DECLARE_NAPI_PROPERTY("fullLog", fullLog),
    };

    napi_value result = nullptr;
    napi_define_class(env, "FaultLogInfo", NAPI_AUTO_LENGTH, FaultLogInfoConstructor, nullptr,
                      sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "FaultLogInfo", result);
}

EXTERN_C_START
napi_value InitNapiRegistry(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("querySelfFaultLog", QuerySelfFaultLog),
        DECLARE_NAPI_FUNCTION("addFaultLog", AddFaultLog),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    FaultLogTypeEnumInit(env, exports);
    FaultLogInfoClassInit(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module _stateRegistryModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = InitNapiRegistry,
    .nm_modname = "faultLogger",
    .nm_priv = ((void *)0),
    .reserved = {(void *)0},
};

extern "C" __attribute__((constructor)) void RegisterFaultLoggerModule(void)
{
    napi_module_register(&_stateRegistryModule);
}
}  // namespace HiviewDFX
}  // namespace OHOS