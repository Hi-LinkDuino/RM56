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
#include <algorithm>
#include <map>
#include <mutex>
#include <set>

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

#include "base/log/log.h"
#include "bridge/common/media_query/media_queryer.h"
#include "bridge/common/utils/engine_helper.h"
#include "bridge/js_frontend/engine/common/js_engine.h"

namespace OHOS::Ace::Napi {
namespace {
constexpr size_t STR_BUFFER_SIZE = 1024;
}

using namespace OHOS::Ace::Framework;
struct MediaQueryResult {
    bool matches_ = false;
    std::string media_;

    MediaQueryResult(bool match, const std::string& media) : matches_(match), media_(media) {}
    virtual ~MediaQueryResult() = default;
    virtual void NapiSerilizer(napi_env& env, napi_value& result)
    {
        /* construct a MediaQueryListener object */
        napi_create_object(env, &result);

        napi_value matchesVal = nullptr;
        napi_get_boolean(env, matches_, &matchesVal);
        napi_set_named_property(env, result, "matches", matchesVal);

        napi_value mediaVal = nullptr;
        napi_create_string_utf8(env, media_.c_str(), media_.size(), &mediaVal);
        napi_set_named_property(env, result, "media", mediaVal);
    }
};

class MediaQueryListener : public MediaQueryResult {
public:
    MediaQueryListener(bool match, const std::string& media) : MediaQueryResult(match, media) {}
    ~MediaQueryListener() override
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        auto iter = listenerSets_.begin();
        while (iter != listenerSets_.end()) {
            iter->second.erase(this);
            if (iter->second.empty()) {
                iter->first->UnregistMediaUpdateCallback();
                iter = listenerSets_.erase(iter);
            } else {
                iter++;
            }
        }

        if (env_ == nullptr) {
            return;
        }
        for (auto& item : cbList_) {
            napi_delete_reference(env_, item);
        }
        if (thisVarRef_ != nullptr) {
            napi_delete_reference(env_, thisVarRef_);
        }
    }

    static void NapiCallback(JsEngine* jsEngine)
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        MediaQueryer queryer;
        for (auto listener : listenerSets_[jsEngine]) {
            auto json = MediaQueryInfo::GetMediaQueryJsonInfo();
            listener->matches_ = queryer.MatchCondition(listener->media_, json);
            for (auto& cbRef : listener->cbList_) {
                napi_value thisVal = nullptr;
                napi_get_reference_value(listener->env_, listener->thisVarRef_, &thisVal);

                napi_value cb = nullptr;
                napi_get_reference_value(listener->env_, cbRef, &cb);

                napi_value resultArg = nullptr;
                listener->MediaQueryResult::NapiSerilizer(listener->env_, resultArg);

                napi_value result = nullptr;
                LOGI("NAPI MeidaQueryCallback call js");
                napi_call_function(listener->env_, thisVal, cb, 1, &resultArg, &result);
            }
        }
    }

    static napi_value On(napi_env env, napi_callback_info info)
    {
        LOGI("NAPI MeidaQuery On called");
        auto jsEngine = EngineHelper::GetCurrentEngine();
        if (!jsEngine) {
            LOGE("get jsEngine failed");
            return nullptr;
        }
        jsEngine->RegistMediaUpdateCallback(NapiCallback);

        napi_value thisVar = nullptr;
        napi_value cb = nullptr;
        size_t argc = ParseArgs(env, info, thisVar, cb);
        NAPI_ASSERT(env, (argc == 2 && thisVar != nullptr && cb != nullptr), "Invalid arguments");

        MediaQueryListener* listener = GetListener(env, thisVar);
        if (!listener) {
            LOGE("listener is null");
            return nullptr;
        }
        auto iter = listener->FindCbList(cb);
        if (iter != listener->cbList_.end()) {
            return nullptr;
        }
        napi_ref ref = nullptr;
        napi_create_reference(env, cb, 1, &ref);
        listener->cbList_.emplace_back(ref);

#if defined(WINDOWS_PLATFORM) || defined(MAC_PLATFORM)
        NapiCallback(AceType::RawPtr(jsEngine));
#endif

        return nullptr;
    }

    static napi_value Off(napi_env env, napi_callback_info info)
    {
        napi_value thisVar = nullptr;
        napi_value cb = nullptr;
        size_t argc = ParseArgs(env, info, thisVar, cb);
        MediaQueryListener* listener = GetListener(env, thisVar);
        if (!listener) {
            LOGE("listener is null");
            return nullptr;
        }
        if (argc == 1) {
            for (auto& item : listener->cbList_) {
                napi_delete_reference(listener->env_, item);
            }
            listener->cbList_.clear();
        } else {
            NAPI_ASSERT(env, (argc == 2 && listener != nullptr && cb != nullptr), "Invalid arguments");
            auto iter = listener->FindCbList(cb);
            if (iter != listener->cbList_.end()) {
                napi_delete_reference(listener->env_, *iter);
                listener->cbList_.erase(iter);
            }
        }
        auto jsEngine = EngineHelper::GetCurrentEngine();
        if (!jsEngine) {
            LOGE("get jsEngine failed");
            return nullptr;
        }
        const std::lock_guard<std::mutex> lock(mutex_);
        if (listenerSets_[AceType::RawPtr(jsEngine)].empty()) {
            jsEngine->UnregistMediaUpdateCallback();
            listenerSets_.erase(AceType::RawPtr(jsEngine));
        }
        return nullptr;
    }

    std::list<napi_ref>::iterator FindCbList(napi_value cb)
    {
        return std::find_if(cbList_.begin(), cbList_.end(), [env = env_, cb](const napi_ref& item) -> bool {
            bool result = false;
            napi_value refItem;
            napi_get_reference_value(env, item, &refItem);
            napi_strict_equals(env, refItem, cb, &result);
            return result;
        });
    }

    void NapiSerilizer(napi_env& env, napi_value& result) override
    {
        MediaQueryResult::NapiSerilizer(env, result);

        napi_wrap(
            env, result, this,
            [](napi_env env, void* data, void* hint) {
                MediaQueryListener* listener = static_cast<MediaQueryListener*>(data);
                if (listener != nullptr) {
                    delete listener;
                }
            },
            nullptr, nullptr);

        /* insert callback functions */
        const char* funName = "on";
        napi_value funcValue = nullptr;
        napi_create_function(env, funName, NAPI_AUTO_LENGTH, On, nullptr, &funcValue);
        napi_set_named_property(env, result, funName, funcValue);

        funName = "off";
        napi_create_function(env, funName, NAPI_AUTO_LENGTH, Off, nullptr, &funcValue);
        napi_set_named_property(env, result, funName, funcValue);
    }

