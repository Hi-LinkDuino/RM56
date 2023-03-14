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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_BACKEND_DELEGATE_IMPL_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_BACKEND_DELEGATE_IMPL_H

#include <future>
#include <mutex>
#include <unordered_map>

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "form_provider_info.h"
#include "iremote_object.h"
#include "values_bucket.h"
#include "want.h"

#include "adapter/ohos/entrance/pa_engine/backend_delegate.h"
#include "base/memory/ace_type.h"
#include "base/thread/cancelable_callback.h"
#include "core/common/backend.h"
#include "core/common/js_message_dispatcher.h"
#include "core/pipeline/pipeline_context.h"
#include "frameworks/bridge/common/manifest/manifest_parser.h"

namespace OHOS::Ace {

using LoadJsCallback = std::function<void(const std::string& url, const OHOS::AAFwk::Want& want)>;
using JsMessageDispatcherSetterCallback = std::function<void(const RefPtr<JsMessageDispatcher>&)>;
using PaEventCallback = std::function<void(const std::string&, const std::string&)>;
using DestroyApplicationCallback = std::function<void(const std::string& packageName)>;
using InsertCallback = std::function<int32_t(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value)>;
using QueryCallback = std::function<std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet>(
    const Uri& uri, const std::vector<std::string>& columns, const OHOS::NativeRdb::DataAbilityPredicates& predicates)>;
using UpdateCallback = std::function<int32_t(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
    const OHOS::NativeRdb::DataAbilityPredicates& predicates)>;
using DeleteCallback = std::function<int32_t(const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates)>;
using BatchInsertCallback =
    std::function<int32_t(const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values)>;
using GetTypeCallback = std::function<std::string(const Uri& uri)>;
using GetFileTypesCallback = std::function<std::vector<std::string>(const Uri& uri, const std::string& mimeTypeFilter)>;
using OpenFileCallback = std::function<int32_t(const Uri& uri, const std::string& mode)>;
using OpenRawFileCallback = std::function<int32_t(const Uri& uri, const std::string& mode)>;
using NormalizeUriCallback = std::function<Uri(const Uri& uri)>;
using DenormalizeUriCallback = std::function<Uri(const Uri& uri)>;
using ConnectCallback = std::function<sptr<IRemoteObject>(const OHOS::AAFwk::Want& want)>;
using DisConnectCallback = std::function<void(const OHOS::AAFwk::Want& want)>;
using CreateFormCallback = std::function<void(const OHOS::AAFwk::Want& want)>;
using DeleteFormCallback = std::function<void(const int64_t formId)>;
using TriggerEventCallback = std::function<void(const int64_t formId, const std::string& message)>;
using UpdateFormCallback = std::function<void(const int64_t formId)>;
using CastTemptoNormalCallback = std::function<void(const int64_t formId)>;
using VisibilityChangedCallback = std::function<void(const std::map<int64_t, int32_t>& formEventsMap)>;
using AcquireStateCallback = std::function<void(const OHOS::AAFwk::Want &want)>;
using CommandCallback = std::function<void(const OHOS::AAFwk::Want &want, int startId)>;
using CommandApplicationCallback = std::function<void(const std::string& intent, int startId)>;

struct BackendDelegateImplBuilder {
    RefPtr<TaskExecutor> taskExecutor;
    LoadJsCallback loadCallback;
    PaEventCallback asyncEventCallback;
    PaEventCallback syncEventCallback;
    JsMessageDispatcherSetterCallback transferCallback;
    DestroyApplicationCallback destroyApplicationCallback;
    InsertCallback insertCallback;
    QueryCallback queryCallback;
    UpdateCallback updateCallback;
    DeleteCallback deleteCallback;
    BatchInsertCallback batchInsertCallback;
    GetTypeCallback getTypeCallback;
    GetFileTypesCallback getFileTypesCallback;
    OpenFileCallback openFileCallback;
    OpenRawFileCallback openRawFileCallback;
    NormalizeUriCallback normalizeUriCallback;
    DenormalizeUriCallback denormalizeUriCallback;
    ConnectCallback connectCallback;
    DisConnectCallback disConnectCallback;
    CreateFormCallback createFormCallback;
    DeleteFormCallback deleteFormCallback;
    TriggerEventCallback triggerEventCallback;
    UpdateFormCallback updateFormCallback;
    CastTemptoNormalCallback castTemptoNormalCallback;
    VisibilityChangedCallback visibilityChangedCallback;
    AcquireStateCallback acquireStateCallback;
    CommandCallback commandCallback;
    CommandApplicationCallback commandApplicationCallback;
    BackendType type;
};

class BackendDelegateImpl : public BackendDelegate {
    DECLARE_ACE_TYPE(BackendDelegateImpl, BackendDelegate);

public:
    explicit BackendDelegateImpl(const BackendDelegateImplBuilder& builder);
    ~BackendDelegateImpl() override = default;

    // JsBackend delegate functions.
    void RunPa(const std::string& url, const OHOS::AAFwk::Want& want);
    void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) const;

    // BackendDelegate overrides.
    void PostJsTask(std::function<void()>&& task) override;

    void PostDelayedJsTask(std::function<void()>&& task, uint32_t delayTime) override;

    SingleTaskExecutor GetAnimationJsTask() override;

