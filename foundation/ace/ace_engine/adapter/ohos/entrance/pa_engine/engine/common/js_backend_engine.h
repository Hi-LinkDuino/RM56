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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_COMMON_JS_BACKEND_ENGINE_H
#define FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_COMMON_JS_BACKEND_ENGINE_H

#include <string>
#include <unordered_map>

#include "abs_shared_result_set.h"
#include "data_ability_predicates.h"
#include "form_provider_info.h"
#include "iremote_object.h"
#include "values_bucket.h"
#include "want.h"

#include "adapter/ohos/entrance/pa_engine/backend_delegate.h"
#include "core/common/backend.h"

namespace OHOS::Ace {

class JsBackendEngineInstance {
public:
    JsBackendEngineInstance() = default;
    virtual ~JsBackendEngineInstance() = default;
};

class JsBackendEngine : public AceType {
    DECLARE_ACE_TYPE(JsBackendEngine, AceType);

public:
    JsBackendEngine() = default;
    virtual ~JsBackendEngine() = default;

    virtual bool Initialize(const RefPtr<BackendDelegate>& delegate) = 0;

    virtual void LoadJs(const std::string& url, const OHOS::AAFwk::Want& want) = 0;

    virtual void SetJsMessageDispatcher(const RefPtr<JsMessageDispatcher>& dispatcher) = 0;

    virtual void FireAsyncEvent(const std::string& eventId, const std::string& param) = 0;

    virtual void FireSyncEvent(const std::string& eventId, const std::string& param) = 0;

    virtual void DestroyApplication(const std::string& packageName) = 0;

    virtual int32_t Insert(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value) = 0;
    virtual std::shared_ptr<OHOS::NativeRdb::AbsSharedResultSet> Query(const Uri& uri,
        const std::vector<std::string>& columns, const OHOS::NativeRdb::DataAbilityPredicates& predicates) = 0;
    virtual int32_t Update(const Uri& uri, const OHOS::NativeRdb::ValuesBucket& value,
        const OHOS::NativeRdb::DataAbilityPredicates& predicates) = 0;
    virtual int32_t Delete(const Uri& uri, const OHOS::NativeRdb::DataAbilityPredicates& predicates) = 0;

    virtual int32_t BatchInsert(const Uri& uri, const std::vector<OHOS::NativeRdb::ValuesBucket>& values) = 0;
    virtual std::string GetType(const Uri& uri) = 0;
    virtual std::vector<std::string> GetFileTypes(const Uri& uri, const std::string& mimeTypeFilter) = 0;
    virtual int32_t OpenFile(const Uri& uri, const std::string& mode) = 0;
    virtual int32_t OpenRawFile(const Uri& uri, const std::string& mode) = 0;
    virtual Uri NormalizeUri(const Uri& uri) = 0;
    virtual Uri DenormalizeUri(const Uri& uri) = 0;
    virtual sptr<IRemoteObject> OnConnectService(const OHOS::AAFwk::Want& want) = 0;
    virtual void OnDisconnectService(const OHOS::AAFwk::Want& want) = 0;
    virtual void OnCreate(const OHOS::AAFwk::Want& want) = 0;
    virtual void OnDelete(const int64_t formId) = 0;
    virtual void OnTriggerEvent(const int64_t formId, const std::string& message) = 0;
    virtual void OnUpdate(const int64_t formId) = 0;
    virtual void OnCastTemptoNormal(const int64_t formId) = 0;
    virtual void OnVisibilityChanged(const std::map<int64_t, int32_t>& formEventsMap) = 0;
    virtual void OnAcquireState(const OHOS::AAFwk::Want &want) = 0;
    virtual void OnCommand(const OHOS::AAFwk::Want &want, int startId) = 0;

    void SetFormData(const AppExecFwk::FormProviderData &formProviderData)
    {
        formProviderData_ = formProviderData;
    }

    AppExecFwk::FormProviderData GetFormData() const
    {
        return formProviderData_;
    }

    virtual void OnCommandApplication(const std::string& intent, int startId) = 0;

    virtual RefPtr<GroupJsBridge> GetGroupJsBridge() = 0;

    void AddExtraNativeObject(const std::string& key, void* value)
    {
        extraNativeObject_[key] = value;
    }

    const std::unordered_map<std::string, void*>& GetExtraNativeObject() const
    {
        return extraNativeObject_;
    }

    bool IsDebugVersion() const
    {
        return isDebugVersion_;
    }

    void SetDebugVersion(bool value)
    {
        isDebugVersion_ = value;
    }

    bool NeedDebugBreakPoint() const
    {
        return needDebugBreakPoint_;
    }

    void SetNeedDebugBreakPoint(bool value)
    {
        needDebugBreakPoint_ = value;
    }

private:
    std::string instanceName_;
    AppExecFwk::FormProviderData formProviderData_;
    std::unordered_map<std::string, void*> extraNativeObject_;
    // weather the app has debugger.so.
    bool isDebugVersion_ = false;
    // if debug, '-D' means need debug breakpoint, by default, do not enter breakpoint.
    bool needDebugBreakPoint_ = false;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_ADAPTER_OHOS_ENTRANCE_PA_ENGINE_ENGINE_COMMON_JS_BACKEND_ENGINE_H