private:
    void Initialize(napi_env env, napi_value thisVar)
    {
        if (env_ == nullptr) {
            env_ = env;
        }
        if (thisVarRef_ == nullptr) {
            napi_create_reference(env, thisVar, 1, &thisVarRef_);
        }
        auto jsEngine = EngineHelper::GetCurrentEngine();
        if (!jsEngine) {
            LOGE("get jsEngine failed");
            return;
        }
        const std::lock_guard<std::mutex> lock(mutex_);
        listenerSets_[AceType::RawPtr(jsEngine)].emplace(this);
    }

    static MediaQueryListener* GetListener(napi_env env, napi_value thisVar)
    {
        MediaQueryListener* listener = nullptr;
        napi_unwrap(env, thisVar, (void**)&listener);
        listener->Initialize(env, thisVar);
        return listener;
    }

    static size_t ParseArgs(napi_env& env, napi_callback_info& info, napi_value& thisVar, napi_value& cb)
    {
        const size_t argNum = 2;
        size_t argc = argNum;
        napi_value argv[argNum] = { 0 };
        void* data = nullptr;
        napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
        NAPI_ASSERT_BASE(env, argc > 0, "too few parameter", 0);

        napi_valuetype napiType;
        NAPI_CALL_BASE(env, napi_typeof(env, argv[0], &napiType), 0);
        NAPI_ASSERT_BASE(env, napiType == napi_string, "parameter 1 should be string", 0);
        char type[STR_BUFFER_SIZE] = { 0 };
        size_t len = 0;
        napi_get_value_string_utf8(env, argv[0], type, STR_BUFFER_SIZE, &len);
        NAPI_ASSERT_BASE(env, len < STR_BUFFER_SIZE, "condition string too long", 0);
        NAPI_ASSERT_BASE(env, strcmp("change", type) == 0, "type mismatch('change')", 0);

        if (argc <= 1) {
            return argc;
        }

        NAPI_CALL_BASE(env, napi_typeof(env, argv[1], &napiType), 0);
        NAPI_ASSERT_BASE(env, napiType == napi_function, "type mismatch for parameter 2", 0);
        cb = argv[1];
        return argc;
    }

    napi_ref thisVarRef_ = nullptr;
    napi_env env_ = nullptr;
    std::list<napi_ref> cbList_;
    static std::map<JsEngine*, std::set<MediaQueryListener*>> listenerSets_;
    static std::mutex mutex_;
};
std::map<JsEngine*, std::set<MediaQueryListener*>> MediaQueryListener::listenerSets_;
std::mutex MediaQueryListener::mutex_;

static napi_value JSMatchMediaSync(napi_env env, napi_callback_info info)
{
    /* Get arguments */
    size_t argc = 1;
    napi_value argv = nullptr;
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, &argv, &thisVar, &data));
    NAPI_ASSERT(env, argc == 1, "requires 1 parameter");

    /* Checkout arguments */
    napi_valuetype type;
    NAPI_CALL(env, napi_typeof(env, argv, &type));
    NAPI_ASSERT(env, type == napi_string, "type mismatch");
    char condition[STR_BUFFER_SIZE] = { 0 };
    size_t len = 0;
    napi_get_value_string_utf8(env, argv, condition, STR_BUFFER_SIZE, &len);
    NAPI_ASSERT(env, len < STR_BUFFER_SIZE, "condition string too long");

    /* construct object for query */
    std::string conditionStr(condition, len);
    auto mediaFeature = MediaQueryInfo::GetMediaQueryJsonInfo();
    MediaQueryer queryer;
    bool matchResult = queryer.MatchCondition(conditionStr, mediaFeature);
    MediaQueryListener* listener = new MediaQueryListener(matchResult, conditionStr);
    napi_value result = nullptr;
    listener->NapiSerilizer(env, result);
    return result;
}

static napi_value Export(napi_env env, napi_value exports)
{
    napi_property_descriptor properties[] = { DECLARE_NAPI_FUNCTION("matchMediaSync", JSMatchMediaSync) };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(properties) / sizeof(properties[0]), properties));
    return exports;
}

static napi_module media_query_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Export,
    .nm_modname = "mediaquery", // relative to the dynamic library's name
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void Register()
{
    napi_module_register(&media_query_module);
}

} // namespace OHOS::Ace::Napi