    void AddTaskObserver(std::function<void()>&& task) override;
    void RemoveTaskObserver() override;

    BackendType GetType() const override
    {
        return type_;
    }

    const RefPtr<GroupJsBridge>& GetGroupJsBridge() override
    {
        return groupJsBridge_;
    }

    void SetGroupJsBridge(const RefPtr<GroupJsBridge>& groupJsBridge)
    {
        groupJsBridge_ = groupJsBridge;
    }

    void SetCallBackResult(const std::string& callBackId, const std::string& result) override;

    bool GetAssetContent(const std::string& url, std::string& content) override;
    bool GetAssetContent(const std::string& url, std::vector<uint8_t>& content) override;
    bool GetResourceData(const std::string& fileUri, std::vector<uint8_t>& content, std::string& ami) override;
    std::string GetAssetPath(const std::string& url) override;

    // JsEventHandler delegate functions.
    void FireAsyncEvent(const std::string& eventId, const std::string& param, const std::string& jsonArgs);
    bool FireSyncEvent(const std::string& eventId, const std::string& param, const std::string& jsonArgs);
    void FireSyncEvent(
        const std::string& eventId, const std::string& param, const std::string& jsonArgs, std::string& result);

    // special JsEventHandler
    void OnApplicationDestroy(const std::string &packageName);
    sptr<IRemoteObject> OnConnect(const OHOS::AAFwk::Want &want);
    void OnDisConnect(const OHOS::AAFwk::Want &want);
    void OnCommand(const OHOS::AAFwk::Want &want, int startId);
    void OnApplicationCommand(const std::string& intent, int startId);

    void TransferJsResponseData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const;
    void TransferJsPluginGetError(int32_t callbackId, int32_t errorCode, std::string&& errorMessage) const;
    void TransferJsEventData(int32_t callbackId, int32_t code, std::vector<uint8_t>&& data) const;
    void LoadPluginJsCode(std::string&& jsCode) const;
    void LoadPluginJsByteCode(std::vector<uint8_t>&& jsCode, std::vector<int32_t>&& jsCodeLen) const;

    void MethodChannel(const std::string& methodName, std::string& jsonStr);

    int32_t Insert(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value);
    std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> Query(const Uri& uri, const std::vector<std::string>& columns,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates);
    int32_t Update(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates);
    int32_t Delete(const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates);

    int32_t BatchInsert(const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values);
    std::string GetType(const Uri& uri);
    std::vector<std::string> GetFileTypes(const Uri& uri, const std::string& mimeTypeFilter);
    int32_t OpenFile(const Uri& uri, const std::string& mode);
    int32_t OpenRawFile(const Uri& uri, const std::string& mode);
    Uri NormalizeUri(const Uri& uri);
    Uri DenormalizeUri(const Uri& uri);

    void OnCreate(const OHOS::AAFwk::Want &want);
    void OnDelete(const int64_t formId);
    void OnTriggerEvent(const int64_t formId, const std::string& message);
    void OnUpdate(const int64_t formId);
    void OnCastTemptoNormal(const int64_t formId);
    void OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap);
    void OnAcquireState(const OHOS::AAFwk::Want& want);

private:
    void LoadPa(const std::string& url, const OHOS::AAFwk::Want& want);
    bool ParseFileUri(
        const RefPtr<AssetManager>& assetManager, const std::string& fileUri, std::string& assetsFilePath);

    void ParseManifest();
    int32_t callbackCnt_ = 0;
    bool isStagingPageExist_ = false;
    std::unordered_map<int32_t, std::string> jsCallBackResult_;

    LoadJsCallback loadJs_;
    JsMessageDispatcherSetterCallback dispatcherCallback_;

    PaEventCallback asyncEvent_;
    PaEventCallback syncEvent_;

    InsertCallback insert_;
    QueryCallback query_;
    UpdateCallback update_;
    DeleteCallback delete_;
    BatchInsertCallback batchInsert_;
    GetTypeCallback getType_;
    GetFileTypesCallback getFileTypes_;
    OpenFileCallback openFile_;
    OpenRawFileCallback openRawFile_;
    NormalizeUriCallback normalizeUri_;
    DenormalizeUriCallback denormalizeUri_;

    DestroyApplicationCallback destroyApplication_;
    CommandApplicationCallback commandApplication_;
    ConnectCallback connectCallback_;
    DisConnectCallback disConnectCallback_;

    CreateFormCallback createCallback_;
    DeleteFormCallback deleteCallback_;
    TriggerEventCallback triggerEventCallback_;
    UpdateFormCallback updateCallback_;
    CastTemptoNormalCallback castTemptoNormalCallback_;
    VisibilityChangedCallback visibilityChangedCallback_;
    AcquireStateCallback acquireStateCallback_;
    CommandCallback commandCallback_;

    RefPtr<Framework::ManifestParser> manifestParser_;
    BackendType type_ = BackendType::SERVICE;

    RefPtr<TaskExecutor> taskExecutor_;
    RefPtr<GroupJsBridge> groupJsBridge_;
    mutable std::mutex mutex_;
    mutable std::once_flag onceFlag_;

    std::mutex LoadPaMutex_;
    std::condition_variable condition_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_BACKEND_DELEGATE_IMPL_H
