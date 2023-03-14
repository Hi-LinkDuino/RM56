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

#include "frameworks/bridge/declarative_frontend/engine/quickjs/modules/qjs_router_module.h"

#include "base/log/log.h"
#include "frameworks/bridge/declarative_frontend/engine/quickjs/qjs_declarative_engine.h"
#include "frameworks/bridge/js_frontend/engine/common/js_constants.h"
#include "frameworks/bridge/js_frontend/engine/quickjs/qjs_utils.h"

namespace OHOS::Ace::Framework {

std::string JsParseRouteUrl(JSContext* ctx, JSValueConst valObject, const std::string& routerKey)
{
    std::string routeUrl;
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return routeUrl;
    }
    for (uint32_t i = 0; i < len; ++i) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue valItem = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsString(valItem)) {
            ScopedString styleVal(ctx, valItem);
            const char* valStr = styleVal.get();
            if (strcmp(key, routerKey.c_str()) == 0) {
                routeUrl = valStr;
                LOGD("routeUrl : %{public}s ", routeUrl.c_str());
            } else {
                LOGE("routeUrl : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGE("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, valItem);
    }
    js_free(ctx, pTab);
    return routeUrl;
}

std::string JsParseRouteParams(JSContext* ctx, JSValueConst valObject, const std::string& paramsKey)
{
    std::string routeParams;
    JSPropertyEnum* pTab = nullptr;
    uint32_t len = 0;
    if (!CheckAndGetJsProperty(ctx, valObject, &pTab, &len)) {
        return routeParams;
    }
    for (uint32_t i = 0; i < len; ++i) {
        const char* key = JS_AtomToCString(ctx, pTab[i].atom);
        if (key == nullptr) {
            JS_FreeAtom(ctx, pTab[i].atom);
            LOGW("key is null. Ignoring!");
            continue;
        }
        JSValue valItem = JS_GetProperty(ctx, valObject, pTab[i].atom);
        if (JS_IsObject(valItem)) {
            if (strcmp(key, paramsKey.c_str()) == 0) {
                routeParams = ScopedString::Stringify(valItem);
                LOGD("routeParams : %{public}s ", routeParams.c_str());
            } else {
                LOGE("routeParams : %{public}s unsupported. Ignoring!", key);
            }
        } else {
            LOGE("value of unsupported type. Ignoring!");
        }
        JS_FreeAtom(ctx, pTab[i].atom);
        JS_FreeCString(ctx, key);
        JS_FreeValue(ctx, valItem);
    }
    js_free(ctx, pTab);
    return routeParams;
}

JSValue PagePush(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1) {
        LOGE("PagePush args count is invalid");
        return JS_NULL;
    }

    std::string uri = JsParseRouteUrl(ctx, argv[0], ROUTE_KEY_URI);
    std::string params = JsParseRouteParams(ctx, argv[0], ROUTE_KEY_PARAMS);
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->Push(uri, params);
    return JS_NULL;
}

JSValue PageReplace(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1) {
        LOGE("PageReplace args count is invalid");
        return JS_NULL;
    }

    std::string uri = JsParseRouteUrl(ctx, argv[0], ROUTE_KEY_URI);
    std::string params = JsParseRouteParams(ctx, argv[0], ROUTE_KEY_PARAMS);
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->Replace(uri, params);
    return JS_NULL;
}

JSValue PageBack(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    if (argc != 1 && argc != 0) {
        LOGE("PageBack args count is invalid");
        return JS_NULL;
    }

    std::string uri = JsParseRouteUrl(ctx, argv[0], ROUTE_KEY_URI);
    std::string params = JsParseRouteParams(ctx, argv[0], ROUTE_KEY_PARAMS);
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->Back(uri, params);
    return JS_NULL;
}

JSValue PageClear(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->Clear();
    return JS_NULL;
}

JSValue PageGetLength(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    int32_t routeLength = instance->GetDelegate()->GetStackSize();
    return JS_NewString(ctx, std::to_string(routeLength).c_str());
}

JSValue PageGetState(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    int32_t routeIndex = 0;
    std::string routeName;
    std::string routePath;
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->GetState(routeIndex, routeName, routePath);
    JSValue jsState = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, jsState, "index", JS_NewInt32(ctx, routeIndex));
    JS_SetPropertyStr(ctx, jsState, "name", JS_NewString(ctx, routeName.c_str()));
    JS_SetPropertyStr(ctx, jsState, "path", JS_NewString(ctx, routePath.c_str()));
    return jsState;
}

JSValue PageGetParams(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    std::string paramsStr = instance->GetDelegate()->GetParams();
    if (paramsStr.empty()) {
        LOGI("PageGetParams params is null");
        return JS_NULL;
    }

    JSValue paramsRes = JS_ParseJSON(ctx, paramsStr.c_str(), paramsStr.length(), nullptr);
    return paramsRes;
}

JSValue PostponePageTransition(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->PostponePageTransition();
    return JS_NULL;
}

JSValue LaunchPageTransition(JSContext* ctx, JSValueConst value, int32_t argc, JSValueConst* argv)
{
    auto* instance = static_cast<QJSDeclarativeEngineInstance*>(JS_GetContextOpaque(ctx));
    if (instance == nullptr) {
        LOGE("Can not cast Context to QJSDeclarativeEngineInstance object.");
        return JS_NULL;
    }
    instance->GetDelegate()->LaunchPageTransition();
    return JS_NULL;
}

void InitRouterModule(JSContext* ctx, JSValue& moduleObj)
{
    JS_SetPropertyStr(ctx, moduleObj, ROUTE_PAGE_PUSH, JS_NewCFunction(ctx, PagePush, ROUTE_PAGE_PUSH, 1));
    JS_SetPropertyStr(ctx, moduleObj, ROUTE_PAGE_REPLACE, JS_NewCFunction(ctx, PageReplace, ROUTE_PAGE_REPLACE, 1));
    JS_SetPropertyStr(ctx, moduleObj, ROUTE_PAGE_BACK, JS_NewCFunction(ctx, PageBack, ROUTE_PAGE_BACK, 1));
    JS_SetPropertyStr(ctx, moduleObj, ROUTE_PAGE_CLEAR, JS_NewCFunction(ctx, PageClear, ROUTE_PAGE_CLEAR, 0));
    JS_SetPropertyStr(
        ctx, moduleObj, ROUTE_PAGE_GET_LENGTH, JS_NewCFunction(ctx, PageGetLength, ROUTE_PAGE_GET_LENGTH, 0));
    JS_SetPropertyStr(
        ctx, moduleObj, ROUTE_PAGE_GET_STATE, JS_NewCFunction(ctx, PageGetState, ROUTE_PAGE_GET_STATE, 0));
    JS_SetPropertyStr(
        ctx, moduleObj, ROUTE_PAGE_GET_PARAMS, JS_NewCFunction(ctx, PageGetParams, ROUTE_PAGE_GET_PARAMS, 0));
    JS_SetPropertyStr(
        ctx, moduleObj, ROUTE_POSTPONE, JS_NewCFunction(ctx, PostponePageTransition, ROUTE_POSTPONE, 0));
}
} // namespace OHOS::Ace::Framework
