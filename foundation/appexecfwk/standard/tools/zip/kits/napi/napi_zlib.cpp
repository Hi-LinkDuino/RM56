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
#include "napi_zlib.h"

#include <cstring>
#include <uv.h>
#include <vector>

#include "app_log_wrapper.h"
#include "file_path.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_zlib_common.h"
#include "zip.h"
#include "zip_utils.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace AppExecFwk {
namespace LIBZIP {
namespace {
constexpr int32_t NO_ERROR = 0;
constexpr size_t ARGS_TWO = 2;
constexpr size_t ARGS_MAX_COUNT = 10;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM3 = 3;
}

#define COMPRESS_LEVE_CHECK(level, ret)                                                            \
    if (!(level == COMPRESS_LEVEL_NO_COMPRESSION || level == COMPRESS_LEVEL_DEFAULT_COMPRESSION || \
            level == COMPRESS_LEVEL_BEST_SPEED || level == COMPRESS_LEVEL_BEST_COMPRESSION)) {     \
        APP_LOGE("level parameter =[%{public}d] value is incorrect", (int)level);               \
        return ret;                                                                                \
    }

#define COMPRESS_STRATEGY_CHECK(strategy, false)                                                      \
    if (!(strategy == COMPRESS_STRATEGY_DEFAULT_STRATEGY || strategy == COMPRESS_STRATEGY_FILTERED || \
            strategy == COMPRESS_STRATEGY_HUFFMAN_ONLY || strategy == COMPRESS_STRATEGY_RLE ||        \
            strategy == COMPRESS_STRATEGY_FIXED)) {                                                   \
        APP_LOGE("strategy parameter= [%{public}d] value is incorrect", (int)strategy);            \
        return ret;                                                                                   \
    }

#define COMPRESS_MEM_CHECK(mem, false)                                                                            \
    if (!(mem == MEM_LEVEL_MIN_MEMLEVEL || mem == MEM_LEVEL_DEFAULT_MEMLEVEL || mem == MEM_LEVEL_MAX_MEMLEVEL)) { \
        APP_LOGE("memLevel parameter =[%{public}d] value is incorrect", (int)mem);                             \
        return ret;                                                                                               \
    }

std::shared_ptr<ZlibCallbackInfo> g_zipAceCallbackInfo = nullptr;
std::shared_ptr<ZlibCallbackInfo> g_unzipAceCallbackInfo = nullptr;
void ZipFileAsyncCallBack(int result);
void ZipFilePromiseCallBack(int result);
void UnzipFileAsyncCallBack(int result);
void UnzipFilePromiseCallBack(int result);
napi_value UnwrapZipParam(CallZipUnzipParam &param, napi_env env, napi_value *args, size_t argc);
napi_value UnwrapUnZipParam(CallZipUnzipParam &param, napi_env env, napi_value *args, size_t argc);
napi_value ZipFileWrap(napi_env env, napi_callback_info info, AsyncZipCallbackInfo *asyncZipCallbackInfo);
napi_value UnwrapStringParam(std::string &str, napi_env env, napi_value args);
bool UnwrapOptionsParams(OPTIONS &options, napi_env env, napi_value arg);
napi_value ZipFileAsync(napi_env env, napi_value *args, size_t argcAsync, AsyncZipCallbackInfo *asyncZipCallbackInfo);
napi_value UnzipFileAsync(napi_env env, napi_value *args, size_t argcAsync, AsyncZipCallbackInfo *asyncZipCallbackInfo);
void ZipAndUnzipFileAsyncCallBack(std::shared_ptr<ZlibCallbackInfo> &zipAceCallbackInfo, int result);
napi_value ZipFilePromise(napi_env env, AsyncZipCallbackInfo *asyncZipCallbackInfo);
napi_value UnzipFilePromise(napi_env env, AsyncZipCallbackInfo *asyncZipCallbackInfo);
void ZipAndUnzipFileAsyncCallBackInnerJsThread(uv_work_t *work, int status);

/**
 * @brief FlushType data initialization.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value FlushTypeInit(napi_env env, napi_value exports)
{
    APP_LOGI("%{public}s called.", __func__);
    const int FLUSH_TYPE_NO_FLUSH = 0;
    const int FLUSH_TYPE_PARTIAL_FLUSH = 1;
    const int FLUSH_TYPE_SYNC_FLUSH = 2;
    const int FLUSH_TYPE_FULL_FLUSH = 3;
    const int FLUSH_TYPE_FINISH = 4;
    const int FLUSH_TYPE_BLOCK = 5;
    const int FLUSH_TYPE_TREES = 6;

    napi_value flushType = nullptr;
    napi_create_object(env, &flushType);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_NO_FLUSH", FLUSH_TYPE_NO_FLUSH);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_PARTIAL_FLUSH", FLUSH_TYPE_PARTIAL_FLUSH);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_SYNC_FLUSH", FLUSH_TYPE_SYNC_FLUSH);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_FULL_FLUSH", FLUSH_TYPE_FULL_FLUSH);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_FINISH", FLUSH_TYPE_FINISH);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_BLOCK", FLUSH_TYPE_BLOCK);
    SetNamedProperty(env, flushType, "FLUSH_TYPE_TREES", FLUSH_TYPE_TREES);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("FlushType", flushType),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}
/**
 * @brief CompressLevel data initialization.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value CompressLevelInit(napi_env env, napi_value exports)
{
    APP_LOGI("%{public}s called.", __func__);
    const int COMPRESS_LEVEL_NO_COMPRESSION = 0;
    const int COMPRESS_LEVEL_BEST_SPEED = 1;
    const int COMPRESS_LEVEL_BEST_COMPRESSION = 9;
    const int COMPRESS_LEVEL_DEFAULT_COMPRESSION = -1;

    napi_value compressLevel = nullptr;
    napi_create_object(env, &compressLevel);
    SetNamedProperty(env, compressLevel, "COMPRESS_LEVEL_NO_COMPRESSION", COMPRESS_LEVEL_NO_COMPRESSION);
    SetNamedProperty(env, compressLevel, "COMPRESS_LEVEL_BEST_SPEED", COMPRESS_LEVEL_BEST_SPEED);
    SetNamedProperty(env, compressLevel, "COMPRESS_LEVEL_BEST_COMPRESSION", COMPRESS_LEVEL_BEST_COMPRESSION);
    SetNamedProperty(env, compressLevel, "COMPRESS_LEVEL_DEFAULT_COMPRESSION", COMPRESS_LEVEL_DEFAULT_COMPRESSION);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("CompressLevel", compressLevel),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}
/**
 * @brief CompressStrategy data initialization.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value CompressStrategyInit(napi_env env, napi_value exports)
{
    APP_LOGI("%{public}s called.", __func__);
    const int COMPRESS_STRATEGY_DEFAULT_STRATEGY = 0;
    const int COMPRESS_STRATEGY_FILTERED = 1;
    const int COMPRESS_STRATEGY_HUFFMAN_ONLY = 2;
    const int COMPRESS_STRATEGY_RLE = 3;
    const int COMPRESS_STRATEGY_FIXED = 4;

    napi_value compressStrategy = nullptr;
    napi_create_object(env, &compressStrategy);
    SetNamedProperty(env, compressStrategy, "COMPRESS_STRATEGY_DEFAULT_STRATEGY", COMPRESS_STRATEGY_DEFAULT_STRATEGY);
    SetNamedProperty(env, compressStrategy, "COMPRESS_STRATEGY_FILTERED", COMPRESS_STRATEGY_FILTERED);
    SetNamedProperty(env, compressStrategy, "COMPRESS_STRATEGY_HUFFMAN_ONLY", COMPRESS_STRATEGY_HUFFMAN_ONLY);
    SetNamedProperty(env, compressStrategy, "COMPRESS_STRATEGY_RLE", COMPRESS_STRATEGY_RLE);
    SetNamedProperty(env, compressStrategy, "COMPRESS_STRATEGY_FIXED", COMPRESS_STRATEGY_FIXED);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("CompressStrategy", compressStrategy),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}
/**
 * @brief MemLevel data initialization.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value MemLevelInit(napi_env env, napi_value exports)
{
    APP_LOGI("%{public}s called.", __func__);
    const int MEM_LEVEL_MIN = 1;
    const int MEM_LEVEL_DEFAULT = 8;
    const int MEM_LEVEL_MAX = 9;

    napi_value memLevel = nullptr;
    napi_create_object(env, &memLevel);
    SetNamedProperty(env, memLevel, "MEM_LEVEL_MIN", MEM_LEVEL_MIN);
    SetNamedProperty(env, memLevel, "MEM_LEVEL_DEFAULT", MEM_LEVEL_DEFAULT);
    SetNamedProperty(env, memLevel, "MEM_LEVEL_MAX", MEM_LEVEL_MAX);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("MemLevel", memLevel),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}
/**
 * @brief Errorcode data initialization.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value ErrorCodeInit(napi_env env, napi_value exports)
{
    APP_LOGI("%{public}s called.", __func__);

    const int ERROR_CODE_OK = 0;
    const int ERROR_CODE_STREAM_END = 1;
    const int ERROR_CODE_NEED_DICT = 2;
    const int ERROR_CODE_ERRNO = -1;
    const int ERROR_CODE_STREAM_ERROR = -2;
    const int ERROR_CODE_DATA_ERROR = -3;
    const int ERROR_CODE_MEM_ERROR = -4;
    const int ERROR_CODE_BUF_ERROR = -5;
    const int ERROR_CODE_VERSION_ERROR = -6;

    napi_value errorCode = nullptr;
    napi_create_object(env, &errorCode);
    SetNamedProperty(env, errorCode, "ERROR_CODE_OK", ERROR_CODE_OK);
    SetNamedProperty(env, errorCode, "ERROR_CODE_STREAM_END", ERROR_CODE_STREAM_END);
    SetNamedProperty(env, errorCode, "ERROR_CODE_NEED_DICT", ERROR_CODE_NEED_DICT);
    SetNamedProperty(env, errorCode, "ERROR_CODE_ERRNO", ERROR_CODE_ERRNO);
    SetNamedProperty(env, errorCode, "ERROR_CODE_STREAM_ERROR", ERROR_CODE_STREAM_ERROR);
    SetNamedProperty(env, errorCode, "ERROR_CODE_DATA_ERROR", ERROR_CODE_DATA_ERROR);
    SetNamedProperty(env, errorCode, "ERROR_CODE_MEM_ERROR", ERROR_CODE_MEM_ERROR);
    SetNamedProperty(env, errorCode, "ERROR_CODE_BUF_ERROR", ERROR_CODE_BUF_ERROR);
    SetNamedProperty(env, errorCode, "ERROR_CODE_VERSION_ERROR", ERROR_CODE_VERSION_ERROR);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("ErrorCode", errorCode),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}

/**
 * @brief FeatureAbility NAPI module registration.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param exports An empty object via the exports parameter as a convenience.
 *
 * @return The return value from Init is treated as the exports object for the module.
 */
napi_value ZlibInit(napi_env env, napi_value exports)
{
    APP_LOGI("%{public}s,called", __func__);

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("zipFile", NAPI_ZipFile),
        DECLARE_NAPI_FUNCTION("unzipFile", NAPI_UnzipFile),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));

    return exports;
}

