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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_helpers.h"

#include "base/log/log.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

enum class JsLogLevel : int32_t {
    DEBUG = 0,
    INFO,
    WARNING,
    ERROR,
};

std::string ParseLogContent(JSContext* ctx, int32_t argc, JSValueConst* argv)
{
    std::string formatStr = ScopedString(ctx, argv[0]).get();
    if (argc == 1 || formatStr.empty()) {
        return formatStr;
    }
    std::string ret;
    int32_t len = static_cast<int32_t>(formatStr.size());
    int32_t pos = 0;
    int32_t count = 1;
    for (; pos < len; ++pos) {
        if (count >= argc) {
            break;
        }
        if (formatStr[pos] == '%') {
            if (pos + 1 >= len) {
                break;
            }
            switch (formatStr[pos + 1]) {
                case 's':
                case 'j':
                case 'd':
                case 'O':
                case 'o':
                case 'i':
                case 'f':
                case 'c':
                    ret += ScopedString(ctx, argv[count++]).get();
                    ++pos;
                    break;
                case '%':
                    ret += formatStr[pos];
                    ++pos;
                    break;
                default:
                    ret += formatStr[pos];
                    break;
            }
        } else {
            ret += formatStr[pos];
        }
    }
    if (pos < len) {
        ret += formatStr.substr(pos, len - pos);
    }
    return ret;
}

JSValue JsLogPrint(JSContext* ctx, JsLogLevel level, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // Should have 1 parameters.
    if ((argv == nullptr) || (argc == 0)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }
    std::string printLog = ParseLogContent(ctx, argc, argv);
    switch (level) {
        case JsLogLevel::DEBUG:
            LOGD("ace Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::INFO:
            LOGI("ace Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::WARNING:
            LOGW("ace Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::ERROR:
            LOGE("ace Log: %{public}s", printLog.c_str());
            break;
        default:
            break;
    }

    return JS_NULL;
}

JSValue JsLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue JsDebugLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue JsInfoLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::INFO, value, argc, argv);
}

JSValue JsWarnLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::WARNING, value, argc, argv);
}

JSValue JsErrorLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return JsLogPrint(ctx, JsLogLevel::ERROR, value, argc, argv);
}

JSValue AppLogPrint(JSContext* ctx, JsLogLevel level, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    // Should have 1 parameters.
    if ((argv == nullptr) || (argc == 0)) {
        LOGE("the arg is error");
        return JS_EXCEPTION;
    }
    std::string printLog = ParseLogContent(ctx, argc, argv);
    switch (level) {
        case JsLogLevel::DEBUG:
            APP_LOGD("app Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::INFO:
            APP_LOGI("app Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::WARNING:
            APP_LOGW("app Log: %{public}s", printLog.c_str());
            break;
        case JsLogLevel::ERROR:
            APP_LOGE("app Log: %{public}s", printLog.c_str());
            break;
        default:
            break;
    }

    return JS_NULL;
}

JSValue AppLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue AppDebugLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::DEBUG, value, argc, argv);
}

JSValue AppInfoLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::INFO, value, argc, argv);
}

JSValue AppWarnLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::WARNING, value, argc, argv);
}

JSValue AppErrorLogPrint(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    return AppLogPrint(ctx, JsLogLevel::ERROR, value, argc, argv);
}

void InitConsole(JSContext* ctx)
{
    JSValue globalObj;
    globalObj = JS_GetGlobalObject(ctx);
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, AppLogPrint, "log", 1));
    JS_SetPropertyStr(ctx, console, "debug", JS_NewCFunction(ctx, AppDebugLogPrint, "debug", 1));
    JS_SetPropertyStr(ctx, console, "info", JS_NewCFunction(ctx, AppInfoLogPrint, "info", 1));
    JS_SetPropertyStr(ctx, console, "warn", JS_NewCFunction(ctx, AppWarnLogPrint, "warn", 1));
    JS_SetPropertyStr(ctx, console, "error", JS_NewCFunction(ctx, AppErrorLogPrint, "error", 1));
    JS_SetPropertyStr(ctx, globalObj, "console", console);

    // js framework log method
    JSValue aceConsole = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, aceConsole, "log", JS_NewCFunction(ctx, JsLogPrint, "log", 1));
    JS_SetPropertyStr(ctx, aceConsole, "debug", JS_NewCFunction(ctx, JsDebugLogPrint, "debug", 1));
    JS_SetPropertyStr(ctx, aceConsole, "info", JS_NewCFunction(ctx, JsInfoLogPrint, "info", 1));
    JS_SetPropertyStr(ctx, aceConsole, "warn", JS_NewCFunction(ctx, JsWarnLogPrint, "warn", 1));
    JS_SetPropertyStr(ctx, aceConsole, "error", JS_NewCFunction(ctx, JsErrorLogPrint, "error", 1));
    JS_SetPropertyStr(ctx, globalObj, "aceConsole", aceConsole);
    JS_FreeValue(ctx, globalObj);
}

} // namespace OHOS::Ace::Framework
