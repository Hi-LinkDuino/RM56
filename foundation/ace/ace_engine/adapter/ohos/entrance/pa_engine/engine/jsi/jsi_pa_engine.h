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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_JSI_JSI_PA_ENGINE_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_JSI_JSI_PA_ENGINE_H

#include <vector>

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/impl/ark/ark_native_engine.h"
#include "result_set.h"
#include "uri.h"
#include "values_bucket.h"
#include "want.h"

#include "adapter/ohos/entrance/pa_engine/backend_delegate.h"
#include "adapter/ohos/entrance/pa_engine/engine/common/js_backend_engine.h"
#include "base/memory/ace_type.h"
#include "base/utils/noncopyable.h"
#include "core/common/js_message_dispatcher.h"
#include "frameworks/bridge/js_frontend/engine/jsi/js_runtime.h"

namespace OHOS::Ace {

using namespace OHOS::Ace::Framework;

// Each JsFrontend holds only one JsiPaEngineInstance.
class JsiPaEngineInstance final : public AceType, public JsBackendEngineInstance {
public:
    explicit JsiPaEngineInstance(const RefPtr<BackendDelegate>& delegate, int32_t instanceId)
        : backendDelegate_(delegate), instanceId_(instanceId)
    {}
    ~JsiPaEngineInstance() override;

    bool InitJsEnv(bool debuggerMode, const std::unordered_map<std::string, void*>& extraNativeObject);
    void CallJs(const std::string& callbackId, const std::string& args, bool keepAlive = false, bool isGlobal = false);
    bool FireJsEvent(const std::string& eventId);
    bool CallPlatformFunction(const std::string& channel, std::vector<uint8_t>&& data, int32_t id);
    bool PluginErrorCallback(int32_t callbackId, int32_t errorCode, std::string&& errorMessage);

    RefPtr<BackendDelegate> GetDelegate() const;
    std::shared_ptr<JsRuntime> GetJsRuntime() const;
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher);
    void SetArkNativeEngine(ArkNativeEngine* nativeEngine);
    ArkNativeEngine* GetArkNativeEngine() const;

    // add Console object to worker
    void RegisterConsoleModule(ArkNativeEngine* engine);

    bool GetBlockWaiting() const
    {
        return blockWaiting_;
    }

    void SetBlockWaiting(bool blockWaiting)
    {
        blockWaiting_ = blockWaiting;
    }

    shared_ptr<JsValue> GetAsyncResult() const
    {
        return asyncResult_;
    }

    void SetAsyncResult(shared_ptr<JsValue> asyncResult)
    {
        asyncResult_ = asyncResult;
    }

    void SetDebugMode(bool isDebugMode)
    {
        isDebugMode_ = isDebugMode;
    }

private:
    void RegisterPaModule();
    void RegisterConsoleModule();
    void EvaluateJsCode();

    std::shared_ptr<JsRuntime> runtime_;
    RefPtr<BackendDelegate> backendDelegate_;
    int32_t instanceId_ = 0;
    WeakPtr<JsMessageDispatcher> dispatcher_;
    ArkNativeEngine* nativeEngine_ = nullptr;
    bool blockWaiting_ = false;
    shared_ptr<JsValue> asyncResult_ = nullptr;
    bool isDebugMode_ = true;

    ACE_DISALLOW_COPY_AND_MOVE(JsiPaEngineInstance);
};

using RdbValueBucketNewInstance = napi_value (*)(napi_env env, OHOS::NativeRdb::ValuesBucket& valueBucket);
using RdbValueBucketGetNativeObject = OHOS::NativeRdb::ValuesBucket* (*)(napi_env env, napi_value& value);
using RdbResultSetProxyNewInstance = napi_value (*)(napi_env env, OHOS::NativeRdb::AbsSharedResultSet* resultSet);
using RdbResultSetProxyGetNativeObject = OHOS::NativeRdb::AbsSharedResultSet* (*)(const napi_env& env,
    const napi_value& arg);
using DataAbilityPredicatesNewInstance = napi_value (*)(
    napi_env env, OHOS::NativeRdb::DataAbilityPredicates* predicates);
using DataAbilityPredicatesGetNativeObject = OHOS::NativeRdb::DataAbilityPredicates* (*)(const napi_env env,
    const napi_value& arg);