AsyncZipCallbackInfo *CreateZipAsyncCallbackInfo(napi_env env)
{
    APP_LOGI("%{public}s called.", __func__);
    napi_status ret;
    napi_value global = 0;
    const napi_extended_error_info *errorInfo = nullptr;
    ret = napi_get_global(env, &global);
    if (ret != napi_ok) {
        napi_get_last_error_info(env, &errorInfo);
        if (errorInfo == nullptr) {
            APP_LOGE("%{public}s errorInfo is null", __func__);
            return nullptr;
        }
        APP_LOGE("%{public}s get_global=%{public}d err:%{public}s", __func__, ret, errorInfo->error_message);
    }

    AsyncZipCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncZipCallbackInfo {
        .asyncWork = nullptr,
        .aceCallback = nullptr,
    };
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("%{public}s asyncCallbackInfo is null", __func__);
        return nullptr;
    }
    APP_LOGI("%{public}s end.", __func__);
    return asyncCallbackInfo;
}

/**
 * @brief Zlib NAPI method : zipFile.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_ZipFile(napi_env env, napi_callback_info info)
{
    APP_LOGI("%{public}s,called env=%{public}p", __func__, env);
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    size_t argcAsync = 4;
    const size_t argcPromise = 3;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync < argcPromise || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }

    AsyncZipCallbackInfo *asyncZipCallbackInfo = CreateZipAsyncCallbackInfo(env);
    if (asyncZipCallbackInfo == nullptr) {
        return nullptr;
    }

    ret = ZipFileWrap(env, info, asyncZipCallbackInfo);
    if (ret == nullptr) {

        if (g_zipAceCallbackInfo != nullptr) {
            g_zipAceCallbackInfo.reset();
            g_zipAceCallbackInfo = nullptr;
        }
        if (asyncZipCallbackInfo != nullptr) {
            delete asyncZipCallbackInfo;
            asyncZipCallbackInfo = nullptr;
        }
    }

    return ret;
}

napi_value ZipFileWrap(napi_env env, napi_callback_info info, AsyncZipCallbackInfo *asyncZipCallbackInfo)
{
    APP_LOGI("%{public}s,called", __func__);
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    size_t argcAsync = 4;
    const size_t argcPromise = 3;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync < argcPromise || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }
    CallZipUnzipParam param;
    if (UnwrapZipParam(param, env, args, argcAsync) == nullptr) {
        APP_LOGE("%{public}s, call unwrapWant failed.", __func__);
        return nullptr;
    }
    g_zipAceCallbackInfo = std::make_shared<ZlibCallbackInfo>();
    if (g_zipAceCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, call param failed.", __func__);
        return nullptr;
    }
    asyncZipCallbackInfo->aceCallback = g_zipAceCallbackInfo;
    g_zipAceCallbackInfo->param = param;
    g_zipAceCallbackInfo->env = env;

    if (argcAsync > PARAM3) {
        ret = ZipFileAsync(env, args, argcAsync, asyncZipCallbackInfo);
    } else {
        ret = ZipFilePromise(env, asyncZipCallbackInfo);
    }
    return ret;
}
napi_value ZipFilePromise(napi_env env, AsyncZipCallbackInfo *asyncZipCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncZipCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncZipCallbackInfo is nullptr.", __func__);
        return nullptr;
    }

    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    g_zipAceCallbackInfo->deferred = deferred;
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            APP_LOGI("NAPI_ZipFile_Promise, worker pool thread execute.");
            AsyncZipCallbackInfo *asyncCallbackInfo = static_cast<AsyncZipCallbackInfo *>(data);
            if (asyncCallbackInfo != nullptr && asyncCallbackInfo->aceCallback != nullptr) {
                Zip(FilePath(asyncCallbackInfo->aceCallback->param.src),
                    FilePath(asyncCallbackInfo->aceCallback->param.dest),
                    asyncCallbackInfo->aceCallback->param.options,
                    ZipFilePromiseCallBack,
                    false);
            }
            APP_LOGI("NAPI_ZipFile_Promise, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            APP_LOGI("NAPI_ZipFile_Promise, main event thread complete.");
            AsyncZipCallbackInfo *asyncCallbackInfo = static_cast<AsyncZipCallbackInfo *>(data);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
            APP_LOGI("NAPI_ZipFile_Promise, main event thread complete end.");
        },
        (void *)asyncZipCallbackInfo,
        &asyncZipCallbackInfo->asyncWork);

    napi_queue_async_work(env, asyncZipCallbackInfo->asyncWork);
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}

napi_value UnwrapStringParam(std::string &str, napi_env env, napi_value argv)
{
    APP_LOGI("%{public}s,called", __func__);
    // unwrap the param[0]
    napi_valuetype valueType = napi_valuetype::napi_undefined;
    napi_status rev = napi_typeof(env, argv, &valueType);
    if (rev != napi_ok) {
        return nullptr;
    }

    if (valueType != napi_valuetype::napi_string) {
        APP_LOGI("%{public}s called, Parameter type does not match", __func__);
        return nullptr;
    }

    size_t len;
    napi_status status = napi_get_value_string_utf8(env, argv, nullptr, 0, &len);
    if (status != napi_ok) {
        APP_LOGI("%{public}s called, Get locale tag length failed", __func__);
        return nullptr;
    }
    std::vector<char> buf(len + 1);
    status = napi_get_value_string_utf8(env, argv, buf.data(), len + 1, &len);
    if (status != napi_ok) {
        APP_LOGI("%{public}s called, Get locale tag failed", __func__);
        return nullptr;
    }
    str = std::string(buf.data());

    napi_value result;
    NAPI_CALL(env, napi_create_int32(env, 1, &result));
    return result;
}

bool UnwrapOptionsParams(OPTIONS &options, napi_env env, napi_value arg)
{
    APP_LOGI("%{public}s called.", __func__);

    if (!IsTypeForNapiValue(env, arg, napi_object)) {
        return false;
    }
    napi_valuetype jsValueType = napi_undefined;
    napi_value jsProNameList = nullptr;
    uint32_t jsProCount = 0;

    NAPI_CALL_BASE(env, napi_get_property_names(env, arg, &jsProNameList), false);
    NAPI_CALL_BASE(env, napi_get_array_length(env, jsProNameList, &jsProCount), false);
    APP_LOGI("%{public}s called. Property size=%{public}d.", __func__, jsProCount);

    napi_value jsProName = nullptr;
    napi_value jsProValue = nullptr;

    for (uint32_t index = 0; index < jsProCount; index++) {
        NAPI_CALL_BASE(env, napi_get_element(env, jsProNameList, index, &jsProName), false);
        std::string strProName = UnwrapStringFromJS(env, jsProName, std::string());
        APP_LOGI("%{public}s called. Property name=%{public}s.", __func__, strProName.c_str());
        NAPI_CALL_BASE(env, napi_get_named_property(env, arg, strProName.c_str(), &jsProValue), false);
        NAPI_CALL_BASE(env, napi_typeof(env, jsProValue, &jsValueType), false);

        int ret = 0;
        if (strProName == std::string("flush")) {
            NAPI_CALL_BASE_BOOL(UnwrapIntValue(env, jsProValue, ret), false);
            options.flush = static_cast<FLUSH_TYPE>(ret);
        } else if (strProName == std::string("finishFlush")) {
            NAPI_CALL_BASE_BOOL(UnwrapIntValue(env, jsProValue, ret), false);
            options.finishFlush = static_cast<FLUSH_TYPE>(ret);
        } else if (strProName == std::string("chunkSize")) {
            NAPI_CALL_BASE_BOOL(UnwrapIntValue(env, jsProValue, ret), false);
            options.chunkSize = ret;
        } else if (strProName == std::string("level")) {
            NAPI_CALL_BASE_BOOL(UnwrapIntValue(env, jsProValue, ret), false);
            COMPRESS_LEVE_CHECK(ret, false)
            options.level = static_cast<COMPRESS_LEVEL>(ret);
        } else if (strProName == std::string("memLevel")) {
            NAPI_CALL_BASE_BOOL(UnwrapIntValue(env, jsProValue, ret), false);
            COMPRESS_MEM_CHECK(ret, false)
            options.memLevel = static_cast<MEMORY_LEVEL>(ret);
        } else if (strProName == std::string("strategy")) {
            NAPI_CALL_BASE_BOOL(UnwrapIntValue(env, jsProValue, ret), false);
            COMPRESS_STRATEGY_CHECK(ret, false)
            options.strategy = static_cast<COMPRESS_STRATEGY>(ret);
        } else {
            continue;
        }
    }
    return true;
}

napi_value UnwrapZipParam(CallZipUnzipParam &param, napi_env env, napi_value *args, size_t argc)
{
    APP_LOGI("%{public}s,called", __func__);
    size_t argcPromise = 3;
    if (argc < argcPromise) {
        APP_LOGI("%{public}s called, param count is wrong", __func__);
        return nullptr;
    }

    // unwrap the param[0]
    if (UnwrapStringParam(param.src, env, args[0]) == nullptr) {
        APP_LOGI("%{public}s called, args[0] error", __func__);
        return nullptr;
    }

    // unwrap the param[1]
    if (UnwrapStringParam(param.dest, env, args[1]) == nullptr) {
        APP_LOGI("%{public}s called, args[1] error", __func__);
        return nullptr;
    }

    // unwrap the param[2]
    if (!UnwrapOptionsParams(param.options, env, args[2])) {
        APP_LOGI("%{public}s called, args[2] error", __func__);
        return nullptr;
    }
    // create reutrn
    napi_value ret = 0;
    NAPI_CALL_BASE(env, napi_create_int32(env, 0, &ret), nullptr);
    return ret;
}

napi_value UnwrapUnZipParam(CallZipUnzipParam &param, napi_env env, napi_value *args, size_t argc)
{
    APP_LOGI("%{public}s,called", __func__);
    size_t argcPromise = 3;
    if (argc < argcPromise) {
        return nullptr;
    }
    // unwrap the param[0]
    if (UnwrapStringParam(param.src, env, args[0]) == nullptr) {
        return nullptr;
    }

    // unwrap the param[1]
    if (UnwrapStringParam(param.dest, env, args[1]) == nullptr) {
        return nullptr;
    }

    // create reutrn
    napi_value ret = 0;
    NAPI_CALL_BASE(env, napi_create_int32(env, 0, &ret), nullptr);
    return ret;
}
/**
 * @brief ZipFileAsync
 *
 * @param param Indicates the parameters saved the parse result.
 * @param env The environment that the Node-API call is invoked under.
 * @param args Indicates the arguments passed into the callback.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */

napi_value ZipFileAsync(napi_env env, napi_value *args, size_t argcAsync, AsyncZipCallbackInfo *asyncZipCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncZipCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncZipCallbackInfo == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL_BASE(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName), nullptr);

    if (argcAsync > PARAM3) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL_BASE(env, napi_typeof(env, args[PARAM3], &valuetype), nullptr);
        if (valuetype == napi_function) {
            // resultCallback: AsyncCallback<ZipRestult>
            napi_create_reference(env, args[PARAM3], 1, &g_zipAceCallbackInfo->callback);
        } else {
            APP_LOGE("%{public}s, args[3] error. It should be a function type.", __func__);
            return nullptr;
        }
    }
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            APP_LOGI("NAPI_ZipFile_callback, worker pool thread execute.");
            AsyncZipCallbackInfo *asyncCallbackInfo = (AsyncZipCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr && asyncCallbackInfo->aceCallback != nullptr) {
                Zip(FilePath(asyncCallbackInfo->aceCallback->param.src),
                    FilePath(asyncCallbackInfo->aceCallback->param.dest),
                    asyncCallbackInfo->aceCallback->param.options,
                    ZipFileAsyncCallBack,
                    false);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            APP_LOGI("NAPI_ZipFile_callback, main event thread complete.");
            AsyncZipCallbackInfo *asyncCallbackInfo = (AsyncZipCallbackInfo *)data;
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncZipCallbackInfo,
        &asyncZipCallbackInfo->asyncWork);

    napi_queue_async_work(env, asyncZipCallbackInfo->asyncWork);
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}

