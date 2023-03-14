/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_QJS_UTILS_H
#define FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_QJS_UTILS_H

#include <cassert>
#include <cstring>
#include <stack>
#include <vector>

#include "frameworks/bridge/js_frontend/js_ace_page.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "third_party/quickjs/cutils.h"
#include "third_party/quickjs/quickjs-libc.h"
#ifdef __cplusplus
}
#endif

namespace OHOS::Ace::Framework {

enum class JsErrorType {
    JS_CRASH = 0,
    JS_CALLBACK_ERROR,
    FIRE_EVENT_ERROR,
    EVAL_BUFFER_ERROR,
    READ_OBJECT_ERROR,
    DESTROY_APP_ERROR,
    DESTROY_PAGE_ERROR,
    LOAD_JS_BUNDLE_ERROR,
    COMPILE_AND_RUN_BUNDLE_ERROR,
    LOAD_JS_FRAMEWORK_ERROR,
};

class ScopedString {
public:
    ScopedString(JSContext* ctx, JSValueConst val);
    ScopedString(JSContext* ctx, JSAtom val);
    explicit ScopedString(JSValueConst val);
    explicit ScopedString(JSAtom val);
    ScopedString(const ScopedString& rhs) = delete;
    ScopedString& operator=(const ScopedString& rhs) = delete;
    ScopedString(ScopedString&& rhs) = delete;
    ScopedString& operator=(ScopedString&& rhs) = delete;
    ~ScopedString();

    bool operator==(const char* rhs);
    bool operator==(const std::string& rhs);
    bool operator==(const ScopedString& rhs);

    const char* get() const;
    std::string str();

    static std::string Stringify(JSValueConst val);
    static std::string Stringify(JSContext* ctx, JSValueConst val);

    operator std::string() const;

private:
    JSContext* context_ = nullptr;
    const char* stringValue_ = nullptr;
};

class QJSUtils {
public:
    using FilterFunction = bool (*)(std::string);

    static JSValue NewStringLen(JSContext* ctx, const char* str, size_t len);
    static JSValue NewString(JSContext* ctx, const char* str);
    static JSValue ParseJSON(JSContext* ctx, const char* buf, size_t bufLen, const char* filename = nullptr);
    static JSValue NewObject(JSContext* ctx);
    static JSValue Call(JSContext* ctx, JSValueConst funcObj, JSValueConst thisObj, int32_t argc, JSValueConst* argv);
    static JSValue Eval(JSContext* ctx, const char* input, size_t inputLen, const char* filename, int32_t evalFlags);
    static JSValue GetPropertyStr(JSContext* ctx, JSValueConst thisObj, const char* prop);
    static void JsStdDumpErrorAce(JSContext* ctx, JsErrorType errorType = JsErrorType::JS_CRASH, int32_t instanceId = 0,
        const char* pageUrl = nullptr, const RefPtr<JsAcePage>& page = nullptr);
    static void ExtractEachInfo(const std::string& tempStack, std::vector<std::string>& res);
    static void GetPosInfo(const std::string& temp, int32_t start, std::string& line);
    static std::string GetSourceInfo(const std::string& line, const RefPtr<RevSourceMap>& pageMap,
        const RefPtr<RevSourceMap>& appMap, bool isAppPage);
    static void JsDumpMemoryStats(JSContext* ctx);
    static int32_t JsGetArrayLength(JSContext* ctx, JSValueConst arrayObject);

    static std::vector<std::string> GetObjectKeys(JSContext* ctx, JSValueConst obj, int flags = JS_GPN_STRING_MASK);
    static std::vector<std::string> GetObjectKeys(JSValueConst obj, int flags = JS_GPN_STRING_MASK);
    static std::vector<std::string> GetFilteredObjectKeys(
        JSContext* ctx, JSValueConst obj, FilterFunction filterFunc, int flags = JS_GPN_STRING_MASK);
    static bool CheckAndGetJsProperty(JSContext* ctx, JSValueConst jsObj, JSPropertyEnum** pTab, uint32_t* len);
    static std::string typeAsString(JSValueConst val);
    static JSValue GetArgvSafe(int idx, int argc, JSValueConst* argv);
    static void DefineGlobalFunction(JSContext* ctx, JSCFunction jsFunc, const char* name, const int paramNum);
    static std::string JsDumpSourceFile(const char* stack, const RefPtr<RevSourceMap>& pageMap,
        const RefPtr<RevSourceMap>& appMap);
};

/**
 * @brief A class maintaining a stack of JSContext pointers.
 *
 */
class QJSContext {
public:
    /**
     * @brief A class that acquires a JSContext to be retrieved further in a call stack by QJSContext::Current().
     *
     * This is used to relieve the QJS embedder of passing JSContext* as an argument.
     *
     * @note QJSContext::Scope is thread-safe, but not reentrant.
     */
    class Scope {
    public:
        explicit Scope(JSContext* ctx);
        ~Scope();

        static void* operator new(size_t) = delete;
        static void* operator new[](size_t) = delete;
        static void operator delete(void*) = delete;
        static void operator delete[](void*) = delete;
        Scope(const Scope&) = delete;
        void operator=(const Scope&) = delete;
    };

    /**
     * @brief Get the context on the top of the stack
     *
     * @return JSContext*
     */
    static JSContext* Current();

    static void* operator new(size_t) = delete;
    static void* operator new[](size_t) = delete;
    static void operator delete(void*) = delete;
    static void operator delete[](void*) = delete;
    QJSContext(const QJSContext&) = delete;
    void operator=(const QJSContext&) = delete;

private:
#if defined(USE_CLANG_COVERAGE) || defined(CLANG_COVERAGE)
    static std::stack<JSContext*> s_qjsContextStack;
#else
    static thread_local std::stack<JSContext*> s_qjsContextStack;
#endif
};

class QJSHandleScope {
public:
    explicit QJSHandleScope(JSContext* ctx);
    ~QJSHandleScope();

    static void* operator new(size_t) = delete;
    static void* operator new[](size_t) = delete;
    static void operator delete(void*) = delete;
    static void operator delete[](void*) = delete;

private:
    friend class QJSUtils;
    friend class QJSValue;
#if defined(USE_CLANG_COVERAGE) || defined(CLANG_COVERAGE)
    static std::stack<QJSHandleScope*> qjsHandleScopeStack_;
#else
    static thread_local std::stack<QJSHandleScope*> qjsHandleScopeStack_;
#endif

    static QJSHandleScope* GetCurrent();
    void Push(JSValue val);

    int32_t scopeId_ = 0;
    std::vector<JSValueConst> jsValues_;
    JSContext* context_ = nullptr;
};

inline bool CheckAndGetJsProperty(JSContext* ctx, JSValueConst fromMap, JSPropertyEnum** pTab, uint32_t* len)
{
    if (!JS_IsObject(fromMap)) {
        return false;
    }

    JS_GetOwnPropertyNames(ctx, pTab, len, fromMap, JS_GPN_STRING_MASK);
    if (*pTab == nullptr) {
        return false;
    }
    return true;
}

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_FRAMEWORKS_BRIDGE_JS_FRONTEND_ENGINE_QUICKJS_QJS_UTILS_H