class JsiPaEngine : public JsBackendEngine {
public:
    explicit JsiPaEngine(int32_t instanceId) : instanceId_(instanceId) {}
    ~JsiPaEngine() override;

    bool Initialize(const RefPtr<BackendDelegate>& delegate) override;
    // Load and initialize a JS bundle into the JS Framework
    void LoadJs(const std::string& url, const OHOS::AAFwk::Want& want) override;
    // Fire AsyncEvent on JS
    void FireAsyncEvent(const std::string& eventId, const std::string& param) override;
    // Fire SyncEvent on JS
    void FireSyncEvent(const std::string& eventId, const std::string& param) override;

    RefPtr<GroupJsBridge> GetGroupJsBridge() override;
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) override;

    // destroy application instance according packageName
    void DestroyApplication(const std::string& packageName) override;
    void OnCommandApplication(const std::string& intent, int startId) override;

    // data
    int32_t Insert(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value) override;
    int32_t BatchInsert(const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values) override;
    std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> Query(const Uri& uri, const std::vector<std::string>& columns,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates) override;
    int32_t Update(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates) override;
    int32_t Delete(const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates) override;

    std::string GetType(const Uri& uri) override;
    std::vector<std::string> GetFileTypes(const Uri& uri, const std::string& mimeTypeFilter) override;
    int32_t OpenFile(const Uri& uri, const std::string& mode) override;
    int32_t OpenRawFile(const Uri& uri, const std::string& mode) override;
    Uri NormalizeUri(const Uri& uri) override;
    Uri DenormalizeUri(const Uri& uri) override;

    // service
    sptr<IRemoteObject> OnConnectService(const OHOS::AAFwk::Want& want) override;
    void OnDisconnectService(const OHOS::AAFwk::Want& want) override;
    void OnCommand(const OHOS::AAFwk::Want &want, int startId) override;

    // form
    void OnCreate(const OHOS::AAFwk::Want &want) override;
    void OnDelete(const int64_t formId) override;
    void OnTriggerEvent(const int64_t formId, const std::string& message) override;
    void OnUpdate(const int64_t formId) override;
    void OnCastTemptoNormal(const int64_t formId) override;
    void OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap) override;
    void OnAcquireState(const OHOS::AAFwk::Want &want) override;

private:
    void SetPostTask(NativeEngine* nativeEngine);
    void LoadLibrary();
    void UnloadLibrary();
    shared_ptr<JsValue> GetPaFunc(const std::string& funcName);
    shared_ptr<JsValue> CallFunc(const shared_ptr<JsValue>& func);
    shared_ptr<JsValue> CallFunc(const shared_ptr<JsValue>& func, const std::vector<shared_ptr<JsValue>>& argv);
    shared_ptr<JsValue> CallAsyncFunc(const shared_ptr<JsValue>& func, std::vector<shared_ptr<JsValue>>& argv);
    shared_ptr<JsValue> NativeValueToJsValue(NativeValue* nativeValue);
    shared_ptr<JsValue> WantToJsValue(const OHOS::AAFwk::Want& want);
    void StartService();
    void StartData();
    void StartForm(const OHOS::AAFwk::Want& want);

    void RegisterWorker();
    void RegisterInitWorkerFunc();
    void RegisterAssetFunc();

    int32_t instanceId_ = 0;
    ArkNativeEngine* nativeEngine_ = nullptr;
    RefPtr<JsiPaEngineInstance> engineInstance_;

    void* libRdb_ = nullptr;
    void* libDataAbility_ = nullptr;
    RdbValueBucketNewInstance rdbValueBucketNewInstance_ = nullptr;
    RdbValueBucketGetNativeObject rdbValueBucketGetNativeObject_ = nullptr;
    RdbResultSetProxyNewInstance rdbResultSetProxyNewInstance_ = nullptr;
    RdbResultSetProxyGetNativeObject rdbResultSetProxyGetNativeObject_ = nullptr;
    DataAbilityPredicatesNewInstance dataAbilityPredicatesNewInstance_ = nullptr;
    DataAbilityPredicatesGetNativeObject dataAbilityPredicatesGetNativeObject_ = nullptr;

    ACE_DISALLOW_COPY_AND_MOVE(JsiPaEngine);
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_JSI_JSI_PA_ENGINE_H