/**
 * @brief Zlib NAPI method : unzipFile.
 *
 * @param env The environment that the Node-API call is invoked under.
 * @param info The callback info passed into the callback function.
 *
 * @return The return value from NAPI C++ to JS for the module.
 */
napi_value NAPI_UnzipFile(napi_env env, napi_callback_info info)
{
    APP_LOGI("%{public}s,called", __func__);
    napi_value args[ARGS_MAX_COUNT] = {nullptr};
    napi_value ret = 0;
    size_t argcAsync = 4;
    const size_t argcPromise = 3;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argcAsync, args, nullptr, nullptr));
    if (argcAsync < argcPromise || argcAsync > ARGS_MAX_COUNT) {
        APP_LOGE("%{public}s, Wrong argument count.", __func__);
        return nullptr;
    }
    // parse param
    CallZipUnzipParam param;
    if (UnwrapUnZipParam(param, env, args, argcAsync) == nullptr) {
        APP_LOGE("%{public}s, call unwrap param failed.", __func__);
        return nullptr;
    }

    AsyncZipCallbackInfo *asyncZipCallbackInfo = CreateZipAsyncCallbackInfo(env);
    if (asyncZipCallbackInfo == nullptr) {
        return nullptr;
    }

    g_unzipAceCallbackInfo = std::make_shared<ZlibCallbackInfo>();
    asyncZipCallbackInfo->aceCallback = g_unzipAceCallbackInfo;
    g_unzipAceCallbackInfo->param = param;
    g_unzipAceCallbackInfo->env = env;

    if (argcAsync > PARAM3) {
        ret = UnzipFileAsync(env, args, argcAsync, asyncZipCallbackInfo);
    } else {
        ret = UnzipFilePromise(env, asyncZipCallbackInfo);
    }
    if (ret == nullptr) {
        APP_LOGE("%{public}s,ret == nullptr", __func__);
        if (g_unzipAceCallbackInfo != nullptr) {
            g_unzipAceCallbackInfo.reset();
            g_unzipAceCallbackInfo = nullptr;
        }
        if (asyncZipCallbackInfo != nullptr) {
            napi_delete_async_work(env, asyncZipCallbackInfo->asyncWork);
            delete asyncZipCallbackInfo;
            asyncZipCallbackInfo = nullptr;
        }
    }
    APP_LOGI("%{public}s,end", __func__);
    return ret;
}
napi_value UnzipFilePromise(napi_env env, AsyncZipCallbackInfo *asyncZipCallbackInfo)
{
    APP_LOGI("%{public}s, promise.", __func__);
    if (asyncZipCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, asyncZipCallbackInfo is nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    NAPI_CALL(env, napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName));
    napi_deferred deferred;
    napi_value promise = 0;
    NAPI_CALL(env, napi_create_promise(env, &deferred, &promise));
    g_unzipAceCallbackInfo->deferred = deferred;
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            APP_LOGI("NAPI_UnzipFile_Promise, worker pool thread execute.");
            AsyncZipCallbackInfo *asyncCallbackInfo = static_cast<AsyncZipCallbackInfo *>(data);
            if (asyncCallbackInfo != nullptr && asyncCallbackInfo->aceCallback != nullptr) {
                Unzip(FilePath(asyncCallbackInfo->aceCallback->param.src),
                    FilePath(asyncCallbackInfo->aceCallback->param.dest),
                    asyncCallbackInfo->aceCallback->param.options,
                    UnzipFilePromiseCallBack);
            }
            APP_LOGI("NAPI_UnzipFile_Promise, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            APP_LOGI("NAPI_UnzipFile_Promise, main event thread complete.");
            AsyncZipCallbackInfo *asyncCallbackInfo = static_cast<AsyncZipCallbackInfo *>(data);
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
            APP_LOGI("NAPI_UnzipFile_Promise, main event thread complete end.");
        },
        (void *)asyncZipCallbackInfo,
        &asyncZipCallbackInfo->asyncWork);
    napi_queue_async_work(env, asyncZipCallbackInfo->asyncWork);
    APP_LOGI("%{public}s, promise end.", __func__);
    return promise;
}
napi_value UnzipFileAsync(napi_env env, napi_value *args, size_t argcAsync, AsyncZipCallbackInfo *asyncZipCallbackInfo)
{
    APP_LOGI("%{public}s, asyncCallback.", __func__);
    if (args == nullptr || asyncZipCallbackInfo == nullptr) {
        APP_LOGE("%{public}s, param == nullptr.", __func__);
        return nullptr;
    }
    napi_value resourceName = 0;
    napi_create_string_latin1(env, __func__, NAPI_AUTO_LENGTH, &resourceName);
    if (argcAsync > PARAM3) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL_BASE(env, napi_typeof(env, args[PARAM3], &valuetype), nullptr);
        if (valuetype == napi_function) {
            // resultCallback: AsyncCallback<ZipRestult>
            napi_create_reference(env, args[PARAM3], 1, &g_unzipAceCallbackInfo->callback);
        } else {
            APP_LOGE("%{public}s, args[3] error. It should be a function type.", __func__);
            return nullptr;
        }
    }
    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            APP_LOGI("NAPI_UnzipFile, worker pool thread execute.");
            AsyncZipCallbackInfo *asyncCallbackInfo = (AsyncZipCallbackInfo *)data;
            // Unzip
            if (asyncCallbackInfo != nullptr && asyncCallbackInfo->aceCallback != nullptr) {
                Unzip(FilePath(asyncCallbackInfo->aceCallback->param.src),
                    FilePath(asyncCallbackInfo->aceCallback->param.dest),
                    asyncCallbackInfo->aceCallback->param.options,
                    UnzipFileAsyncCallBack);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            APP_LOGI("NAPI_UnzipFile, main event thread complete.");
            AsyncZipCallbackInfo *asyncCallbackInfo = (AsyncZipCallbackInfo *)data;
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncZipCallbackInfo,
        &asyncZipCallbackInfo->asyncWork);

    napi_queue_async_work(env, asyncZipCallbackInfo->asyncWork);
    napi_value result = 0;
    napi_get_null(env, &result);
    return result;
}
// ZipFile callback
void ZipFileAsyncCallBack(int result)
{
    if (g_zipAceCallbackInfo != nullptr) {
        g_zipAceCallbackInfo->isCallBack = true;
    }
    ZipAndUnzipFileAsyncCallBack(g_zipAceCallbackInfo, result);
}
// ZipFile Promise
void ZipFilePromiseCallBack(int result)
{
    if (g_zipAceCallbackInfo != nullptr) {
        g_zipAceCallbackInfo->isCallBack = false;
    }
    ZipAndUnzipFileAsyncCallBack(g_zipAceCallbackInfo, result);
}
// UnzipFile callback
void UnzipFileAsyncCallBack(int result)
{
    if (g_unzipAceCallbackInfo != nullptr) {
        g_unzipAceCallbackInfo->isCallBack = true;
    }
    ZipAndUnzipFileAsyncCallBack(g_unzipAceCallbackInfo, result);
}
// UnzipFile Promise
void UnzipFilePromiseCallBack(int result)
{
    if (g_unzipAceCallbackInfo != nullptr) {
        g_unzipAceCallbackInfo->isCallBack = false;
    }
    ZipAndUnzipFileAsyncCallBack(g_unzipAceCallbackInfo, result);
}
void ZipAndUnzipFileAsyncCallBackInnerJsThread(uv_work_t *work)
{
    if (work == nullptr) {
        return;
    }
    // JS Thread
    ZlibCallbackInfo *asyncCallbackInfo = (ZlibCallbackInfo *)work->data;
    if (asyncCallbackInfo == nullptr) {
        return;
    }
    napi_value result[ARGS_TWO] = {0};
    // callback result
    napi_create_int32(asyncCallbackInfo->env, (int32_t)asyncCallbackInfo->callbackResult, &result[PARAM1]);
    APP_LOGI("%{public}s called, callbackResult =%{public}d", __func__, asyncCallbackInfo->callbackResult);

    if (asyncCallbackInfo->isCallBack) {
        napi_value callback = 0;
        napi_value undefined = 0;
        // callback(err, data)  errorInfo->error_code
        result[PARAM0] = GetCallbackErrorValue(asyncCallbackInfo->env, NO_ERROR);
        // get callback
        napi_get_reference_value(asyncCallbackInfo->env, asyncCallbackInfo->callback, &callback);
        napi_get_undefined(asyncCallbackInfo->env, &undefined);
        // call callback
        napi_value jsResult = 0;
        napi_call_function(asyncCallbackInfo->env, undefined, callback, ARGS_TWO, &result[PARAM0], &jsResult);
        // free data
        if (asyncCallbackInfo->callback != nullptr) {
            napi_delete_reference(asyncCallbackInfo->env, asyncCallbackInfo->callback);
        }
    } else {
        // promise
        napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result[PARAM1]);
    }
    if (asyncCallbackInfo != nullptr) {
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
    }
    if (work != nullptr) {
        delete work;
        work = nullptr;
    }
}
void ZipAndUnzipFileAsyncCallBack(std::shared_ptr<ZlibCallbackInfo> &zipAceCallbackInfo, int result)
{
    if (zipAceCallbackInfo == nullptr) {
        return;
    }
    APP_LOGI("%{public}s,called env=%{public}p, result=%{public}d", __func__, zipAceCallbackInfo->env, result);
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(zipAceCallbackInfo->env, &loop);
    if (loop == nullptr) {
        APP_LOGE("%{public}s, work == nullptr.", __func__);
        return;
    }
    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        APP_LOGE("%{public}s, work == nullptr.", __func__);
        return;
    }
    ZlibCallbackInfo *asyncCallbackInfo = new (std::nothrow) ZlibCallbackInfo();
    if (asyncCallbackInfo == nullptr) {
        delete work;
        work = nullptr;
        return;
    }
    *asyncCallbackInfo = *zipAceCallbackInfo;
    asyncCallbackInfo->callbackResult = result;
    zipAceCallbackInfo.reset();
    zipAceCallbackInfo = nullptr;
    work->data = (void *)asyncCallbackInfo;
    int rev = uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            APP_LOGI("ZipAndUnzipFileAsyncCallBack, uv_queue_work");
            ZipAndUnzipFileAsyncCallBackInnerJsThread(work);
            APP_LOGI("ZipAndUnzipFileAsyncCallBack, uv_queue_work end.");
        });
    if (rev != napi_ok) {
        if (asyncCallbackInfo->isCallBack) {
            if (asyncCallbackInfo->callback != nullptr) {
                napi_delete_reference(asyncCallbackInfo->env, asyncCallbackInfo->callback);
            }
        }
        if (asyncCallbackInfo != nullptr) {
            delete asyncCallbackInfo;
            asyncCallbackInfo = nullptr;
        }
        if (work != nullptr) {
            delete work;
            work = nullptr;
        }
    }
}

}  // namespace LIBZIP
}  // namespace AppExecFwk
}  // namespace OHOS
